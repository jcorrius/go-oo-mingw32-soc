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
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/sheet/XSheetOutline.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <tools/string.hxx>

#include "vbaoutline.hxx"
#include "vbarange.hxx"
#include "vbaworksheet.hxx"

#define STANDARDWIDTH 2267 
#define STANDARDHEIGHT 427
#define DOESNOTEXIST -1

static sal_Int16 
nameExists( uno::Reference <sheet::XSpreadsheetDocument>& xSpreadDoc, ::rtl::OUString & name)
{
	uno::Reference <sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
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
				return i;
			}
		}
	}
	return DOESNOTEXIST;
}

static void getNewSpreadsheetName (rtl::OUString &aNewName, rtl::OUString aOldName, uno::Reference <sheet::XSpreadsheetDocument>& xSpreadDoc )
{
	rtl::OUString aUnderScre( RTL_CONSTASCII_USTRINGPARAM( "_" ) );
	int currentNum =2;
	aNewName = aOldName + aUnderScre+
					String::CreateFromInt32(currentNum) ;
	while ( nameExists(xSpreadDoc,aNewName) != DOESNOTEXIST )
	{
		aNewName = aOldName + aUnderScre +
		String::CreateFromInt32(++currentNum) ;
	}
}

static void removeAllSheets( uno::Reference <sheet::XSpreadsheetDocument>& xSpreadDoc, rtl::OUString aSheetName)
{
	uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
	uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );

	if ( xIndex.is() )
	{
		uno::Reference<container::XNameContainer> xNameContainer(xSheets,uno::UNO_QUERY_THROW);
		for (sal_Int32 i = xIndex->getCount() -1; i>= 1; i--)
		{
			uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(i), uno::UNO_QUERY);
			uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
			if (xNamed.is())
			{
				xNameContainer->removeByName(xNamed->getName());
			}
		}

		uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(0), uno::UNO_QUERY);                uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
		if (xNamed.is())
		{
			xNamed->setName(aSheetName);
		}
	}
}

static uno::Reference<frame::XModel>
openNewDoc(rtl::OUString aSheetName )
{
	uno::Reference<frame::XModel> xModel;
	try
	{
		uno::Reference<uno::XComponentContext > xContext(  ::cppu::defaultBootstrap_InitialComponentContext());
		if ( !xContext.is() )
		{
			return xModel;
		}
		uno::Reference<lang::XMultiComponentFactory > xServiceManager(
										xContext->getServiceManager() );
		if ( !xServiceManager.is() )
		{
			return xModel;
		}
		uno::Reference <frame::XComponentLoader > xComponentLoader(
						xServiceManager->createInstanceWithContext(
						rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ) ),
						xContext ), uno::UNO_QUERY_THROW );

		uno::Reference<lang::XComponent > xComponent( xComponentLoader->loadComponentFromURL(
				rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:factory/scalc" ) ),
				rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_blank" ) ), 0,
				uno::Sequence < ::com::sun::star::beans::PropertyValue >() ) );
		uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xComponent, uno::UNO_QUERY_THROW );
		if ( xSpreadDoc.is() )
		{
			removeAllSheets(xSpreadDoc,aSheetName);
		}
		uno::Reference<frame::XModel> xModel(xSpreadDoc,uno::UNO_QUERY_THROW);
		return xModel;
	}
	catch ( ::cppu::BootstrapException & e )    
	{
		return xModel;
	}
	catch ( uno::Exception & e )
	{
		return xModel;
	}
}


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
	uno::Reference<sheet::XUsedAreaCursor> xUsedCursor(xSheetCellCursor,uno::UNO_QUERY_THROW);
	xUsedCursor->gotoStartOfUsedArea( false );
	xUsedCursor->gotoEndOfUsedArea( true );
	uno::Reference< table::XCellRange > xRange( xSheetCellCursor, uno::UNO_QUERY);
	return new ScVbaRange(m_xContext, xRange);
}

uno::Reference< vba::XOutline >
ScVbaWorksheet::Outline( ) throw (uno::RuntimeException)
{
	uno::Reference<sheet::XSheetOutline> xOutline(mxSheet,uno::UNO_QUERY_THROW);
	return uno::Reference<vba::XOutline> (new ScVbaOutline(m_xContext, xOutline));
}

