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

namespace uno.Binary {

using System;
using System.Runtime.InteropServices;
using uno.Typelib;
using uno.rtl;

[ StructLayout(LayoutKind.Sequential) ]
public unsafe struct Any
{
    /** type of value
     */
    public uno.Typelib.TypeDescriptionReference *pType;

    /** pointer to value; this may point to pReserved and thus the uno_Any is not anytime
        mem-copyable! You may have to correct the pData pointer to pReserved. Otherwise you need
        not, because the data is stored in heap space.
    */
    public void *pData;
    /** reserved space for storing value
    */
    public void *pReserved;

    [ DllImport("uno_cppu", EntryPoint="uno_any_construct") ]
    public static unsafe extern void Construct(/* Any */ void *dest,
					       void *source,
					       /* uno.Typelib.TypeDescriptionReference */ void *type,
					       void *acquireFunction);

    [ DllImport("uno_cppu", EntryPoint="uno_any_destruct") ]
    public static unsafe extern void Destroy(/* Any */ void *value, void *releaseFunction);
}

// FIXME wrap this nicely
public struct Interface 
{
    [ DllImport("cli_uno", EntryPoint="cli_uno_interface_acquire") ]
    public static extern void Acquire(IntPtr unoInterface);

    [ DllImport("cli_uno", EntryPoint="cli_uno_interface_release") ]
    public static extern void Release(IntPtr unoInterface);

    [ DllImport("cli_uno", EntryPoint="cli_uno_interface_dispatch") ]
    public static unsafe extern void Dispatch(IntPtr unoInterface,
                                              /* uno.Typelib.TypeDescription */ void *memberTD,
                                              void *result,
                                              void **args,
                                              uno.Binary.Any **exception);
}

// FIXME and this
public class Environment
{
    [ DllImport("cli_uno", EntryPoint="cli_uno_environment_getObjectIdentifier") ]
    public static unsafe extern void GetObjectIdentifier(IntPtr unoEnvironment,
							 UString** oid,
							 IntPtr unoInterface);

    [ DllImport("cli_uno", EntryPoint="cli_uno_environment_registerInterface") ]
    public static unsafe extern void RegisterInterface(
        IntPtr unoEnvironment,
        ref IntPtr ppInterface,
        /* UString */ void* oid,
        /* InterfaceTypeDescription */ void *td);

    [ DllImport("cli_uno", EntryPoint="cli_uno_environment_getRegisteredInterface") ]
    public static unsafe extern void GetRegisteredInterface(
        IntPtr unoEnvironment,
        ref IntPtr ppInterface,
        /* UString */ void* oid,
        /* InterfaceTypeDescription */ void *td);
}

public struct Data
{
    [ DllImport("uno_cppu", EntryPoint="uno_type_destructData") ]
    public static unsafe extern void Destroy(void *data,
                                             /* uno.Typelib.TypeDescription */ void *td,
                                             // FIXME is this okay? release is a function pointer
                                             void *release);
}

public unsafe struct SequencePtr
{
    int *sal_Sequence;

    // 	sal_Int32			nRefCount;
    // 	sal_Int32			nElements;
    // 	char				elements[1];

	/** element count<br>
	*/
	/** elements array<br>
	*/

	/** reference count of sequence<br>
	*/
    private unsafe int nRefCount
    {
        get { return *(sal_Sequence); }
        set { *(sal_Sequence) = value; }
    }

    public unsafe int nElements
    {
        // FIXME +1 is not 64 bit safe
        get { return *(sal_Sequence + 1); }
        set { *(sal_Sequence + 1) = value; }
    }

    public unsafe IntPtr elementsPtr
    {
        // FIXME +2 is not 64 bit safe
        get { return new IntPtr((void *)(sal_Sequence + 2)); }        
    }

    private unsafe SequencePtr(void *mem)
    {
        sal_Sequence = mem;
    }

    private const int HEADER_SIZE = 8; // FIXME alignment

    public static SequencePtr Allocate(int length, int elementSize)
    {
        void *rtlPtr = uno.rtl.Mem.Allocate(HEADER_SIZE + (length * elementSize));
        SequencePtr seqPtr = new SequencePtr(rtlPtr);
        seqPtr.nRefCount = 1;
        seqPtr.nElements = length;
        return seqPtr;
    }
}
}
