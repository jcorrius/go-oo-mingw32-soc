/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: vbawindow.cxx,v $
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
#include "vbawindow.hxx"
#include "vbaworksheets.hxx"
#include "vbaworksheet.hxx"
#include "vbapane.hxx"
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/view/DocumentZoomType.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <org/openoffice/excel/XlWindowState.hpp>
#include <org/openoffice/excel/XlWindowView.hpp>
#include <org/openoffice/excel/Constants.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/awt/PosSize.hpp>

#include <docsh.hxx>
#include <tabvwsh.hxx>
#include <docuno.hxx>
#include <sc.hrc>
#include <hash_map>
#include <sfx2/viewfrm.hxx>
#include <sfx2/topfrm.hxx>

using namespace ::com::sun::star;
using namespace ::org::openoffice;
using namespace ::org::openoffice::excel::XlWindowState;
#define SHOWGRID "ShowGrid"
#define HASVERTSCROLLBAR "HasVerticalScrollBar"
#define HASHORIZSCROLLBAR "HasHorizontalScrollBar"

typedef  std::hash_map< rtl::OUString,
SCTAB, ::rtl::OUStringHash,
::std::equal_to< ::rtl::OUString > > NameIndexHash;

typedef std::vector < uno::Reference< sheet::XSpreadsheet > > Sheets;

typedef ::cppu::WeakImplHelper1< container::XEnumeration

> Enumeration_BASE;

typedef ::cppu::WeakImplHelper3< container::XEnumerationAccess 
	, com::sun::star::container::XIndexAccess
	, com::sun::star::container::XNameAccess
	> SelectedSheets_BASE;


class SelectedSheetsEnum : public Enumeration_BASE
{
public:
	uno::Reference< uno::XComponentContext > m_xContext;
	Sheets m_sheets;
	uno::Reference< frame::XModel > m_xModel;
	Sheets::const_iterator m_it;

	SelectedSheetsEnum( const uno::Reference< uno::XComponentContext >& xContext, const Sheets& sheets, const uno::Reference< frame::XModel >& xModel ) throw ( uno::RuntimeException ) :  m_xContext( xContext ), m_sheets( sheets ), m_xModel( xModel )
	{
		m_it = m_sheets.begin();
	}
	// XEnumeration
	virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException) 
	{ 
		return m_it != m_sheets.end();
	}
	virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		if ( !hasMoreElements() )
		{
			throw container::NoSuchElementException();
		}
		// #FIXME needs ThisWorkbook as parent
		return uno::makeAny( uno::Reference< excel::XWorksheet > ( new ScVbaWorksheet( uno::Reference< vba::XHelperInterface >(), m_xContext, *(m_it++), m_xModel ) ) );
	}


};

class SelectedSheetsEnumAccess : public SelectedSheets_BASE
{
	uno::Reference< uno::XComponentContext > m_xContext;
	NameIndexHash namesToIndices;
	Sheets sheets;
	uno::Reference< frame::XModel > m_xModel; 
public:
	SelectedSheetsEnumAccess( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< frame::XModel >& xModel ):m_xContext( xContext ), m_xModel( xModel )
	{
		ScModelObj* pModel = static_cast< ScModelObj* >( m_xModel.get() );
		if ( !pModel )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot obtain current document" ) ), uno::Reference< uno::XInterface >() );
		ScDocShell* pDocShell = (ScDocShell*)pModel->GetEmbeddedObject();
		if ( !pDocShell )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot obtain docshell" ) ), uno::Reference< uno::XInterface >() );
		ScTabViewShell* pViewShell = getBestViewShell( m_xModel );
		if ( !pViewShell )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot obtain view shell" ) ), uno::Reference< uno::XInterface >() );
			
		SCTAB nTabCount = pDocShell->GetDocument()->GetTableCount();
		uno::Sequence<sal_Int32> aSheets( nTabCount );
		SCTAB nIndex = 0;
		const ScMarkData& rMarkData = pViewShell->GetViewData()->GetMarkData();
		sheets.reserve( nTabCount );
		uno::Reference <sheet::XSpreadsheetDocument> xSpreadSheet( m_xModel, uno::UNO_QUERY_THROW );
		uno::Reference <container::XIndexAccess> xIndex( xSpreadSheet->getSheets(), uno::UNO_QUERY_THROW );
		for ( SCTAB nTab=0; nTab<nTabCount; nTab++ )
		{
			if ( rMarkData.GetTableSelect(nTab) )
			{
				uno::Reference< sheet::XSpreadsheet > xSheet( xIndex->getByIndex( nTab ), uno::UNO_QUERY_THROW );
				uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
				sheets.push_back( xSheet );
				namesToIndices[ xNamed->getName() ] = nIndex++;
			}
		}

	}
	
