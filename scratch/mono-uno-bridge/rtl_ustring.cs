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

namespace uno.rtl {

using System;
using System.Runtime.InteropServices;
using System.Text;

public unsafe struct UString
{
    public int RefCount;
    public int Length;
    public char FirstChar;

    [DllImport("sal")]
    private static extern void rtl_uString_acquire(void* data);

    [DllImport("sal")]
    private static unsafe extern void rtl_uString_release(void* data);

    [DllImport("sal")]
    private static unsafe extern void rtl_uString_new(void* data);

    [DllImport("sal")]
    private static unsafe extern void rtl_uString_newFromStr_WithLength(
        void* data,
        // this should pass a pointer to the original string's char[]
        [MarshalAs(UnmanagedType.LPWStr)] string value,
        int len);

    [DllImport("sal")]
    private static unsafe extern void rtl_uString_newFromStr_WithLength(
        void* data,
        // this should pass a pointer to the stringbuilder's internal char[]
        [MarshalAs(UnmanagedType.LPWStr)] StringBuilder buffer,
        int len);

    public static unsafe void Acquire(UString* us)
    {
        rtl_uString_acquire(us);
    }

    public static unsafe void Release(UString* us)
    {
        rtl_uString_release(us);
    }

    public static unsafe void New(UString** p)
    {
        rtl_uString_new(p);
    }

    public static unsafe void NewFromString(UString **p, string s)
    {
        rtl_uString_newFromStr_WithLength(p, s, s.Length);
    }

    public static unsafe void NewFromStringBuilder(UString **p, StringBuilder sb)
    {
        rtl_uString_newFromStr_WithLength(p, sb, sb.Length);
    }

    public static unsafe string UStringToString(UString *p)
    {
        return new String(&(p->FirstChar), 0, p->Length);
    }
}

// FIXME move this to its own file or rename this file to e.g. sal
public unsafe struct Mem
{
    // FIXME parameter is a sal_Size which is unsigned and has the
    // size of a native long.  Thus this is not 64bit safe.  Might have
    // to write a glue function that always takes 32bit.
    [DllImport("sal", EntryPoint="rtl_allocateMemory")]    
	public static unsafe extern void *Allocate(int bytes);
}

}
