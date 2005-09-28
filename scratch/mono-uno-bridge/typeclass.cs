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

namespace uno.Typelib {

public class TypeClass
{
	/** type class of void */
	public const int VOID = 0;
	/** type class of char */
	public const int CHAR = 1;
	/** type class of boolean */
	public const int BOOLEAN = 2;
	/** type class of byte */
	public const int BYTE = 3;
	/** type class of short */
	public const int SHORT = 4;
	/** type class of unsigned short */
	public const int UNSIGNED_SHORT = 5;
	/** type class of long */
	public const int LONG = 6;
	/** type class of unsigned long */
	public const int UNSIGNED_LONG = 7;
	/** type class of hyper */
	public const int HYPER = 8;
	/** type class of unsigned hyper */
	public const int UNSIGNED_HYPER = 9;
	/** type class of float */
	public const int FLOAT = 10;
	/** type class of double */
	public const int DOUBLE = 11;
	/** type class of string */
	public const int STRING = 12;
	/** type class of type */
	public const int TYPE = 13;
	/** type class of any */
	public const int ANY = 14;
	/** type class of enum */
	public const int ENUM = 15;
	/** type class of typedef */
	public const int TYPEDEF = 16;
	/** type class of struct */
	public const int STRUCT = 17;
	/** type class of union (not implemented) */
	public const int UNION = 18;
	/** type class of exception */
	public const int EXCEPTION = 19;
	/** type class of sequence */
	public const int SEQUENCE = 20;
	/** type class of array (not implemented) */
	public const int ARRAY = 21;
	/** type class of interface */
	public const int INTERFACE = 22;
	/** type class of service (not implemented) */
	public const int SERVICE = 23;
	/** type class of module (not implemented) */
	public const int MODULE = 24;
	/** type class of interface method */
	public const int INTERFACE_METHOD = 25;
	/** type class of interface attribute */
	public const int INTERFACE_ATTRIBUTE = 26;
	/** type class of unknown type */
	public const int UNKNOWN = 27;
    /** type class of properties */
    public const int PROPERTY = 28;
    /** type class of constants */
    public const int CONSTANT = 29;
    /** type class of constants groups */
    public const int CONSTANTS = 30;
    /** type class of singletons */
    public const int SINGLETON = 31;
}

}