	//XEnumerationAccess
	virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException)
	{
		return new SelectedSheetsEnum( m_xContext, sheets, m_xModel  ); 
	}
	// XIndexAccess
	virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException) 
	{ 
		return sheets.size();
	}
	virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw ( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		if ( Index < 0
		|| static_cast< Sheets::size_type >( Index ) >= sheets.size() ) 
			throw lang::IndexOutOfBoundsException();
		
		return uno::makeAny( sheets[ Index ] );
	}

	//XElementAccess
	virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException)
	{ 
		return excel::XWorksheet::static_type(0); 
	}

	virtual ::sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException) 
	{ 
		return (sheets.size() > 0);
	}

	//XNameAccess
	virtual uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		NameIndexHash::const_iterator it = namesToIndices.find( aName );
		if ( it == namesToIndices.end() )
			throw container::NoSuchElementException();
		return uno::makeAny( sheets[ it->second ] );
		
	}

	virtual uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (uno::RuntimeException) 
	{ 
		uno::Sequence< ::rtl::OUString > names( namesToIndices.size() );
		::rtl::OUString* pString = names.getArray();
		NameIndexHash::const_iterator it = namesToIndices.begin();
		NameIndexHash::const_iterator it_end = namesToIndices.end();
		for ( ; it != it_end; ++it, ++pString )
			*pString = it->first;	
		return names;	
	}

	virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (uno::RuntimeException) 
	{ 
		NameIndexHash::const_iterator it = namesToIndices.find( aName );
		return (it != namesToIndices.end());
	}


};

ScVbaWindow::ScVbaWindow( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< frame::XModel >& xModel ) : WindowImpl_BASE( xParent, xContext ), m_xModel( xModel ) 
{
	uno::Reference< frame::XController > xController( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	m_xViewPane.set( xController, uno::UNO_QUERY_THROW );
	m_xViewFreezable.set( xController, uno::UNO_QUERY_THROW );
	m_xViewSplitable.set( xController, uno::UNO_QUERY_THROW );
	m_xPane.set( ActivePane(), uno::UNO_QUERY_THROW );
	m_xDevice.set( xController->getFrame()->getComponentWindow(), uno::UNO_QUERY_THROW );
}


void  
ScVbaWindow::Scroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft, bool bLargeScroll ) throw (uno::RuntimeException)
{
	if( bLargeScroll )
		m_xPane->LargeScroll( Down, Up, ToRight, ToLeft );
	else
		m_xPane->SmallScroll( Down, Up, ToRight, ToLeft );
}
void SAL_CALL 
ScVbaWindow::SmallScroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft ) throw (uno::RuntimeException)
{
	Scroll( Down, Up, ToRight, ToLeft );
}
void SAL_CALL 
ScVbaWindow::LargeScroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft ) throw (uno::RuntimeException)
{
	Scroll( Down, Up, ToRight, ToLeft, true );
}

uno::Any SAL_CALL 
ScVbaWindow::SelectedSheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	uno::Reference< container::XEnumerationAccess > xEnumAccess( new SelectedSheetsEnumAccess( mxContext, m_xModel  ) );
	// #FIXME needs a workbook as a parent
	uno::Reference< excel::XWorksheets > xSheets(  new ScVbaWorksheets( uno::Reference< vba::XHelperInterface >(), mxContext, xEnumAccess, m_xModel ) );
	if ( aIndex.hasValue() )
	{
		uno::Reference< vba::XCollection > xColl( xSheets, uno::UNO_QUERY_THROW );
		return xColl->Item( aIndex, uno::Any() );	
	}
	return uno::makeAny( xSheets ); 	
}

