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

#include "uno/dispatcher.h"
#include "uno/environment.h"
#include "uno/lbnames.h"

#include "osl/diagnose.h"
#include "rtl/unload.h"
#include "rtl/ustring.hxx"

#include "uno/mapping.hxx"

extern "C" {
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/object.h"
#include "mono/metadata/threads.h"
}

#include "mono_bridge.h"

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )

using namespace ::rtl;
using namespace ::mono_uno;

extern "C" {

void SAL_CALL Mapping_acquire( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    static_cast< Mapping const * >( mapping )->m_bridge->acquire();
}

void SAL_CALL Mapping_release( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    static_cast< Mapping const * >( mapping )->m_bridge->release();
}

void SAL_CALL Mapping_map_to_uno(
    uno_Mapping * mapping, void ** ppOut,
    void * pIn, typelib_InterfaceTypeDescription * td )
    SAL_THROW_EXTERN_C()
{
    uno_Interface ** ppUnoI = (uno_Interface **)ppOut;
    void * monoI = pIn;

// FIXME do this here? OSL_ASSERT( sizeof (void *) >= sizeof (guint32))
    OSL_ENSURE( ppUnoI && td, "### null ptr!" );

    if (0 != *ppUnoI)
    {
        uno_Interface * pUnoI = *ppUnoI;
        (*pUnoI->release)( pUnoI );
        *ppUnoI = 0;
    }

    try
    {
        Bridge const *bridge =
            static_cast< Mapping const * >( mapping )->m_bridge;
        // FIXME any wrapper necessary around mono calls? cf. JNI_guarded_context
        uno_Interface * pUnoI = bridge->map_to_uno(
            monoI, (typelib_TypeDescription *)td );
        *ppUnoI = pUnoI;
    }
    catch (BridgeRuntimeError & err)
    {
#if OSL_DEBUG_LEVEL > 0
        OString cstr_msg(
            OUStringToOString(
                OUSTR("[mono_uno bridge error] ") + err.m_message,
                RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif    
    }
}

void SAL_CALL Mapping_map_to_mono(
    uno_Mapping * mapping, void ** ppOut,
    void * pIn, typelib_InterfaceTypeDescription * td )
    SAL_THROW_EXTERN_C()
{
    void ** pMonoI = ppOut;
    uno_Interface * pUnoI = (uno_Interface *)pIn;

    OSL_ENSURE( ppOut && td, "### null ptr!" );

    if (0 != *pMonoI)
    {
        // FIXME JNI bridge has guarded_context here
        // FIXME: do the right thing in the managed bridge
        // mono_gchandle_free( *pMonoI );
    }

    try
    {
        if (0 != pUnoI)
        {
            Bridge const * bridge =
                static_cast< Mapping const *>( mapping )->m_bridge;
            // FIXME guarded context
             *ppOut = (void *)bridge->map_to_mono(
                 pUnoI, (typelib_TypeDescription *)td );
             OSL_ASSERT( ppOut && *ppOut );
        }
    }
    catch (BridgeRuntimeError & err)
    {
#if OSL_DEBUG_LEVEL >= 1
        rtl::OString cstr_msg(
            rtl::OUStringToOString(
                OUSTR("[mono_uno bridge error] ") + err.m_message,
                RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }
}

void SAL_CALL Bridge_free( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    Mapping * that = static_cast< Mapping * >( mapping );
    delete that->m_bridge;
}

} // extern "C"

rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

namespace mono_uno
{

void Bridge::acquire() const SAL_THROW( () )
{
    if (1 == osl_incrementInterlockedCount( &m_ref ))
    {
        if (m_registered_mono2uno)
        {
            uno_Mapping * mapping = const_cast< Mapping * >( &m_mono2uno );
            uno_registerMapping(
                &mapping, Bridge_free,
                m_mono_env, (uno_Environment *)m_uno_env, 0 );
        }
        else
        {
            uno_Mapping * mapping = const_cast< Mapping * >( &m_uno2mono );
            uno_registerMapping(
                &mapping, Bridge_free,
                (uno_Environment *)m_uno_env, m_mono_env, 0 );
        }
    }
}

void Bridge::release() const SAL_THROW( () )
{
    if (! osl_decrementInterlockedCount( &m_ref ))
    {
        uno_revokeMapping(
            m_registered_mono2uno
            ? const_cast< Mapping * >( &m_mono2uno )
            : const_cast< Mapping * >( &m_uno2mono ) );
    }
}

Bridge::Bridge(
    uno_Environment * mono_env, uno_ExtEnvironment * uno_env,
    bool registered_mono2uno )
    : m_ref( 1 ),
      m_uno_env( uno_env ),
      m_mono_env( mono_env ),
      m_registered_mono2uno( registered_mono2uno )
{
    MonoDomain * pDomain = mono_get_root_domain();
    // FIXME where is this freed?
    MonoAssembly * pAssembly = mono_domain_assembly_open( pDomain, "cli_uno_bridge.dll" );
    // FIXME and this, is this needed later?
    MonoClass * pClass = mono_class_from_name (
        (MonoImage *)mono_assembly_get_image( pAssembly ), "com.sun.star.bridges.mono_uno", "Bridge" );
    OSL_ASSERT( 0 != pClass );
    /* FIXME add args to method description string */
    MonoMethodDesc * pMethodDesc = mono_method_desc_new( ":.ctor", FALSE );
    MonoMethod * pCtor = mono_method_desc_search_in_class( pMethodDesc, pClass );
    mono_method_desc_free( pMethodDesc );
    OSL_ASSERT( 0 != pCtor );

    pMethodDesc = mono_method_desc_new( "Bridge:MapManagedToUno", FALSE );
    m_mapManagedToUnoMethod = mono_method_desc_search_in_class( pMethodDesc, pClass );
    mono_method_desc_free( pMethodDesc );
    OSL_ASSERT( 0 != m_mapManagedToUnoMethod );

    pMethodDesc = mono_method_desc_new( "Bridge:MapUnoToManaged", FALSE );
    m_mapUnoToManagedMethod = mono_method_desc_search_in_class( pMethodDesc, pClass );
    mono_method_desc_free( pMethodDesc );
    OSL_ASSERT( 0 != m_mapUnoToManagedMethod );

    gpointer pParams[1];
    pParams[0] = &uno_env;
    m_managedBridge = mono_object_new( pDomain, pClass );
    mono_uno::runtime_invoke( pCtor, m_managedBridge, pParams, NULL,
							  mono_object_get_domain( m_managedBridge ) );

    OSL_ASSERT( 0 != m_mono_env && 0 != m_uno_env );
    (*((uno_Environment *)m_uno_env)->acquire)( (uno_Environment *)m_uno_env );
    (*m_mono_env->acquire)( m_mono_env );

    // mono2uno mapping
    m_mono2uno.acquire = Mapping_acquire;
    m_mono2uno.release = Mapping_release;
    m_mono2uno.mapInterface = Mapping_map_to_uno;
    m_mono2uno.m_bridge = this;
    // uno2mono mapping
    m_uno2mono.acquire = Mapping_acquire;
    m_uno2mono.release = Mapping_release;
    m_uno2mono.mapInterface = Mapping_map_to_mono;
    m_uno2mono.m_bridge = this;

    (*g_moduleCount.modCnt.acquire)( &g_moduleCount.modCnt );
}

Bridge::~Bridge() SAL_THROW( () )
{
    (*m_mono_env->release)( m_mono_env );
    (*((uno_Environment *)m_uno_env)->release)( (uno_Environment *)m_uno_env );
    // FIXME release managed bridge

    (*g_moduleCount.modCnt.release)( &g_moduleCount.modCnt );
}

void * Bridge::map_to_mono(
    uno_Interface *pUnoI, typelib_TypeDescription * pTD ) const
{
    gpointer pMonoParams[2];

    pMonoParams[0] = &pUnoI;
    pMonoParams[1] = pTD;
    
    return
        mono_uno::runtime_invoke( m_mapUnoToManagedMethod,
								  m_managedBridge, pMonoParams, NULL,
								  mono_object_get_domain( m_managedBridge ) );
}

uno_Interface * Bridge::map_to_uno(
    void * pMonoI, typelib_TypeDescription * pTD ) const
{
    gpointer pMonoParams[2];
    uno_Interface ** ppResult;

    pMonoParams[0] = pMonoI;
    pMonoParams[1] = pTD;

    ppResult = (uno_Interface **)mono_object_unbox(
        mono_uno::runtime_invoke( m_mapManagedToUnoMethod,
								  m_managedBridge, pMonoParams, NULL,
								  mono_object_get_domain( m_managedBridge ) ) );

    return *ppResult;
}

} // namespace mono_uno

extern "C" {

// void SAL_CALL mono_environmentDisposing( uno_Environment * mono_env )
// 	SAL_THROW_EXTERN_C()
// {
// }

void SAL_CALL uno_initEnvironment( uno_Environment * mono_env )
	SAL_THROW_EXTERN_C()
{
	// mono_env->environmentDisposing = mono_environmentDisposing;
	mono_env->pExtEnv = 0; /* no extended support */
}

void SAL_CALL uno_ext_getMapping(
	uno_Mapping ** ppMapping, uno_Environment *pFrom, uno_Environment *pTo )
	SAL_THROW_EXTERN_C()
{
	OSL_ASSERT( 0 != ppMapping && 0 != pFrom && 0 != pTo );
    if (0 != *ppMapping)
    {
        (*(*ppMapping)->release)( *ppMapping );
        *ppMapping = 0;
    }

    /* FIXME check that mono's and sal's types have matching sizes
     * like jni_bridge does? */

    OUString const & from_env_typename = OUString::unacquired( &pFrom->pTypeName );
    OUString const & to_env_typename = OUString::unacquired( &pTo->pTypeName );

    uno_Mapping * mapping = 0;

    try
    {
        if (from_env_typename.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( UNO_LB_CLI )) &&
            to_env_typename.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( UNO_LB_UNO)))
        {
            Bridge * bridge =
                new Bridge( pFrom, pTo->pExtEnv, true ); // ref count = 1
            mapping = &bridge->m_mono2uno;
            uno_registerMapping(
                &mapping, Bridge_free, pFrom,
                (uno_Environment *)pTo->pExtEnv, 0);
        }
        else if (from_env_typename.equalsAsciiL(
                     RTL_CONSTASCII_STRINGPARAM( UNO_LB_UNO)) &&
                 to_env_typename.equalsAsciiL(
                     RTL_CONSTASCII_STRINGPARAM( UNO_LB_CLI)))
        {
            Bridge * bridge =
                new Bridge( pTo, pFrom->pExtEnv, false ); // ref count = 1
            mapping = &bridge->m_uno2mono;
            uno_registerMapping(
                &mapping, Bridge_free,
                (uno_Environment *)pFrom->pExtEnv, pTo, 0);
        }
    }
    catch (BridgeRuntimeError & err)
    {
#if OSL_DEBUG_LEVEL >= 1
        OString cstr_msg(
                OUStringToOString(
                    OUSTR("[mono_uno bridge error] ") + err.m_message,
                    RTL_TEXTENCODING_ASCII_US ) );
            OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }

    *ppMapping = mapping;
}

sal_Bool SAL_CALL component_canUnload( TimeValue * pTime )
    SAL_THROW_EXTERN_C()
{
    return (*g_moduleCount.canUnload)( &g_moduleCount, pTime );
}

} // extern "C"

MonoObject*
mono_uno::runtime_invoke (MonoMethod *method, void *obj, void **params,
						  MonoObject **exc, MonoDomain *domain)
{
	mono_thread_attach( domain );
	fprintf( stderr, "Thread attach, invoke '%s'\n",
			 mono_method_full_name (method, TRUE) );
	return mono_runtime_invoke( method, obj, params, exc );
}
