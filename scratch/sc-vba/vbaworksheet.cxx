#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XCalculatable.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include <com/sun/star/sheet/XSheetCellCursor.hpp>
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/sheet/XSheetPastable.hpp>
#include <com/sun/star/sheet/XCellAddressable.hpp>
#include <viewuno.hxx>
#include "vbarange.hxx"
#include "vbaworksheet.hxx"

::rtl::OUString
ScVbaWorksheet::getName() throw (uno::RuntimeException)
{
	uno::Reference< container::XNamed > xNamed( mxSheet, uno::UNO_QUERY_THROW );
	return xNamed->getName();
}

void
ScVbaWorksheet::setName(const ::rtl::OUString &rName ) throw (uno::RuntimeException)
{
	uno::Reference< container::XNamed > xNamed( mxSheet, uno::UNO_QUERY_THROW );
	xNamed->setName( rName );
}

sal_Bool
ScVbaWorksheet::getVisible() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( mxSheet, uno::UNO_QUERY_THROW );
	uno::Any aValue = xProps->getPropertyValue
			(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsVisible" ) ) );
	sal_Bool bRet;
	aValue >>= bRet;
	return bRet;
}

void
ScVbaWorksheet::setVisible( sal_Bool bVisible ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( mxSheet, uno::UNO_QUERY_THROW );
	uno::Any aValue( bVisible );
	xProps->setPropertyValue
			(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsVisible" ) ), aValue);
}

uno::Reference< vba::XRange > 
ScVbaWorksheet::getUsedRange() throw (uno::RuntimeException)
{
 	uno::Reference< sheet::XSheetCellRange > xSheetCellRange(mxSheet, uno::UNO_QUERY);
	uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = mxSheet->createCursorByRange( xSheetCellRange );
	xSheetCellCursor->collapseToCurrentArray();
	uno::Reference<sheet::XUsedAreaCursor> xUsedCursor(xSheetCellCursor,uno::UNO_QUERY_THROW);
	xUsedCursor->gotoStartOfUsedArea( false );
	xUsedCursor->gotoEndOfUsedArea( true );

	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xSheetCellCursor, uno::UNO_QUERY);

	uno::Reference< table::XCellRange > xRange( xSheetCellCursor, uno::UNO_QUERY);
	return uno::Reference< vba::XRange >( new ScVbaRange( xRange->getCellRangeByPosition(
                                        xCellRangeAddressable->getRangeAddress().StartColumn,
                                        xCellRangeAddressable->getRangeAddress().StartRow,
                                        xCellRangeAddressable->getRangeAddress().EndColumn,
                                        xCellRangeAddressable->getRangeAddress().EndRow ) ) );
}

sal_Int32 
ScVbaWorksheet::getStandardWidth() throw (uno::RuntimeException)
{
	return 2267;
}

sal_Int32 
ScVbaWorksheet::getStandardHeight() throw (uno::RuntimeException)
{
	return 427;
}

sal_Bool 
ScVbaWorksheet::getProtectionMode() throw (uno::RuntimeException) 
{
	return false;
}

sal_Bool
ScVbaWorksheet::getProtectContents()throw (uno::RuntimeException) 
{
	uno::Reference<util::XProtectable > xProtectable(mxSheet, uno::UNO_QUERY_THROW);
	return xProtectable->isProtected();
}

sal_Bool 
ScVbaWorksheet::getProtectDrawingObjects() throw (uno::RuntimeException) 
{
	return false;
}

void
ScVbaWorksheet::Activate() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSpreadsheetView > xSpreadsheet(
        	mxModel->getCurrentController(), uno::UNO_QUERY_THROW );
	xSpreadsheet->setActiveSheet(mxSheet);	
}

void 
ScVbaWorksheet::Move( const uno::Any& before, const uno::Any& after ) throw (uno::RuntimeException) 
{

	sal_Int32 nDest=0;
	rtl::OUString aBefore;
	rtl::OUString aAfter;
	before >>= aBefore;
	after >>= aAfter;

	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
	uno::Reference< sheet::XSpreadsheetView > xSpreadsheetView(
                        mxModel->getCurrentController(), uno::UNO_QUERY_THROW );
	uno::Reference<sheet::XSpreadsheet> xSpreadSheet = xSpreadsheetView->getActiveSheet();
	uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
	uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
       	sal_Int16 nCount = xIndex->getCount();

	if (aBefore==NULL && aAfter==NULL) 
	{
	        if ( xIndex.is() )
        	{
			//FIXME Ideally it should open a new workbook 
			xSheets->moveByName(getName(),1 + nCount);	
                	return ;
        	}

	}

	if (aBefore) 
	{
        	if ( xIndex.is() )
        	{
	                for (sal_Int16 i=0; i < nCount; i++)
        	        {
                	        uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(i), uno::UNO_QUERY);
                        	uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
                        if (xNamed->getName() == aBefore)
                        {
                                nDest = i - 1;
                                break;
                        }
                }
        }
	
	}
	
	if (nDest >= 0)
		xSheets->moveByName(getName(),nDest);	
	else
		xSheets->moveByName(getName(),0);

}