void SAL_CALL 
ScVbaWindow::ScrollWorkbookTabs( const uno::Any& /*Sheets*/, const uno::Any& /*Position*/ ) throw (uno::RuntimeException)
{
// #TODO #FIXME need some implementation to scroll through the tabs
// but where is this done?
/*
	sal_Int32 nSheets = 0;
	sal_Int32 nPosition = 0;
	throw uno::RuntimeException( rtl::OUString::createFromAscii("No Implemented" ), uno::Reference< uno::XInterface >() ); 
	sal_Bool bSheets = ( Sheets >>= nSheets );
	sal_Bool bPosition = ( Position >>= nPosition );
	if ( bSheets || bPosition ) // at least one param specified
		if ( bSheets )
			;// use sheets
		else if ( bPosition )
			; //use position
*/

}
uno::Reference< beans::XPropertySet >
getPropsFromModel( const uno::Reference< frame::XModel >& xModel )
{
	uno::Reference< frame::XController > xController = xModel->getCurrentController();
	if ( !xController.is() )
		throw uno::RuntimeException( rtl::OUString(
			RTL_CONSTASCII_USTRINGPARAM ("No controller for model") ), uno::Reference< uno::XInterface >() );	
	return uno::Reference< beans::XPropertySet >(  xController->getFrame(), uno::UNO_QUERY );
}


uno::Any SAL_CALL 
ScVbaWindow::getCaption() throw (uno::RuntimeException)
{
	static rtl::OUString sCrud(RTL_CONSTASCII_USTRINGPARAM(" - OpenOffice.org Calc" ) );
	static sal_Int32 nCrudLen = sCrud.getLength();

	uno::Reference< beans::XPropertySet > xProps = getPropsFromModel( m_xModel );
	rtl::OUString sTitle;
	xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ("Title") ) ) >>= sTitle;	
	sal_Int32 nCrudIndex = sTitle.indexOf( sCrud );	
	// adjust title ( by removing crud )
	// sCrud string present
	if ( nCrudIndex != -1 )
	{
		// and ends with sCrud
		if ( ( nCrudLen + nCrudIndex ) == sTitle.getLength() )
		{
			sTitle = sTitle.copy( 0, nCrudIndex );
			ScVbaWorkbook workbook( uno::Reference< vba::XHelperInterface >( ScVbaGlobals::getGlobalsImpl( mxContext )->getApplication(), uno::UNO_QUERY_THROW ), mxContext, m_xModel );
			rtl::OUString sName = workbook.getName();
			// rather bizare hack to make sure the name behavior
			// is like XL
			// if the adjusted title == workbook name, use name
			// if the adjusted title != workbook name but ...
			// 	name == title + extension ( .csv, ,odt, .xls )
			//	etc. then also use the name

			if ( !sTitle.equals( sName ) )
			{
				static rtl::OUString sDot( RTL_CONSTASCII_USTRINGPARAM(".") );
				// starts with title
				if ( sName.indexOf( sTitle ) == 0 )
					// extention starts immediately after
					if ( sName.match( sDot, sTitle.getLength() ) )
						sTitle = sName;
			}
		}
	}			
	return uno::makeAny( sTitle );
}

void SAL_CALL 
ScVbaWindow::setCaption( const uno::Any& _caption ) throw (uno::RuntimeException)
{
	
	uno::Reference< beans::XPropertySet > xProps = getPropsFromModel( m_xModel );
	xProps->setPropertyValue( rtl::OUString(
		RTL_CONSTASCII_USTRINGPARAM ("Title") ) , _caption );	
}

uno::Any SAL_CALL 
ScVbaWindow::getScrollRow() throw (uno::RuntimeException)
{	
    sal_Int32 nValue = 0;
    ScTabViewShell* pViewShell = getBestViewShell( m_xModel );	
	if ( pViewShell )
	{
	    ScSplitPos eWhich = pViewShell->GetViewData()->GetActivePart();
	    nValue = pViewShell->GetViewData()->GetPosY(WhichV(eWhich));
	}
    
    return uno::makeAny( nValue + 1);
}

void SAL_CALL 
ScVbaWindow::setScrollRow( const uno::Any& _scrollrow ) throw (uno::RuntimeException)
{
	ScTabViewShell* pViewShell = getBestViewShell( m_xModel );	
	if ( pViewShell )
	{
		sal_Int32 scrollRow = 0;	
	    _scrollrow >>= scrollRow;
	    ScSplitPos eWhich = pViewShell->GetViewData()->GetActivePart();
	    sal_Int32 nOldValue = pViewShell->GetViewData()->GetPosY(WhichV(eWhich)) + 1;
		pViewShell->ScrollLines(0, scrollRow - nOldValue);
	}
}

uno::Any SAL_CALL 
ScVbaWindow::getScrollColumn() throw (uno::RuntimeException)
{	
    sal_Int32 nValue = 0;
    ScTabViewShell* pViewShell = getBestViewShell( m_xModel );	
	if ( pViewShell )
	{
	    ScSplitPos eWhich = pViewShell->GetViewData()->GetActivePart();
	    nValue = pViewShell->GetViewData()->GetPosX(WhichH(eWhich));
	}
    
    return uno::makeAny( nValue + 1);
}

