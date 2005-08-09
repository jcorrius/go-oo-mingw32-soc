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
	uno::Reference< frame::XModel > xModel( m_xDesktop->getCurrentComponent(), uno::UNO_QUERY );

	if( xModel.is() )
	{
		uno::Reference< vba::XWorkbook > xWrkBk( new ScVbaWorkbook( m_xContext,
			xModel )); 
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

uno::Reference< vba::XRange >
ScVbaApplication::getActiveCell() throw (uno::RuntimeException )
{
    uno::Reference< frame::XModel > xModel( m_xDesktop->getCurrentComponent(), uno::UNO_QUERY );
                                                                                                                             
    uno::Reference< table::XCellRange > xRange( xModel->getCurrentSelection(), ::uno::UNO_QUERY);
                                                                                                                             
    return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRange ) );
}

uno::Any SAL_CALL
ScVbaApplication::Workbooks( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	uno::Reference< vba::XWorkbooks > xWorkBooks( new ScVbaWorkbooks( m_xContext ) );
	if (  aIndex.getValueTypeClass() == uno::TypeClass_VOID )
	{
		// void then somebody did Workbooks.something in vba
	    return uno::Any( xWorkBooks );
	}

	return uno::Any ( xWorkBooks->Item( aIndex ) );
}

