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
 
 
#include "unohelper.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <cassert>

using ::com::sun::star::uno::UNO_QUERY;

namespace scsolver { namespace unohelper {

 
uno::Any getPropertyValue( const Reference< uno::XInterface >& obj, const rtl::OUString& sPropName )
{
	Reference< beans::XPropertySet > xPS( obj, UNO_QUERY );
	assert( xPS != NULL );
	return xPS->getPropertyValue( sPropName );
}

void getPropertyValue( const Reference< XInterface >& obj, const rtl::OUString& sPropName, sal_Int16& t )
{
	uno::Any aValue = getPropertyValue( obj, sPropName );
	aValue >>= t;
} 

void setPropertyValue( const Reference< uno::XInterface >& obj, const rtl::OUString& sPropName,
		const sal_Int32 n )
{
	uno::Any aVal;
	aVal <<= n;
	setPropertyValue( obj, sPropName, aVal );
}

void setPropertyValue( const Reference< uno::XInterface >& obj, const rtl::OUString& sPropName,
		const uno::Any& aVal )
{
	Reference< beans::XPropertySet > xPS( obj, UNO_QUERY );
	assert( xPS != NULL );
	xPS->setPropertyValue( sPropName, aVal );
}


}}

