void SAL_CALL 
ScVbaWindow::setScrollColumn( const uno::Any& _scrollcolumn ) throw (uno::RuntimeException)
{
	ScTabViewShell* pViewShell = getBestViewShell( m_xModel );	
	if ( pViewShell )
	{
		sal_Int32 scrollColumn = 0;	
	    _scrollcolumn >>= scrollColumn;
	    ScSplitPos eWhich = pViewShell->GetViewData()->GetActivePart();
	    sal_Int32 nOldValue = pViewShell->GetViewData()->GetPosX(WhichH(eWhich)) + 1;
		pViewShell->ScrollLines(scrollColumn - nOldValue, 0);
	}
}

uno::Any SAL_CALL 
ScVbaWindow::getWindowState() throw (uno::RuntimeException)
{	
    sal_Int32 nwindowState = xlNormal;
    ScTabViewShell* pViewShell = getBestViewShell( m_xModel );	
    SfxViewFrame* pViewFrame = pViewShell -> GetViewFrame();
	SfxTopViewFrame *pTop= PTR_CAST( SfxTopViewFrame, pViewFrame -> GetTopViewFrame() );
	if ( pTop )
    {
        WorkWindow* pWork = (WorkWindow*) pTop->GetTopFrame_Impl()->GetSystemWindow();
        if ( pWork )
        {
            if ( pWork -> IsMaximized())
                nwindowState = xlMaximized;
            else if (pWork -> IsMinimized())
                nwindowState = xlMinimized;
        }
    }
    return uno::makeAny( nwindowState );
}

void SAL_CALL 
ScVbaWindow::setWindowState( const uno::Any& _windowstate ) throw (uno::RuntimeException)
{
	sal_Int32 nwindowState = xlMaximized;
	_windowstate >>= nwindowState;
	ScTabViewShell* pViewShell = getBestViewShell( m_xModel );	
	SfxViewFrame* pViewFrame = pViewShell -> GetViewFrame();
	SfxTopViewFrame *pTop= PTR_CAST( SfxTopViewFrame, pViewFrame -> GetTopViewFrame() );
	if ( pTop )
    {
        WorkWindow* pWork = (WorkWindow*) pTop->GetTopFrame_Impl()->GetSystemWindow();
        if ( pWork )
        {
            if ( nwindowState == xlMaximized)
                pWork -> Maximize();
            else if (nwindowState == xlMinimized)
                pWork -> Minimize();
            else if (nwindowState == xlNormal)
                pWork -> Restore();
            else
                throw uno::RuntimeException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Invalid Parameter" ) ), uno::Reference< uno::XInterface >() );
        }
    }
}

void
ScVbaWindow::Activate() throw (css::uno::RuntimeException)
{
	ScVbaWorkbook workbook( uno::Reference< vba::XHelperInterface >( ScVbaGlobals::getGlobalsImpl( mxContext )->getApplication(), uno::UNO_QUERY_THROW ), mxContext, m_xModel );

	workbook.Activate();
}

void
ScVbaWindow::Close( const uno::Any& SaveChanges, const uno::Any& FileName, const uno::Any& RouteWorkBook ) throw (uno::RuntimeException)
{
	ScVbaWorkbook workbook( uno::Reference< vba::XHelperInterface >( ScVbaGlobals::getGlobalsImpl( mxContext )->getApplication(), uno::UNO_QUERY_THROW ), mxContext, m_xModel );
	workbook.Close(SaveChanges, FileName, RouteWorkBook );
}

uno::Reference< excel::XPane > SAL_CALL
ScVbaWindow::ActivePane() throw (script::BasicErrorException, uno::RuntimeException) 
{ 
	return new ScVbaPane( mxContext, m_xViewPane ); 
}

uno::Reference< excel::XRange > SAL_CALL 
ScVbaWindow::ActiveCell(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	return ScVbaGlobals::getGlobalsImpl( mxContext )->getApplication()->getActiveCell();
}

uno::Any SAL_CALL 
ScVbaWindow::Selection(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	return ScVbaGlobals::getGlobalsImpl( mxContext )->getApplication()->getSelection();
}

::sal_Bool SAL_CALL 
ScVbaWindow::getDisplayGridlines() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( SHOWGRID ) );
	sal_Bool bGrid = sal_True;
	xProps->getPropertyValue( sName ) >>= bGrid;
	return bGrid;	
}