static sal_Bool 
nameExsists( uno::Reference <sheet::XSpreadsheetDocument>& xSpreadDoc, ::rtl::OUString & name)
{
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
              sal_Int32  nCount = xIndex->getCount();
                for (sal_Int32 i=0; i < nCount; i++)
                {
                        uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(i), uno::UNO_QUERY);
                        uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
                        if (xNamed->getName() == name)
                        {
				return true;
                        }
                }
        }
return false;

}

void 
ScVbaWorksheet::Copy( const uno::Any& before, const uno::Any& after ) throw (uno::RuntimeException) 
{
	sal_Int32 nDest=0;
	rtl::OUString aBefore;
	rtl::OUString aAfter;
	before >>= aBefore;
	after >>= aAfter;
	rtl::OUString aName( RTL_CONSTASCII_USTRINGPARAM( "Sheet" ) );
	rtl::OUString sheetName = getName();

	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
	uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
	uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
       	sal_Int16 nCount = xIndex->getCount();

	if (aBefore==NULL && aAfter==NULL) 
	{
		if ( xIndex.is() )
		{
			//FIXME Ideally it should open a new workbook and check if the new name exsists
			if (!nameExsists(xSpreadDoc,sheetName)) {
				printf("Name does not exsists\n");
				return;
			}
			aName += String::CreateFromInt32((1+nCount));
			xSheets->copyByName(getName(),aName,nCount);
			return ;
        }
	}

	if (aBefore) 
	{
		if ( xIndex.is() )
		{
			for (sal_Int16 i=0; i < nCount; i++)
			{
				uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(i), uno::UNO_QUERY);
				uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
				if (xNamed->getName() == aBefore)
				{
					nDest = i - 1;
					break;
				}
			}
		}
	}
	
	aName= getName() ;
		aName += String::CreateFromInt32((1+nCount));
	if (nDest >= 0)
		xSheets->copyByName(getName(),aName,nDest);	
	else
		xSheets->copyByName(getName(),aName,0);

}

void 
ScVbaWorksheet::Paste( const uno::Any& Destination, const uno::Any& Link ) throw (uno::RuntimeException)
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
	uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
	uno::Reference<sheet::XSheetPastable> xsheetpastable(mxModel,uno::UNO_QUERY_THROW);

	uno::Reference<table::XCellRange> xCellRange(mxSheet,uno::UNO_QUERY_THROW);
        uno::Reference<table::XCell> xCell = xCellRange->getCellByPosition( 6,4 );
	uno::Reference<sheet::XCellAddressable> xCellAddr(xCell,uno::UNO_QUERY_THROW);

	table::CellAddress xcelladdress = xCellAddr->getCellAddress();
	//FIXME : XSheetPastable not implemented by OOo
	xsheetpastable->paste(xcelladdress);
}

void 
ScVbaWorksheet::Delete(  ) throw (uno::RuntimeException)
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY_THROW );
	rtl::OUString sheetName = getName();
	if (!nameExsists(xSpreadDoc, sheetName)) 
	{
		return;
	}
	if ( xSpreadDoc.is() )
	{
		uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		if (xSheets.is())
		{
			uno::Reference<container::XNameContainer> xNameContainer(xSheets,uno::UNO_QUERY_THROW);
			xNameContainer->removeByName(getName());
		}
	}
}

void
ScVbaWorksheet::Protect( const uno::Any& Password, const uno::Any& DrawingObjects, const uno::Any& Contents, const uno::Any& Scenarios, const uno::Any& UserInterfaceOnly ) throw (uno::RuntimeException)
{
	uno::Reference<util::XProtectable > xProtectable(mxSheet, uno::UNO_QUERY_THROW);
	::rtl::OUString aStringBase;
	Password >>= aStringBase;
	xProtectable->protect( aStringBase );
}

void 
ScVbaWorksheet::Unprotect( const uno::Any& Password ) throw (uno::RuntimeException)
{
	uno::Reference<util::XProtectable > xProtectable(mxSheet, uno::UNO_QUERY_THROW);
	::rtl::OUString aStringBase;
	Password >>= aStringBase;
	xProtectable->unprotect( aStringBase );
}

void 
ScVbaWorksheet::Calculate(  ) throw (uno::RuntimeException)
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY_THROW); 
	if ( xSpreadDoc.is() )
	{
		uno::Reference< frame::XModel > xModel(xSpreadDoc,uno::UNO_QUERY_THROW);
		uno::Reference <sheet::XCalculatable> xReCalculate(xModel, uno::UNO_QUERY_THROW);
		xReCalculate->calculate();
	}
}

uno::Reference< vba::XRange >
ScVbaWorksheet::Range( const ::uno::Any &rRange ) throw (uno::RuntimeException)
{
	rtl::OUString aStringRange;
	rRange >>= aStringRange;
	uno::Reference< table::XCellRange > xRanges( mxSheet, uno::UNO_QUERY_THROW );
	if (xRanges.is()) 
	{
		return uno::Reference< vba::XRange >( new ScVbaRange( xRanges->getCellRangeByName( aStringRange ) ) );
	}
	return NULL;
}

