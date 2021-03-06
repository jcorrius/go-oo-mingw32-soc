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

#include "rtl/ustring.h"
#include "uno/dispatcher.h"
#include "uno/environment.h"
#include <stdio.h>

extern "C" {

/* uno_Interface */

void SAL_CALL cli_uno_interface_acquire( uno_Interface *pInterface )
    SAL_THROW_EXTERN_C()
{
    (*pInterface->acquire)( pInterface );
}

void SAL_CALL cli_uno_interface_release( uno_Interface *pInterface )
    SAL_THROW_EXTERN_C()
{
    (*pInterface->release)( pInterface );
}

void SAL_CALL cli_uno_interface_dispatch(
    uno_Interface *pInterface, const struct _typelib_TypeDescription *pMemberType,
    void *pReturn, void *pArgs[], uno_Any **ppException )
    SAL_THROW_EXTERN_C()
{
    fprintf( stderr, "dispatch %p\n", pInterface );
    (*pInterface->pDispatcher)( pInterface, pMemberType, pReturn, pArgs, ppException );
}

/* uno_ExtEnvironment */

void SAL_CALL cli_uno_environment_getObjectIdentifier(
    uno_ExtEnvironment *pUnoEnv, rtl_uString **ppOId, uno_Interface *pUnoI )
    SAL_THROW_EXTERN_C()
{
    (*pUnoEnv->getObjectIdentifier)( pUnoEnv, ppOId, pUnoI );
}

void SAL_CALL cli_uno_environment_registerInterface(
    uno_ExtEnvironment *pUnoEnv, void **ppInterface, rtl_uString *pOId,
    struct _typelib_InterfaceTypeDescription *pTypeDescr )
    SAL_THROW_EXTERN_C()
{
    (*pUnoEnv->registerInterface)( pUnoEnv, ppInterface, pOId, pTypeDescr );
}

void SAL_CALL cli_uno_environment_getRegisteredInterface(
    uno_ExtEnvironment *pUnoEnv, void **ppInterface, rtl_uString *pOId,
    struct _typelib_InterfaceTypeDescription *pTypeDescr )
    SAL_THROW_EXTERN_C()
{
    (*pUnoEnv->getRegisteredInterface)( pUnoEnv, ppInterface, pOId, pTypeDescr );
}


}