void SAL_CALL 
ScVbaWindow::setDisplayGridlines( ::sal_Bool _displaygridlines ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( SHOWGRID ) );
	xProps->setPropertyValue( sName, uno::makeAny( _displaygridlines ));
}

::sal_Bool SAL_CALL 
ScVbaWindow::getDisplayHeadings() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "HasColumnRowHeaders" ) );
	sal_Bool bHeading = sal_True;
	xProps->getPropertyValue( sName ) >>= bHeading;
	return bHeading;	
}

void SAL_CALL 
ScVbaWindow::setDisplayHeadings( ::sal_Bool _bDisplayHeadings ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "HasColumnRowHeaders" ) );
	xProps->setPropertyValue( sName, uno::makeAny( _bDisplayHeadings ));
}

::sal_Bool SAL_CALL 
ScVbaWindow::getDisplayHorizontalScrollBar() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( HASHORIZSCROLLBAR ) );
	sal_Bool bHorizontalScrollBar = sal_True;
	xProps->getPropertyValue( sName ) >>= bHorizontalScrollBar;
	return bHorizontalScrollBar;	
}

void SAL_CALL 
ScVbaWindow::setDisplayHorizontalScrollBar( ::sal_Bool _bDisplayHorizontalScrollBar ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( HASHORIZSCROLLBAR ) );
	xProps->setPropertyValue( sName, uno::makeAny( _bDisplayHorizontalScrollBar ));
}

::sal_Bool SAL_CALL 
ScVbaWindow::getDisplayOutline() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "IsOutlineSymbolsSet" ) );
	sal_Bool bOutline = sal_True;
	xProps->getPropertyValue( sName ) >>= bOutline;
	return bOutline;	
}

void SAL_CALL 
ScVbaWindow::setDisplayOutline( ::sal_Bool _bDisplayOutline ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "IsOutlineSymbolsSet" ) );
	xProps->setPropertyValue( sName, uno::makeAny( _bDisplayOutline ));
}

::sal_Bool SAL_CALL 
ScVbaWindow::getDisplayVerticalScrollBar() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( HASVERTSCROLLBAR ) );
	sal_Bool bVerticalScrollBar = sal_True;
	xProps->getPropertyValue( sName ) >>= bVerticalScrollBar;
	return bVerticalScrollBar;	
}

void SAL_CALL 
ScVbaWindow::setDisplayVerticalScrollBar( ::sal_Bool _bDisplayVerticalScrollBar ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( HASVERTSCROLLBAR ) );
	xProps->setPropertyValue( sName, uno::makeAny( _bDisplayVerticalScrollBar ));
}

::sal_Bool SAL_CALL 
ScVbaWindow::getDisplayWorkbookTabs() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "HasSheetTabs" ) );
	sal_Bool bWorkbookTabs = sal_True;
	xProps->getPropertyValue( sName ) >>= bWorkbookTabs;
	return bWorkbookTabs;	
}

void SAL_CALL 
ScVbaWindow::setDisplayWorkbookTabs( ::sal_Bool _bDisplayWorkbookTabs ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "HasSheetTabs" ) );
	xProps->setPropertyValue( sName, uno::makeAny( _bDisplayWorkbookTabs ));
}

::sal_Bool SAL_CALL 
ScVbaWindow::getFreezePanes() throw (uno::RuntimeException)
{
	return m_xViewFreezable->hasFrozenPanes();	
}

void SAL_CALL 
ScVbaWindow::setFreezePanes( ::sal_Bool _bFreezePanes ) throw (uno::RuntimeException)
{
	if( m_xViewSplitable->getIsWindowSplit() )
	{
		// if there is a split we freeze at the split
		sal_Int32 nColumn = getSplitColumn();
		sal_Int32 nRow = getSplitRow();
		m_xViewFreezable->freezeAtPosition( nColumn, nRow );
	}
	else
	{
		// otherwise we freeze in the center of the visible sheet	
		table::CellRangeAddress aCellRangeAddress = m_xViewPane->getVisibleRange();
		sal_Int32 nColumn = aCellRangeAddress.StartColumn + (( aCellRangeAddress.EndColumn - aCellRangeAddress.StartColumn )/2 );
		sal_Int32 nRow = aCellRangeAddress.StartRow + (( aCellRangeAddress.EndRow - aCellRangeAddress.StartRow )/2 );
		m_xViewFreezable->freezeAtPosition( nColumn, nRow );	
	}
}

