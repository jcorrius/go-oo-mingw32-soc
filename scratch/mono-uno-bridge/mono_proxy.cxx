/*************************************************************************
 *
 *  $RCSfile: $
 *
 *  $Revision: $
 *
 *  last change: $Author: $ $Date: $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "mono_bridge.h"

#include "rtl/ustring.h"
#include "uno/dispatcher.h"
#include "uno/environment.h"
#include "typelib/typedescription.h"

extern "C" {
#include "mono/metadata/threads.h"
}

using namespace mono_uno;

extern "C" {

static void SAL_CALL mono_proxy_free( uno_ExtEnvironment * env, void * proxy)
{
    MonoProxy * monoProxy = reinterpret_cast< MonoProxy * >( proxy );

    delete monoProxy;
}

uno_Interface * SAL_CALL cli_uno_environment_createMonoProxyAndRegister(
    uno_ExtEnvironment *pUnoEnv, void *pMonoProxy, rtl_uString *pOid,
    typelib_TypeDescription *pTD )
    SAL_THROW_EXTERN_C()
{
    uno_Interface * proxy = static_cast< uno_Interface * >(
        new MonoProxy( pUnoEnv,
                       static_cast< guint32 >( reinterpret_cast< sal_IntPtr >( pMonoProxy ) ),
                       pOid, pTD ) );

    pUnoEnv->registerProxyInterface(
        pUnoEnv,
        reinterpret_cast< void ** >( &proxy ),
        mono_proxy_free,
        pOid,
        (typelib_InterfaceTypeDescription*) pTD );

    return proxy;
}

static void SAL_CALL mono_proxy_acquire( uno_Interface * pUnoI )
{
    MonoProxy const * that = static_cast< MonoProxy const * >( pUnoI );
    that->acquire();
}

static void SAL_CALL mono_proxy_release( uno_Interface * pUnoI )
{
    MonoProxy const * that = static_cast< MonoProxy const * >( pUnoI );
    that->release();
}

static void SAL_CALL mono_proxy_dispatch(
    uno_Interface * pUnoI, typelib_TypeDescription const * member_td,
    void * uno_ret, void * uno_args [], uno_Any ** uno_exc )
    SAL_THROW_EXTERN_C()
{
    MonoProxy * that = static_cast< MonoProxy * >( pUnoI );
    that->dispatch( member_td, uno_ret, uno_args, uno_exc );
}

} // extern "C"

namespace mono_uno
{

MonoProxy::MonoProxy(uno_ExtEnvironment * pUnoEnv, guint32 managedProxy,
                     rtl_uString *pOid, typelib_TypeDescription * pTD):
    m_ref(1),
    m_unoEnv(pUnoEnv),
    m_managedProxy(managedProxy), // FIXME free this in the destructor?
    m_Oid(pOid),
    m_unoType(pTD)
{
    uno_Interface::acquire = mono_proxy_acquire;
    uno_Interface::release = mono_proxy_release;
    uno_Interface::pDispatcher = mono_proxy_dispatch;

    MonoObject * pObj = mono_gchandle_get_target( m_managedProxy );
    MonoClass * pClass = mono_object_get_class( pObj );
    MonoMethodDesc * pMethodDesc = mono_method_desc_new( "ManagedProxy:Dispatch", FALSE );
    m_managedDispatch = mono_method_desc_search_in_class( pMethodDesc, pClass );
    mono_method_desc_free( pMethodDesc );
    OSL_ASSERT( 0 != m_managedDispatch );
}

inline void MonoProxy::acquire() const
{
    if (1 == osl_incrementInterlockedCount( &m_ref ))
    {
        // rebirth of proxy zombie
        void * that = const_cast< MonoProxy * >( this );
        // register at uno env
        (*m_unoEnv->registerProxyInterface)(
            m_unoEnv, &that,
            mono_proxy_free, m_Oid.pData,
            (typelib_InterfaceTypeDescription *)m_unoType.get() );
#if OSL_DEBUG_LEVEL >= 2
        OSL_ASSERT( this == (void const * const)that );
#endif
    }
}

inline void MonoProxy::release() const
{
    if (0 == osl_decrementInterlockedCount( &m_ref ))
    {
        // revoke from uno env on last release,
        // The proxy can be resurrected if acquire is called before the uno
        // environment calls mono_proxy_free. mono_proxy_free will
        //delete the proxy. The environment does not acquire a registered
        //proxy.
        (*m_unoEnv->revokeInterface)(
            m_unoEnv, const_cast< MonoProxy * >( this ) );
    }
}

inline void MonoProxy::dispatch( typelib_TypeDescription const * member_td,
                                 void * uno_ret, void * uno_args [], uno_Any ** uno_exc )
{
    OSL_ASSERT( m_managedDispatch != 0 );
    
    gpointer pMonoParams[4];

    pMonoParams[0] = const_cast< typelib_TypeDescription * >(member_td);
    pMonoParams[1] = uno_ret;
    pMonoParams[2] = uno_args;
    pMonoParams[3] = uno_exc;

	MonoObject *obj = mono_gchandle_get_target( m_managedProxy );

    mono_uno::runtime_invoke( m_managedDispatch,
							  obj, pMonoParams, NULL,
							  mono_object_get_domain( obj ) );
}

}

