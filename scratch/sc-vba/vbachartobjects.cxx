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
#include <com/sun/star/document/XEmbeddedObjectSupplier.hpp>


#include "vbachartobjects.hxx"
#include "vbachartobject.hxx"
#include "vbaglobals.hxx"

using namespace ::com::sun::star;
using namespace ::org::openoffice;


class ChartObjectEnumerationImpl : public EnumerationHelperImpl
{
public:

	ChartObjectEnumerationImpl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< container::XEnumeration >& xEnumeration ) throw ( uno::RuntimeException ) : EnumerationHelperImpl( xContext, xEnumeration ) {}
	virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		uno::Reference< table::XTableChart > xTableChart( m_xEnumeration->nextElement(), uno::UNO_QUERY_THROW );
		// #FIXME what is the correct parent object
		return uno::makeAny( uno::Reference< excel::XChartObject > ( new ScVbaChartObject(  uno::Reference< vba::XHelperInterface >(), m_xContext, xTableChart ) ) );
	}
};



// XEnumerationAccess

uno::Reference< container::XEnumeration >
ScVbaChartObjects::createEnumeration() throw (uno::RuntimeException)
{
	css::uno::Reference< container::XEnumerationAccess > xEnumAccess( m_xTableCharts, uno::UNO_QUERY_THROW );
	return new ChartObjectEnumerationImpl( m_xContext, xEnumAccess->createEnumeration() );
}

// XElementAccess

uno::Type 
ScVbaChartObjects::getElementType() throw (uno::RuntimeException)
{
	return excel::XChartObject::static_type(0);
}

// ScVbaCollectionBaseImpl
uno::Any
ScVbaChartObjects::createCollectionObject( const css::uno::Any& aSource )
{	
	uno::Reference< table::XTableChart > xTableChart( aSource, uno::UNO_QUERY_THROW );
	//#FIXME needs correct parent object ( this? )
	return uno::makeAny( uno::Reference< excel::XChartObject > ( new ScVbaChartObject( uno::Reference< vba::XHelperInterface >(), m_xContext, xTableChart ) ) );
}

rtl::OUString& 
ScVbaChartObjects::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaChartObjects") );
	return sImplName;
}

css::uno::Sequence<rtl::OUString> 
ScVbaChartObjects::getServiceNames()
{
	static uno::Sequence< rtl::OUString > sNames;
	if ( sNames.getLength() == 0 )
	{
		sNames.realloc( 1 );
		sNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.excel.ChartObjects") );
	}
	return sNames;
}



