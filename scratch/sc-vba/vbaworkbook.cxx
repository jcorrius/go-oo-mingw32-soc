#include <tools/urlobj.hxx>

#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>

#include "vbaworksheet.hxx"
#include "vbaworksheets.hxx"
#include "vbaworkbook.hxx"

// Much of the impl. for the equivalend UNO module is
// sc/source/ui/unoobj/docuno.cxx, viewuno.cxx

using namespace ::org::openoffice;
using namespace ::com::sun::star;

::rtl::OUString
ScVbaWorkbook::getName() throw (uno::RuntimeException)
{
	INetURLObject aURL( mxModel->getURL() );
	return aURL.GetLastName();
}
::rtl::OUString
ScVbaWorkbook::getPath() throw (uno::RuntimeException)
{
	INetURLObject aURL( mxModel->getURL() );
	return aURL.GetURLPath();
}

::rtl::OUString
ScVbaWorkbook::getFullName() throw (uno::RuntimeException)
{
	SC_VBA_FIXME(("Workbook::FullName unclear / needs work"));
	return getName();
}

uno::Reference< vba::XWorksheet >
ScVbaWorkbook::getActiveSheet() throw (uno::RuntimeException)
{
	OSL_TRACE("In ScVbaWorkbook::getActiveSheet()");
	uno::Reference< sheet::XSpreadsheetView > xSpreadsheet(
			mxModel->getCurrentController(), uno::UNO_QUERY_THROW );

	uno::Reference< vba::XWorksheet > xWrkSt( new ScVbaWorksheet( m_xContext,xSpreadsheet->getActiveSheet() ,mxModel ));

	if ( xWrkSt.is() )
	{
		OSL_TRACE("Looks like returning a valid worksheet");
	}
	return xWrkSt;
}

uno::Any SAL_CALL
ScVbaWorkbook::Worksheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
        uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
       	uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();

	uno::Reference< vba::XWorksheets > xWorkSheets( new ScVbaWorksheets(m_xContext,
		mxModel) );
	if (  aIndex.getValueTypeClass() == uno::TypeClass_VOID )
	{
		return uno::Any( xWorkSheets );	
	}
	return uno::Any( xWorkSheets->Item( aIndex ) );
}

void
ScVbaWorkbook::Close( const uno::Any &rSaveArg, const uno::Any &rFileArg,
					  const uno::Any &rRouteArg ) throw (uno::RuntimeException)
{
	sal_Bool bSaveChanges = sal_False;
	rtl::OUString aFileName( RTL_CONSTASCII_USTRINGPARAM( "" ) );
	sal_Bool bRouteWorkbook = sal_True;

	rSaveArg >>= bSaveChanges;
	rFileArg >>= aFileName;
	rRouteArg >>= bRouteWorkbook;

	uno::Reference< util::XCloseable > xCloseable( mxModel, uno::UNO_QUERY_THROW );
	SC_VBA_FIXME(("Workbook::Close ignores 3 arguments"));
	xCloseable->close( FALSE );
}

void
ScVbaWorkbook::Protect() throw (uno::RuntimeException)
{
	uno::Reference< util::XProtectable > xProt( mxModel, uno::UNO_QUERY_THROW );
	SC_VBA_FIXME(("Workbook::Protect stub"));
	xProt->protect( rtl::OUString() );
}

::sal_Bool
ScVbaWorkbook::getProtectStructure() throw (uno::RuntimeException)
{
	uno::Reference< util::XProtectable > xProt( mxModel, uno::UNO_QUERY_THROW );
	return xProt->isProtected();
}
/*
uno::Reference< vba::XWorksheet >
ScVbaWorkbook::Worksheets( const ::uno::Any &rSheet ) throw (uno::RuntimeException)
{
	rtl::OUString aStringSheet;
	rSheet >>= aStringSheet;

        uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY );
        if ( xSpreadDoc.is() )
        {
        	uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		if (xSheets.is())
		{
			uno::Reference <container::XNameAccess> xName( xSheets, uno::UNO_QUERY );
			if ( xName.is() )
			{
				uno::Reference< sheet::XSpreadsheet > xSheet(xName->getByName(aStringSheet), uno::UNO_QUERY);
				return uno::Reference< vba::XWorksheet >( new ScVbaWorksheet(xSheet));
			}
		}
        }

}
*/
