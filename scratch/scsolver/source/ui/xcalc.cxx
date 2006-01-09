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


#include <xcalc.hxx>
#include <global.hxx>
#include <unoglobal.hxx>

#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/document/XDocumentInfo.hpp>
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>

#include <com/sun/star/frame/XDesktop.hpp>

#include <com/sun/star/lang/XComponent.hpp>

#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XRangeSelection.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>

#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/XCellRange.hpp>

#include <com/sun/star/uno/XComponentContext.hpp>

#include <iostream>

using com::sun::star::uno::UNO_QUERY;
using namespace ::com::sun::star::sheet;
using namespace std;

namespace scsolver {
	

CalcInterface::CalcInterface( const Reference< uno::XComponentContext >& xCC ) :
	m_xCC( xCC ), m_xSM( NULL ), m_xCurComp( NULL )
{
}

CalcInterface::~CalcInterface()
{
}

//------------------------------------------------------------------------------
// Basic Component Methods
	
Reference< lang::XComponent > CalcInterface::getCurrentComponent() const
{
	if ( m_xCurComp == NULL )
		updateCurrentComponent();
	return m_xCurComp;
}

void CalcInterface::updateCurrentComponent() const
{
	Reference< uno::XInterface > aDesktop = getServiceManager()->createInstanceWithContext(
		ascii( "com.sun.star.frame.Desktop" ), m_xCC );

	Reference< frame::XDesktop > xDesktop( aDesktop, UNO_QUERY );
	m_xCurComp = xDesktop->getCurrentComponent();	
}

Reference< lang::XMultiComponentFactory > CalcInterface::getServiceManager() const
{
	if ( m_xSM == NULL )
		m_xSM = getComponentContext()->getServiceManager();
	return m_xSM;
}

void CalcInterface::setDocumentProperty( const rtl::OUString& sName, uno::Any aVal )
{
	Reference< document::XDocumentInfoSupplier > xDIS( getCurrentComponent(), UNO_QUERY );
	Reference< document::XDocumentInfo > xDI = xDIS->getDocumentInfo();

	Reference< beans::XPropertySet > xPS( xDI, UNO_QUERY );
	Reference< beans::XPropertySetInfo > xPInfo = xPS->getPropertySetInfo();
	if ( !xPInfo->hasPropertyByName( sName ) )
	{
		Reference< beans::XPropertyContainer > xPC( xDI, UNO_QUERY );
		xPC->addProperty( sName, beans::PropertyAttribute::OPTIONAL, aVal );
	}
	xPS->setPropertyValue( sName, aVal );
}

uno::Any CalcInterface::getDocumentProperty( const rtl::OUString& sName ) const
{
	Reference< document::XDocumentInfoSupplier > xDIS( getCurrentComponent(), UNO_QUERY );
	Reference< document::XDocumentInfo > xDI = xDIS->getDocumentInfo();
	Reference< beans::XPropertySet > xPS( xDI, UNO_QUERY );
	uno::Any aVal = xPS->getPropertyValue( sName );
	return aVal;
}

//------------------------------------------------------------------------------
// Calc Component Methods
	
Reference< XRangeSelection > CalcInterface::getXRangeSelection() const
{
	if ( m_xRngSel == NULL )
	{
		Reference< frame::XModel > xModel( getCurrentComponent(), UNO_QUERY );
		assert( xModel != NULL );
		
		Reference< frame::XController > xController = xModel->getCurrentController();
		assert( xController != NULL );
		
		Reference< XRangeSelection > xRngSel( xController, UNO_QUERY );
		assert( xRngSel != NULL );

		m_xRngSel = xRngSel;
	}
	
	return m_xRngSel;
}

//------------------------------------------------------------------------------
// Sheet-Related Methods

Reference< XSpreadsheet > CalcInterface::getActiveSheet() const
{
	Reference< lang::XComponent > xCurComp = getCurrentComponent();
	Reference< frame::XModel > xModel( xCurComp, UNO_QUERY );
	Reference< frame::XController > xCtrler = xModel->getCurrentController();
	Reference< sheet::XSpreadsheetView > xView( xCtrler, UNO_QUERY );
	return xView->getActiveSheet();
}
	
Reference< XSpreadsheet > CalcInterface::getSheetByIndex( const sal_uInt16 nIdx ) const
{
	Reference< XSpreadsheetDocument > xDoc( getCurrentComponent(), UNO_QUERY );
	Reference< XSpreadsheets > xSheets = xDoc->getSheets();
	Reference< container::XIndexAccess > xIndexAccess( xSheets, UNO_QUERY );
	uno::Any aSheet = xIndexAccess->getByIndex( nIdx );
	Reference< XSpreadsheet > xSheet;
	aSheet >>= xSheet;
	
	return xSheet;
}

Reference< XSpreadsheet > CalcInterface::getSheetByName( const rtl::OUString& sName ) const
{
	Reference< XSpreadsheetDocument > xDoc( getCurrentComponent(), UNO_QUERY );
	Reference< XSpreadsheets > xSheets = xDoc->getSheets();
	Reference< container::XNameAccess > xNameAccess( xSheets, UNO_QUERY );
	uno::Any aSheet = xNameAccess->getByName( sName );
	Reference< XSpreadsheet > xSheet;
	aSheet >>= xSheet;
	
	return xSheet;
}




rtl::OUString lcl_resolveSheetName( const rtl::OUString& sSheetAddr )
{
	rtl::OUString sSheetName;
	if ( sSheetAddr.indexOf( ascii( "$" ) ) == 0 )
	{
		if ( sSheetAddr.indexOf( ascii( "'" ) ) == 1 && 
			 sSheetAddr.lastIndexOf( ascii( "'" ) ) == sSheetAddr.getLength() - 1 )
			sSheetName = sSheetAddr.copy( 2, sSheetAddr.getLength() - 3 );
		else
			sSheetName = sSheetAddr.copy( 1, sSheetAddr.getLength() - 1 );
	}
	else
	{
		if ( sSheetAddr.indexOf( ascii( "'" ) ) == 0 && 
			 sSheetAddr.lastIndexOf( ascii( "'" ) ) == sSheetAddr.getLength() - 1 )
			sSheetName = sSheetAddr.copy( 1, sSheetAddr.getLength() - 2 );
		else
			sSheetName = sSheetAddr;
	}
	return sSheetName;
}

Reference< table::XCell > CalcInterface::getCell( const table::CellAddress& aAddr ) const
{
	return getSheetByIndex( aAddr.Sheet )->getCellByPosition( aAddr.Column, aAddr.Row );
}

table::CellAddress CalcInterface::getCellAddress( const rtl::OUString& sFullAddr )
{
	rtl::OUString sSheetAddr, sCellAddr;
	splitCellRangeAddress( sFullAddr, sSheetAddr, sCellAddr );
	return getCellAddress( sSheetAddr, sCellAddr );
}

table::CellAddress CalcInterface::getCellAddress(
	const rtl::OUString& sSheetAddr, const rtl::OUString& sCellAddr )
{
	table::CellRangeAddress aCRAddr = getCellRangeAddress( sSheetAddr, sCellAddr );
	table::CellAddress aAddr;
	aAddr.Sheet = aCRAddr.Sheet;
	aAddr.Column = aCRAddr.StartColumn;
	aAddr.Row = aCRAddr.StartRow;
	
	return aAddr;
}

table::CellRangeAddress CalcInterface::getCellRangeAddress( 
	const rtl::OUString& sFullAddr )
{
	rtl::OUString sSheetAddr, sCellAddr;
	splitCellRangeAddress( sFullAddr, sSheetAddr, sCellAddr );
	return getCellRangeAddress( sSheetAddr, sCellAddr );
}

table::CellRangeAddress CalcInterface::getCellRangeAddress( 
	const rtl::OUString& sSheetAddr, const rtl::OUString& sCellAddr )
{
	rtl::OUString sSheetName = lcl_resolveSheetName( sSheetAddr );
	
	Reference< sheet::XSpreadsheet > xSheet;
	table::CellRangeAddress aCRAddr;
	try
	{
		xSheet = getSheetByName( sSheetName );
	}
	catch ( const container::NoSuchElementException& e )
	{
		Debug( "There is no such sheet!" );
		return aCRAddr;
	}
	
	Reference< table::XCellRange > xCR( xSheet, UNO_QUERY );
	Reference< table::XCellRange > xCR2 = xCR->getCellRangeByName( sCellAddr );
	
	Reference< sheet::XCellRangeAddressable > xCRA( xCR2, UNO_QUERY );
	aCRAddr = xCRA->getRangeAddress();
	sal_Int16 nSheetId = aCRAddr.Sheet;
	sal_Int32 nSCol = aCRAddr.StartColumn;
	sal_Int32 nSRow = aCRAddr.StartRow;
	sal_Int32 nECol = aCRAddr.EndColumn;
	sal_Int32 nERow = aCRAddr.EndRow;
	
	cout << "Sheet: " << nSheetId << " (" << nSRow << ", " << nSCol << ") - (" 
		 << nERow << ", " << nECol << ")" << endl;
	
	return aCRAddr;
}

rtl::OUString CalcInterface::getCellFormula( const table::CellAddress& aAddr )
{
	return getCell( aAddr )->getFormula();
}

void CalcInterface::setCellFormula( 
		const table::CellAddress& aAddr, const rtl::OUString& sStr ) const
{
	getCell( aAddr )->setFormula( sStr );
}

void CalcInterface::setCellValue(
		const table::CellAddress& aAddr, double fVal ) const
{
	getCell( aAddr )->setValue( fVal );
}

double CalcInterface::getCellValue( const table::CellAddress& aAddr )
{
	return getCell( aAddr )->getValue();
}

rtl::OUString CalcInterface::getCellFormula( const rtl::OUString& sSheetAddr, 
		const rtl::OUString& sCellAddr )
{
	rtl::OUString sSheetName = lcl_resolveSheetName( sSheetAddr );
	
	Reference< sheet::XSpreadsheet > xSheet = getSheetByName( sSheetName );
	Reference< table::XCellRange > xCR( xSheet, UNO_QUERY );
	Reference< table::XCellRange > xCR2 = xCR->getCellRangeByName( sCellAddr );
	Reference< table::XCell > xCell = xCR2->getCellByPosition( 0, 0 );
	rtl::OUString sFormula = xCell->getFormula();
	
	return sFormula;
}




void CalcInterface::splitCellRangeAddress( const rtl::OUString& sFullAddr,
		rtl::OUString& sSheetAddr, rtl::OUString& sCellAddr )
{
	sal_Int32 nIdDot = sFullAddr.indexOf( ascii( "." ) );
	sSheetAddr = sFullAddr.copy( 0, nIdDot );
	sCellAddr = sFullAddr.copy( nIdDot + 1 );
}


}
