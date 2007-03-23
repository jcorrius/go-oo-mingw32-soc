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
#include "vbaworksheets.hxx"

#include <sfx2/dispatch.hxx>
#include <sfx2/app.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/itemwrapper.hxx>
#include <svtools/itemset.hxx>
#include <svtools/eitem.hxx>

#include <comphelper/processfactory.hxx>
#include <cppuhelper/implbase3.hxx>

#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <org/openoffice/excel/XApplication.hpp>
#include <tools/string.hxx>

#include "vbaglobals.hxx"
#include "vbaworksheet.hxx"
#include "vbaworkbook.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;


class SheetsEnumeration : public EnumerationHelperImpl
{
	uno::Reference< frame::XModel > m_xModel;
	uno::WeakReference< vba::XHelperInterface > m_xParent;
public:
	SheetsEnumeration( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< container::XEnumeration >& xEnumeration,  const uno::Reference< frame::XModel >& xModel  ) throw ( uno::RuntimeException ) : EnumerationHelperImpl( xContext, xEnumeration ), m_xModel( xModel ), m_xParent( xParent ) {}

	virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		uno::Reference< sheet::XSpreadsheet > xSheet( m_xEnumeration->nextElement(), uno::UNO_QUERY_THROW );
		return uno::makeAny( uno::Reference< excel::XWorksheet > ( new ScVbaWorksheet( m_xParent, m_xContext, xSheet, m_xModel ) ) );
	}

};

ScVbaWorksheets::ScVbaWorksheets( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, const uno::Reference< sheet::XSpreadsheets >& xSheets, const uno::Reference< frame::XModel >& xModel ): ScVbaWorksheets_BASE( xParent, xContext, uno::Reference< container::XIndexAccess >( xSheets, uno::UNO_QUERY ) ), mxModel( xModel ), m_xSheets( xSheets ) 
{
}

ScVbaWorksheets::ScVbaWorksheets( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, const uno::Reference< container::XEnumerationAccess >& xEnumAccess, const uno::Reference< frame::XModel >& xModel  ):  ScVbaWorksheets_BASE( xParent, xContext, uno::Reference< container::XIndexAccess >( xEnumAccess, uno::UNO_QUERY ) ), mxModel(xModel) 
{
}

// XEnumerationAccess
uno::Type 
ScVbaWorksheets::getElementType() throw (uno::RuntimeException)
{
	return excel::XWorksheet::static_type(0);
}

uno::Reference< container::XEnumeration >
ScVbaWorksheets::createEnumeration() throw (uno::RuntimeException)
{
	if ( !m_xSheets.is() )
	{
		uno::Reference< container::XEnumerationAccess > xAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
		return xAccess->createEnumeration(); 
	}
	uno::Reference< container::XEnumerationAccess > xEnumAccess( m_xSheets, uno::UNO_QUERY_THROW );
	return new SheetsEnumeration( this, mxContext, xEnumAccess->createEnumeration(), mxModel );
}

uno::Any
ScVbaWorksheets::createCollectionObject( const uno::Any& aSource )
{
	uno::Reference< sheet::XSpreadsheet > xSheet( aSource, uno::UNO_QUERY );
	return uno::makeAny( uno::Reference< excel::XWorksheet > ( new ScVbaWorksheet( uno::Reference< vba::XHelperInterface >( getParent(), uno::UNO_QUERY_THROW ), mxContext, xSheet, mxModel ) ) );
}

// XWorksheets
uno::Any
ScVbaWorksheets::Add( const uno::Any& Before, const uno::Any& After,
					 const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException)
{
	if ( isSelectedSheets() )
		return uno::Any(); // or should we throw?
	
	rtl::OUString aStringSheet;
	sal_Bool bBefore(sal_True);
	SCTAB nSheetIndex = 0;
	SCTAB nNewSheets = 1, nType = 0;
	Count >>= nNewSheets;
	Type >>= nType;
	SCTAB nCount = 0;
	Before >>= aStringSheet;
	if (!aStringSheet.getLength())
	{
		After >>= aStringSheet;
		bBefore = sal_False;
	}
	if (!aStringSheet.getLength())
	{
		aStringSheet = ScVbaGlobals::getGlobalsImpl( 
			mxContext )->getApplication()->getActiveWorkbook()->getActiveSheet()->getName();
		bBefore = sal_True;
	}
		nCount = static_cast< SCTAB >( m_xIndexAccess->getCount() );
		for (SCTAB i=0; i < nCount; i++)
		{
			uno::Reference< sheet::XSpreadsheet > xSheet(m_xIndexAccess->getByIndex(i), uno::UNO_QUERY);
			uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
			if (xNamed->getName() == aStringSheet)
			{
				nSheetIndex = i;
				break;
			}
		}

	if(!bBefore)
		nSheetIndex++;

	SCTAB nSheetName = nCount + 1L;
	String aStringBase( RTL_CONSTASCII_USTRINGPARAM("Sheet") );				
	uno::Any result;
	for (SCTAB i=0; i < nNewSheets; i++, nSheetName++)
	{
		String aStringName = aStringBase;
		aStringName += String::CreateFromInt32(nSheetName);
		while (m_xNameAccess->hasByName(aStringName))
		{
			nSheetName++;
			aStringName = aStringBase;
			aStringName += String::CreateFromInt32(nSheetName);
		}
		m_xSheets->insertNewByName(aStringName, nSheetIndex + i);
		result = getItemByStringIndex( aStringName );
	}
	
	return  result;
}