sal_Int32 
ScVbaWorksheet::getStandardWidth() throw (uno::RuntimeException)
{
	return STANDARDWIDTH ;
}

sal_Int32 
ScVbaWorksheet::getStandardHeight() throw (uno::RuntimeException)
{
	return STANDARDHEIGHT;
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
ScVbaWorksheet::Select() throw (uno::RuntimeException)
{
	Activate();
}

void 
ScVbaWorksheet::Move( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException) 
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
	rtl::OUString aSheetName;
	uno::Reference<vba::XWorksheet> xSheet;
	rtl::OUString aCurrSheetName =getName();

	if (!(Before >>= xSheet) && !(After >>=xSheet)&& !(Before.hasValue()) && !(After.hasValue()))
	{
		uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = mxSheet->createCursor( );
		uno::Reference<sheet::XUsedAreaCursor> xUsedCursor(xSheetCellCursor,uno::UNO_QUERY_THROW);
        	uno::Reference< table::XCellRange > xRange1( xSheetCellCursor, uno::UNO_QUERY);
		uno::Reference<vba::XRange> xRange =  new ScVbaRange(m_xContext, xRange1);
		if (xRange.is())
			xRange->Select();
		implnCopy();
		uno::Reference<frame::XModel> xModel = openNewDoc(aCurrSheetName);
		if (xModel.is()) 
		{
			implnPaste();
			Delete();
		}
		return ;
	}

	sal_Int32 nDest = DOESNOTEXIST;
	sal_Bool bAfter = false;
	if (Before >>= xSheet )
	{
		aSheetName = xSheet->getName();
		nDest = nameExists (xSpreadDoc, aSheetName);
	}
	else if (After >>= xSheet)
	{
		aSheetName = xSheet->getName();
		nDest = nameExists (xSpreadDoc, aSheetName);
		bAfter =true;
	}

	if (nDest != DOESNOTEXIST)
	{
		if (bAfter)  nDest++;
		uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		xSheets->moveByName(aCurrSheetName,nDest);
	}
}

void 
ScVbaWorksheet::Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException) 
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
	rtl::OUString aSheetName;
	uno::Reference<vba::XWorksheet> xSheet;
	rtl::OUString aCurrSheetName =getName();
	if (!(Before >>= xSheet) && !(After >>=xSheet)&& !(Before.hasValue()) && !(After.hasValue()))
	{
		uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = mxSheet->createCursor( );
		uno::Reference<sheet::XUsedAreaCursor> xUsedCursor(xSheetCellCursor,uno::UNO_QUERY_THROW);
        	uno::Reference< table::XCellRange > xRange1( xSheetCellCursor, uno::UNO_QUERY);
		uno::Reference<vba::XRange> xRange =  new ScVbaRange(m_xContext, xRange1);
		if (xRange.is())
			xRange->Select();
		implnCopy();
		uno::Reference<frame::XModel> xModel = openNewDoc(aCurrSheetName);
		if (xModel.is())
		{
			implnPaste();
		}
		return;
	}

	sal_Int32 nDest = DOESNOTEXIST;
	sal_Bool bAfter = false;
	if (Before >>= xSheet )
	{
		aSheetName = xSheet->getName();
		nDest = nameExists (xSpreadDoc, aSheetName);
	}
	else if (After >>= xSheet)
	{
		aSheetName = xSheet->getName();
		nDest = nameExists (xSpreadDoc, aSheetName);
		bAfter =true;
	}

	if (nDest != DOESNOTEXIST)
	{
		if(bAfter)  nDest++;
		uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		getNewSpreadsheetName(aSheetName,aCurrSheetName,xSpreadDoc);
		xSheets->copyByName(aCurrSheetName,aSheetName,nDest);
	}
}

void 
ScVbaWorksheet::Paste( const uno::Any& Destination, const uno::Any& Link ) throw (uno::RuntimeException)
{
	uno::Reference<vba::XRange> xRange;
	if (Destination >>= xRange)
		xRange->Select();
	implnPaste();
}

