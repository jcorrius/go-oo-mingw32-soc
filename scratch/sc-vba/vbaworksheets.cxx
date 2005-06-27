#include <cppuhelper/queryinterface.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <tools/string.hxx>

#include "vbaglobals.hxx"
#include "vbaworksheet.hxx"
#include "vbaworksheets.hxx"
#include "vbaworkbook.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XInterface implementation
uno::Any ScVbaWorksheets::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XWorksheets * >( this ) ),
			 static_cast< vba::XWorksheets * >( this ),
			 static_cast< container::XEnumerationAccess * >( this ) );
}


// XEnumerationAccess
uno::Type
ScVbaWorksheets::getElementType() throw (uno::RuntimeException)
{
	return vba::XWorksheet::static_type(0);
}
::sal_Bool
ScVbaWorksheets::hasElements() throw (uno::RuntimeException)
{
	return sal_True;
}
uno::Reference< container::XEnumeration >
ScVbaWorksheets::createEnumeration() throw (uno::RuntimeException)
{
	return uno::Reference< container::XEnumeration >( NULL );
}

// XCollection
uno::Any
ScVbaWorksheets::getParent() throw (uno::RuntimeException)
{
	return uno::Any( ScVbaGlobals::get()->getApplication()->getActiveWorkbook() );
}
::sal_Int32
ScVbaWorksheets::getCreator() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return 0;
}
uno::Reference< vba::XApplication >
ScVbaWorksheets::getApplication() throw (uno::RuntimeException)
{
	return ScVbaGlobals::get()->getApplication();
}

::sal_Int32
ScVbaWorksheets::getCount() throw (uno::RuntimeException)
{
	uno::Reference <container::XIndexAccess> xIndex( mxSheets, uno::UNO_QUERY );
	if ( xIndex.is() )
	{
		return xIndex->getCount();
	}
	return 0;
}

uno::Any
ScVbaWorksheets::Item( ::sal_Int32 nIndex ) throw (uno::RuntimeException)
{
	return uno::Any( uno::Reference< vba::XWorksheet >(NULL) );
}

// XWorksheets
uno::Any
ScVbaWorksheets::Add( const uno::Any& Before, const uno::Any& After,
					 const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException)
{
	rtl::OUString aStringSheet;
	sal_Bool bBefore(sal_True);
	sal_Int32 nSheetIndex;
	sal_Int32 nNewSheets = 1, nType = 0;
	Count >>= nNewSheets;
	Type >>= nType;
	sal_Int32 nCount = 0;
	Before >>= aStringSheet;
	if (aStringSheet == NULL)
	{
		After >>= aStringSheet;
		bBefore = sal_False;
	}
	if (aStringSheet == NULL)
	{
		aStringSheet = ScVbaGlobals::get()->getApplication()->getActiveWorkbook()->getActiveSheet()->getName();
		bBefore = sal_True;
	}
	uno::Reference <container::XIndexAccess> xIndex( mxSheets, uno::UNO_QUERY );
	if ( xIndex.is() )
	{
		nCount = xIndex->getCount();
		for (sal_Int32 i=0; i < nCount; i++)
		{
			uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(i), uno::UNO_QUERY);
			uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
			if (xNamed->getName() == aStringSheet)
			{
				nSheetIndex = i;
				break;
			}
		}
	}

	if(!bBefore)
		nSheetIndex++;

	uno::Reference< container::XNameAccess > xNameAccess( mxSheets, uno::UNO_QUERY_THROW );
	sal_Int32 nSheetName = nCount + 1L;
	String aStringBase( RTL_CONSTASCII_USTRINGPARAM("Sheet") );				
	for (sal_Int32 i=0; i < nNewSheets; i++, nSheetName++)
	{
		String aStringName = aStringBase;
		aStringName += String::CreateFromInt32(nSheetName);
		while (xNameAccess->hasByName(aStringName))
		{
			nSheetName++;
			aStringName = aStringBase;
			aStringName += String::CreateFromInt32(nSheetName);
		}
		mxSheets->insertNewByName(aStringName, nSheetIndex + i);
	}
	return uno::Any();
}

void
ScVbaWorksheets::Delete() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
