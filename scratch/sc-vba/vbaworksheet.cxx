#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>

#include <sfx2/objsh.hxx>

#include "vbarange.hxx"
#include "vbaworksheet.hxx"

// XInterface implementation
uno::Any ScVbaWorksheet::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XWorksheet * >( this ) ),
			 static_cast< vba::XWorksheet * >( this ) );
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
			(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Visible" ) ) );
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
			(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Visible" ) ), aValue);
}

void
ScVbaWorksheet::Activate() throw (uno::RuntimeException)
{
	// FIXME:
/*	ThisComponent.CurrentController.setActiveSheet( this ); */
}

uno::Reference< vba::XRange >
ScVbaWorksheet::Range( const ::uno::Any &rRange ) throw (uno::RuntimeException)
{
	rtl::OUString aStringRange;
	rRange >>= aStringRange;
	uno::Reference< table::XCellRange > xRanges( mxSheet, uno::UNO_QUERY_THROW );
	return uno::Reference< vba::XRange >( new ScVbaRange( xRanges->getCellRangeByName( aStringRange ) ) );
}

void
ScVbaWorksheet::Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
void
ScVbaWorksheet::Move( const ::uno::Any& Before, const ::uno::Any& After ) throw (uno::RuntimeException)
{
	rtl::OUString aStringSheet;
	Before >>= aStringSheet;
	sal_Int32 nSheetIndex = -1;

	//FIXME: Don't get the currentshell; get the Sheets collection to which the sheet belongs to
        SfxObjectShell* pObjSh = SfxObjectShell::Current();
        if ( pObjSh )
        {
            uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( pObjSh->GetModel(), uno::UNO_QUERY );
            if ( xSpreadDoc.is() )
            {
                uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		if (xSheets.is())
		{
			uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
			if ( xIndex.is() )
			{
				sal_Int32 nCount = xIndex->getCount();
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
		}
		if(nSheetIndex != -1)
		{
			if (nSheetIndex)
				nSheetIndex--;
			xSheets->moveByName(getName(), nSheetIndex);
		}
            }
        }
}