void
ScVbaWorksheets::Delete() throw (uno::RuntimeException)
{
	//SC_VBA_STUB();
}

bool
ScVbaWorksheets::isSelectedSheets()
{
	return !m_xSheets.is();
}

void SAL_CALL 
ScVbaWorksheets::PrintOut( const uno::Any& From, const uno::Any& To, const uno::Any& Copies, const uno::Any& Preview, const uno::Any& ActivePrinter, const uno::Any& PrintToFile, const uno::Any& Collate, const uno::Any& PrToFileName ) throw (uno::RuntimeException)
{
	sal_Int32 nTo = 0;
	sal_Int32 nFrom = 0;
	sal_Int16 nCopies = 1;
	sal_Bool bCollate = sal_False;
	sal_Bool bSelection = sal_False;
	From >>= nFrom;
	To >>= nTo;
	Copies >>= nCopies;
	if ( nCopies > 1 ) // Collate only useful when more that 1 copy
		Collate >>= bCollate;

	if ( !( nFrom || nTo ) )
		if ( isSelectedSheets() )
			bSelection = sal_True;

	PrintOutHelper( From, To, Copies, Preview, ActivePrinter, PrintToFile, Collate, PrToFileName, mxModel, bSelection );
}

uno::Any SAL_CALL 
ScVbaWorksheets::getVisible() throw (uno::RuntimeException)
{
	sal_Bool bVisible = sal_True;
	uno::Reference< container::XEnumeration > xEnum( createEnumeration(), uno::UNO_QUERY_THROW );
	while ( xEnum->hasMoreElements() )
	{
		uno::Reference< excel::XWorksheet > xSheet( xEnum->nextElement(), uno::UNO_QUERY_THROW );
		if ( xSheet->getVisible() == sal_False )
		{
				bVisible = sal_False;
				break;
		}
	}
	return uno::makeAny( bVisible );
}

void SAL_CALL 
ScVbaWorksheets::setVisible( const uno::Any& _visible ) throw (uno::RuntimeException)
{
	sal_Bool bState;
	if ( _visible >>= bState )
	{
		uno::Reference< container::XEnumeration > xEnum( createEnumeration(), uno::UNO_QUERY_THROW );
		while ( xEnum->hasMoreElements() )
		{
			uno::Reference< excel::XWorksheet > xSheet( xEnum->nextElement(), uno::UNO_QUERY_THROW );
			xSheet->setVisible( bState );
		}
	}
	else
		throw uno::RuntimeException( rtl::OUString( 
			RTL_CONSTASCII_USTRINGPARAM( "Visible property doesn't support non boolean #FIXME" ) ), uno::Reference< uno::XInterface >() );
}
//ScVbaCollectionBaseImpl
uno::Any 
ScVbaWorksheets::getItemByStringIndex( const rtl::OUString& sIndex ) throw (uno::RuntimeException)
{
	String sScIndex = sIndex;
	ScDocument::ConvertToValidTabName( sScIndex, '_' );
	return ScVbaWorksheets_BASE::getItemByStringIndex( sScIndex );
}

rtl::OUString& 
ScVbaWorksheets::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaWorksheets") );
	return sImplName;
}

css::uno::Sequence<rtl::OUString> 
ScVbaWorksheets::getServiceNames()
{
	static uno::Sequence< rtl::OUString > sNames;
	if ( sNames.getLength() == 0 )
	{
		sNames.realloc( 1 );
		sNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.excel.Worksheets") );
	}
	return sNames;
}