::sal_Bool SAL_CALL 
ScVbaWindow::getSplit() throw (uno::RuntimeException)
{
	return m_xViewSplitable->getIsWindowSplit();	
}

void SAL_CALL 
ScVbaWindow::setSplit( ::sal_Bool _bSplit ) throw (uno::RuntimeException)
{
	if( !_bSplit )
	{
		m_xViewSplitable->splitAtPosition(0,0);
	}
	else
	{
		uno::Reference< excel::XRange > xRange = ActiveCell();
		sal_Int32 nRow = xRange->getRow();
		sal_Int32 nColumn = xRange->getColumn();
		m_xViewFreezable->freezeAtPosition( nColumn-1, nRow-1 );
		SplitAtDefinedPosition( sal_True );
	}
}

sal_Int32 SAL_CALL 
ScVbaWindow::getSplitColumn() throw (uno::RuntimeException)
{
	return m_xViewSplitable->getSplitColumn();	
}

void SAL_CALL 
ScVbaWindow::setSplitColumn( sal_Int32 _splitcolumn ) throw (uno::RuntimeException)
{
	if( getSplitColumn() != _splitcolumn )
	{
		sal_Bool bFrozen = getFreezePanes();
		sal_Int32 nRow = getSplitRow();
		m_xViewFreezable->freezeAtPosition( _splitcolumn, nRow );
		SplitAtDefinedPosition( !bFrozen );
	}
}

double SAL_CALL 
ScVbaWindow::getSplitHorizontal() throw (uno::RuntimeException)
{
	double fSplitHorizontal = m_xViewSplitable->getSplitHorizontal();
	double fHoriPoints = PixelsToPoints( m_xDevice, fSplitHorizontal, sal_True );
	return fHoriPoints;
}

void SAL_CALL 
ScVbaWindow::setSplitHorizontal( double _splithorizontal ) throw (uno::RuntimeException)
{
	double fHoriPixels = PointsToPixels( m_xDevice, _splithorizontal, sal_True );
   m_xViewSplitable->splitAtPosition( (int) fHoriPixels, 0 );	
}

sal_Int32 SAL_CALL 
ScVbaWindow::getSplitRow() throw (uno::RuntimeException)
{
	return m_xViewSplitable->getSplitRow();	
}

void SAL_CALL 
ScVbaWindow::setSplitRow( sal_Int32 _splitrow ) throw (uno::RuntimeException)
{
	if( getSplitRow() != _splitrow )
	{
		sal_Bool bFrozen = getFreezePanes();
		sal_Int32 nColumn = getSplitColumn();
		m_xViewFreezable->freezeAtPosition( nColumn , _splitrow );
		SplitAtDefinedPosition( !bFrozen );
	}
}

double SAL_CALL 
ScVbaWindow::getSplitVertical() throw (uno::RuntimeException)
{
	double fSplitVertical = m_xViewSplitable->getSplitVertical();
	double fVertiPoints = PixelsToPoints( m_xDevice, fSplitVertical, sal_False );	
	return fVertiPoints;
}

void SAL_CALL 
ScVbaWindow::setSplitVertical(double _splitvertical ) throw (uno::RuntimeException)
{
	double fVertiPixels = PointsToPixels( m_xDevice, _splitvertical, sal_False );
	m_xViewSplitable->splitAtPosition( 0, (int) fVertiPixels );
}

void ScVbaWindow::SplitAtDefinedPosition(sal_Bool _bUnFreezePane)
{
	sal_Int32 nVertSplit = m_xViewSplitable->getSplitVertical();
	sal_Int32 nHoriSplit = m_xViewSplitable->getSplitHorizontal();
	if( _bUnFreezePane )
	{
		m_xViewFreezable->freezeAtPosition(0,0);
	}
	m_xViewSplitable->splitAtPosition(nHoriSplit, nVertSplit);
}

uno::Any SAL_CALL 
ScVbaWindow::getZoom() throw (uno::RuntimeException)
{	
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "ZoomType" ) );
	sal_Int16 nZoomType = view::DocumentZoomType::PAGE_WIDTH;
	xProps->getPropertyValue( sName ) >>= nZoomType;
	if( nZoomType == view::DocumentZoomType::PAGE_WIDTH )
	{
		return uno::makeAny( sal_True );
	}
	else if( nZoomType == view::DocumentZoomType::BY_VALUE )
	{
		sName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ZoomValue"));
		sal_Int16 nZoom = 100;
		xProps->getPropertyValue( sName ) >>= nZoom;
		return uno::makeAny( nZoom );
	}
    return uno::Any();
}