void 
ScVbaWorksheet::Delete() throw (uno::RuntimeException)
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY_THROW );
	rtl::OUString aSheetName = getName();
	if ( xSpreadDoc.is() )
	{
		if (!nameExists(xSpreadDoc, aSheetName)) 
		{
			return;
		}
		uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		if (xSheets.is())
		{
			uno::Reference<container::XNameContainer> xNameContainer(xSheets,uno::UNO_QUERY_THROW);
			xNameContainer->removeByName(aSheetName);
		}
	}
}

uno::Reference< vba::XWorksheet >
ScVbaWorksheet::getSheetAtOffset(int offset) throw (uno::RuntimeException)
{
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
	uno::Reference <sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
	uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY_THROW );

	rtl::OUString aName = getName();
	sal_Int16 nIdx = nameExists (xSpreadDoc, aName);
	if (nIdx >= 0)
		nIdx += offset;

	if (nIdx < 0 || nIdx >= xIndex->getCount()) // TESTME - throw ?
		return uno::Reference< vba::XWorksheet >();
	else
	{
		uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(nIdx), uno::UNO_QUERY_THROW);
		return new ScVbaWorksheet (m_xContext, xSheet, mxModel);
	}
}

uno::Reference< vba::XWorksheet >
ScVbaWorksheet::getNext() throw (uno::RuntimeException)
{
	return getSheetAtOffset(1);
}

uno::Reference< vba::XWorksheet >
ScVbaWorksheet::getPrevious() throw (uno::RuntimeException)
{
	return getSheetAtOffset(-1);
}

void
ScVbaWorksheet::Protect( const uno::Any& Password, const uno::Any& DrawingObjects, const uno::Any& Contents, const uno::Any& Scenarios, const uno::Any& UserInterfaceOnly ) throw (uno::RuntimeException)
{
	uno::Reference<util::XProtectable > xProtectable(mxSheet, uno::UNO_QUERY_THROW);
	::rtl::OUString aPasswd;
	Password >>= aPasswd;
	xProtectable->protect( aPasswd );
}

void 
ScVbaWorksheet::Unprotect( const uno::Any& Password ) throw (uno::RuntimeException)
{
	uno::Reference<util::XProtectable > xProtectable(mxSheet, uno::UNO_QUERY_THROW);
	::rtl::OUString aPasswd;
	Password >>= aPasswd;
	xProtectable->unprotect( aPasswd );
}

void 
ScVbaWorksheet::Calculate() throw (uno::RuntimeException)
{
	uno::Reference <sheet::XCalculatable> xReCalculate(mxModel, uno::UNO_QUERY_THROW);
	xReCalculate->calculate();
}

uno::Reference< vba::XRange >
ScVbaWorksheet::Range( const ::uno::Any &rRange ) throw (uno::RuntimeException)
{
	rtl::OUString aStringRange;
	rRange >>= aStringRange;
	uno::Reference< table::XCellRange > xRanges( mxSheet, uno::UNO_QUERY_THROW );
	if (xRanges.is()) 
	{
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRanges->getCellRangeByName( aStringRange ) ) );
	}
	return NULL;
}

void
ScVbaWorksheet::CheckSpelling( const uno::Any& CustomDictionary,const uno::Any& IgnoreUppercase,const uno::Any& AlwaysSuggest, const uno::Any& SpellingLang ) throw (uno::RuntimeException)
{
	rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:SpellDialog"));
	dispatchRequests(mxModel,url);
}

uno::Reference< vba::XRange > 
ScVbaWorksheet::getSheetRange() throw (uno::RuntimeException)
{
	uno::Reference< table::XCellRange > xRange( mxSheet,uno::UNO_QUERY_THROW );
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRange ) );
}

// These are hacks - we prolly (somehow) need to inherit
// the vbarange functionality here ...
uno::Reference< vba::XRange > 
ScVbaWorksheet::Cells( const ::uno::Any &nRow, const ::uno::Any &nCol )
		throw (uno::RuntimeException)
{
	return getSheetRange()->Cells( nRow, nCol );
}

uno::Any
ScVbaWorksheet::Rows(const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	return getSheetRange()->Rows( aIndex );
}

uno::Any
ScVbaWorksheet::Columns( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	return getSheetRange()->Columns( aIndex );
}
