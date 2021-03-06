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

#if ! defined INCLUDED_MONO_BRIDGE_H
#define INCLUDED_MONO_BRIDGE_H

#include "glib/gtypes.h"
#include "osl/interlck.h"
#include "rtl/ustring.hxx"
#include "typelib/typedescription.hxx"
#include "uno/dispatcher.h"
#include "uno/mapping.h"

extern "C" {
#include "mono/metadata/appdomain.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/object.h"
#include "mono/metadata/threads.h"
}

namespace cssu = com::sun::star::uno;

typedef struct _uno_ExtEnvironment uno_ExtEnvironment;
typedef struct _uno_Environment uno_Environment;
typedef struct _typelib_TypeDescription typelib_TypeDescription;

namespace mono_uno
{

MonoObject* runtime_invoke (MonoMethod *method, void *obj, void **params,
							MonoObject **exc, MonoDomain *domain);

//==== holds environments and mappings =========================================
struct Bridge;
struct Mapping : public uno_Mapping
{
    Bridge * m_bridge;
};

//==============================================================================
struct Bridge
{
    mutable oslInterlockedCount m_ref;
    MonoObject *                m_managedBridge;
    
    uno_ExtEnvironment *        m_uno_env;
    uno_Environment *           m_mono_env;
    
    Mapping                     m_mono2uno;
    Mapping                     m_uno2mono;
    bool                        m_registered_mono2uno;
    
    MonoMethod *                m_mapUnoToManagedMethod;
    MonoMethod *                m_mapManagedToUnoMethod;

    ~Bridge() SAL_THROW( () );
    explicit Bridge(
        uno_Environment * mono_env, uno_ExtEnvironment * uno_env,
        bool registered_mono2uno );
    
    void acquire() const;
    void release() const;

    void * map_to_mono(
        uno_Interface * pUnoI, typelib_TypeDescription * pTD ) const;

    uno_Interface * map_to_uno(
        void * pMonoI, typelib_TypeDescription * pTD ) const;
};

struct MonoProxy : public uno_Interface
{
    mutable oslInterlockedCount m_ref;
    guint32                     m_managedProxy;
    uno_ExtEnvironment *        m_unoEnv;
    const cssu::TypeDescription m_unoType;
    const rtl::OUString         m_Oid;
    MonoMethod *                m_managedDispatch;

    void acquire() const;
    void release() const;
    void dispatch(typelib_TypeDescription const * member_td, void * uno_ret,
                  void * uno_args [], uno_Any ** uno_exc);

    MonoProxy(uno_ExtEnvironment * pUnoEnv, guint32 managedProxy,
              rtl_uString * pOid, typelib_TypeDescription * pTD);
};

struct BridgeRuntimeError
{
    ::rtl::OUString m_message;
    
    inline BridgeRuntimeError( ::rtl::OUString const & message )
        : m_message( message )
        {}
};

}

#endif
