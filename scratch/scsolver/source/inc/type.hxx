/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#ifndef _TYPE_HXX_
#define _TYPE_HXX_

#include <cppuhelper/implementationentry.hxx>
#include <numeric/lpmodel.hxx>

namespace scsolver {


enum ConstButtonType 
{
	CONST_ADD,
	CONST_CHANGE,
	CONST_DELETE 
};

/** This class is used to hold a constraint item for dialog component. */
class ConstraintString
{
public:
	rtl::OUString Left;
	rtl::OUString Right;
	scsolver::numeric::opres::Equality Equal;
};

}

#endif