void SAL_CALL 
ScVbaWindow::setZoom( const uno::Any& _zoom ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	rtl::OUString sZoomType( RTL_CONSTASCII_USTRINGPARAM( "ZoomType" ) );
	sal_Int16 nZoomType = view::DocumentZoomType::PAGE_WIDTH;
	if( _zoom.getValueTypeClass() == uno::TypeClass_BOOLEAN )
	{		
		//zoom type is PAGE_WIDTH_EXACT in helperapi, it seems that there is a issue for this zoom type in current OOo.
		// so PAGE_WIDTH is used.   	
		xProps->setPropertyValue(sZoomType, uno::makeAny( nZoomType ));
	}					
	else
	{
		nZoomType = view::DocumentZoomType::BY_VALUE;
		rtl::OUString sZoomValue( RTL_CONSTASCII_USTRINGPARAM( "ZoomValue" ));
		sal_Int16 nZoomValue = 100;
		_zoom >>= nZoomValue;
		xProps->setPropertyValue( sZoomType, uno::makeAny( nZoomType ));
		xProps->setPropertyValue( sZoomValue, uno::makeAny( nZoomValue ));
	}			
}

uno::Reference< excel::XWorksheet > SAL_CALL 
ScVbaWindow::ActiveSheet(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	return ScVbaGlobals::getGlobalsImpl(mxContext)->getApplication()->getActiveSheet();
}

uno::Any SAL_CALL
ScVbaWindow::getView() throw (uno::RuntimeException)
{
	// not supported now	
	sal_Int32 nWindowView = excel::XlWindowView::xlNormalView;	
	return uno::makeAny( nWindowView );	
}

void SAL_CALL
ScVbaWindow::setView( const uno::Any& _view) throw (uno::RuntimeException)
{
	sal_Int32 nWindowView = excel::XlWindowView::xlNormalView;
	_view >>= nWindowView;
	USHORT nSlot = FID_NORMALVIEWMODE;
	switch ( nWindowView )
	{
		case excel::XlWindowView::xlNormalView:
			nSlot = FID_NORMALVIEWMODE;
			break;
		case excel::XlWindowView::xlPageBreakPreview:
			nSlot = FID_PAGEBREAKMODE;
			break;
		default:
			DebugHelper::exception(SbERR_BAD_PARAMETER, rtl::OUString() );
	}
	dispatchExecute( m_xModel, nSlot );
}

