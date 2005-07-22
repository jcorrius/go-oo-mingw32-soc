#include <sfx2/objsh.hxx>

#include<com/sun/star/sheet/XCellRangeAddressable.hpp>
#include<com/sun/star/sheet/XCellAddressable.hpp>
#include<com/sun/star/table/XCell.hpp>
#include<com/sun/star/table/XCellRange.hpp>
#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>
#include<com/sun/star/sheet/XViewPane.hpp>
#include<com/sun/star/table/CellRangeAddress.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaApplication::ScVbaApplication( uno::Reference<uno::XComponentContext >& xContext ): m_xContext( xContext )
{
	uno::Reference< lang::XMultiComponentFactory > xSMgr( m_xContext->getServiceManager(), uno::UNO_QUERY );
	if ( !xSMgr.is() )
	{
		throw uno::RuntimeException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": Couldn't instantiate MultiComponent factory" ) ),
                    uno::Reference< XInterface >() );

	}
	m_xDesktop.set( 
		xSMgr->createInstanceWithContext( rtl::OUString::createFromAscii(
			"com.sun.star.frame.Desktop"), xContext ) , uno::UNO_QUERY );

	if ( !m_xDesktop.is() )
	{
		throw uno::RuntimeException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": Couldn't create Desktop component" ) ),
                    uno::Reference< XInterface >() );
	}
}

ScVbaApplication::~ScVbaApplication()
{
}


uno::Reference< vba::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
	OSL_TRACE("In ScVbaApplication::getActiveWorkbook() ");
	uno::Reference< frame::XModel > xModel( m_xDesktop->getCurrentComponent(), uno::UNO_QUERY );

	if( xModel.is() )
	{
		OSL_TRACE("ScVbaApplication::getActiveWorkbook() about to return new Workbook");
		/*return uno::Reference< vba::XWorkbook >(new ScVbaWorkbook( m_xContext,
			xModel ) ); */
		uno::Reference< vba::XWorkbook > xWrkBk( new ScVbaWorkbook( m_xContext,
			xModel )); 
		OSL_TRACE("Have constucted a Workbook object ");
		return xWrkBk; 
	}
	else
		return uno::Reference< vba::XWorkbook >( NULL );
}

uno::Reference< vba::XRange >
ScVbaApplication::getSelection() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( m_xDesktop->getCurrentComponent(), uno::UNO_QUERY );
                                                                                                                             
    uno::Reference< table::XCellRange > xRange( xModel->getCurrentSelection(), ::uno::UNO_QUERY);

	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRange ) );
}

uno::Reference< vba::XWorkbooks >
ScVbaApplication::getWorkbooks() throw (uno::RuntimeException)
{
	return uno::Reference< vba::XWorkbooks >( new ScVbaWorkbooks( m_xContext ) );
}

