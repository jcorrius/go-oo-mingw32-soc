/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
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
#include "vbachart.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>


using namespace ::com::sun::star;
using namespace ::org::openoffice;

const rtl::OUString CHART_NAME( RTL_CONSTASCII_USTRINGPARAM("Name") );
::rtl::OUString SAL_CALL 
ScVbaChart::getName() throw (css::uno::RuntimeException)
{
	rtl::OUString sName;
	uno::Reference< beans::XPropertySet > xProps( m_xChartDoc, uno::UNO_QUERY_THROW );
	try
	{
		xProps->getPropertyValue( CHART_NAME ) >>= sName;
	}
	catch( uno::Exception e ) // swallow exceptions
	{
	}
	return sName;
}

uno::Any  SAL_CALL
ScVbaChart::SeriesCollection(const uno::Any&) throw (uno::RuntimeException)
{
	return uno::Any();
}

rtl::OUString& 
ScVbaChart::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaChart") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaChart::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.excel.Chart" ) );
	}
	return aServiceNames;
}