sal_Bool SAL_CALL
ScVbaWindow::getVisible() throw (uno::RuntimeException)
{
	sal_Bool bVisible = sal_True;	
	uno::Reference< frame::XController > xController( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	uno::Reference< css::awt::XWindow > xWindow (xController->getFrame()->getContainerWindow(), uno::UNO_QUERY_THROW );
	uno::Reference< css::awt::XWindow2 > xWindow2 (xWindow, uno::UNO_QUERY_THROW );
	if( xWindow2.is() )
	{
		bVisible = xWindow2->isVisible();	
	}
	return bVisible;
}

void SAL_CALL
ScVbaWindow::setVisible(sal_Bool _visible) throw (uno::RuntimeException)
{
	uno::Reference< frame::XController > xController( m_xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	uno::Reference< css::awt::XWindow > xWindow (xController->getFrame()->getContainerWindow(), uno::UNO_QUERY_THROW );
	if( xWindow.is() )
	{
		xWindow->setVisible( _visible );	
	}
}

css::awt::Rectangle getPosSize( const uno::Reference< frame::XModel >& xModel )
{
	css::awt::Rectangle aRect;
	uno::Reference< frame::XController > xController( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	uno::Reference< css::awt::XWindow > xWindow (xController->getFrame()->getContainerWindow(), uno::UNO_QUERY_THROW );
	if( xWindow.is() )
	{
		aRect = xWindow->getPosSize();
	}
	return aRect;
}

void setPosSize( const uno::Reference< frame::XModel >& xModel, sal_Int32 nValue, USHORT nFlag )
{
	uno::Reference< frame::XController > xController( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	uno::Reference< css::awt::XWindow > xWindow (xController->getFrame()->getContainerWindow(), uno::UNO_QUERY_THROW );
	if( xWindow.is() )
	{
		css::awt::Rectangle aRect = xWindow->getPosSize();
		switch( nFlag )
		{
			case css::awt::PosSize::X:
				xWindow->setPosSize( nValue, aRect.Y,	0, 0, css::awt::PosSize::X );
				break;
			case css::awt::PosSize::Y:
				xWindow->setPosSize( aRect.X, nValue,	0, 0, css::awt::PosSize::Y );
				break;
			case css::awt::PosSize::WIDTH:
				xWindow->setPosSize( 0, 0,	nValue, aRect.Height, css::awt::PosSize::WIDTH );
				break;
			case css::awt::PosSize::HEIGHT:
				xWindow->setPosSize( 0, 0,	aRect.Width, nValue, css::awt::PosSize::HEIGHT );
				break;
			default:
				break;
		}
	}
}	

sal_Int32 SAL_CALL 
ScVbaWindow::getHeight() throw (uno::RuntimeException)
{
	css::awt::Rectangle aRect = getPosSize(m_xModel);
	return aRect.Height;
}

void SAL_CALL 
ScVbaWindow::setHeight( sal_Int32 _height ) throw (uno::RuntimeException)
{
	setPosSize(m_xModel, _height, css::awt::PosSize::HEIGHT);
}

sal_Int32 SAL_CALL 
ScVbaWindow::getLeft() throw (uno::RuntimeException)
{
	css::awt::Rectangle aRect = getPosSize(m_xModel);
	return aRect.X;
}

void SAL_CALL 
ScVbaWindow::setLeft( sal_Int32 _left ) throw (uno::RuntimeException)
{
	setPosSize(m_xModel, _left, css::awt::PosSize::X);
}
sal_Int32 SAL_CALL 
ScVbaWindow::getTop() throw (uno::RuntimeException)
{
	css::awt::Rectangle aRect = getPosSize(m_xModel);
	return aRect.Y;
}

void SAL_CALL 
ScVbaWindow::setTop( sal_Int32 _top ) throw (uno::RuntimeException)
{
	setPosSize(m_xModel, _top, css::awt::PosSize::Y);
}
sal_Int32 SAL_CALL 
ScVbaWindow::getWidth() throw (uno::RuntimeException)
{
	css::awt::Rectangle aRect = getPosSize(m_xModel);
	return aRect.Width;
}

void SAL_CALL 
ScVbaWindow::setWidth( sal_Int32 _width ) throw (uno::RuntimeException)
{
	setPosSize(m_xModel, _width, css::awt::PosSize::WIDTH);
}

sal_Int32 SAL_CALL 
ScVbaWindow::PointsToScreenPixelsX(sal_Int32 _points) throw (css::script::BasicErrorException, css::uno::RuntimeException)
{
	sal_Int32 nHundredthsofOneMillimeters = Millimeter::getInHundredthsOfOneMillimeter( _points );
	double fConvertFactor = (m_xDevice->getInfo().PixelPerMeterX/100000);
	return static_cast<sal_Int32>(fConvertFactor * nHundredthsofOneMillimeters );
}

sal_Int32 SAL_CALL 
ScVbaWindow::PointsToScreenPixelsY(sal_Int32 _points) throw (css::script::BasicErrorException, css::uno::RuntimeException)
{
	sal_Int32 nHundredthsofOneMillimeters = Millimeter::getInHundredthsOfOneMillimeter( _points );
	double fConvertFactor = (m_xDevice->getInfo().PixelPerMeterY/100000);
	return static_cast<sal_Int32>(fConvertFactor * nHundredthsofOneMillimeters );
}

void SAL_CALL 
ScVbaWindow::PrintOut( const css::uno::Any& From, const css::uno::Any&To, const css::uno::Any& Copies, const css::uno::Any& Preview, const css::uno::Any& ActivePrinter, const css::uno::Any& PrintToFile, const css::uno::Any& Collate, const css::uno::Any& PrToFileName ) throw (css::script::BasicErrorException, css::uno::RuntimeException)
{
	// need test, print current active sheet	
	PrintOutHelper( From, To, Copies, Preview, ActivePrinter, PrintToFile, Collate, PrToFileName, m_xModel, sal_True );
}

void SAL_CALL 
ScVbaWindow::PrintPreview( const css::uno::Any& EnableChanges ) throw (css::script::BasicErrorException, css::uno::RuntimeException)
{
	// need test, print preview current active sheet	
	PrintPreviewHelper( EnableChanges, m_xModel );	
}

rtl::OUString& 
ScVbaWindow::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaWindow") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaWindow::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.excel.Window" ) );
	}
	return aServiceNames;
}
