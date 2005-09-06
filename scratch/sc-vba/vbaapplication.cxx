#include <sfx2/objsh.hxx>

#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"

#include "tabvwsh.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

inline uno::Reference< frame::XModel > 
ScVbaApplication::getCurrentDocument() throw (uno::RuntimeException)
{
	// This is lame - we should do better ... pwrt. the IDE ...
	return uno::Reference< frame::XModel >( m_xDesktop->getCurrentComponent(), uno::UNO_QUERY_THROW );
}

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
	uno::Reference< table::XCellRange > xRange( getCurrentDocument()->getCurrentSelection(), ::uno::UNO_QUERY);
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRange ) );
}

uno::Reference< vba::XRange >
ScVbaApplication::getActiveCell() throw (uno::RuntimeException )
{
	uno::Reference< sheet::XSpreadsheetView > xView( getCurrentDocument()->getCurrentController(), uno::UNO_QUERY_THROW );
	uno::Reference< table::XCellRange > xRange( xView->getActiveSheet(), ::uno::UNO_QUERY_THROW);
                                                                                                                             
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	sal_Int32 nCursorX, nCursorY;
	nCursorX = sal_Int32(pViewShell->GetViewData()->GetCurX()), nCursorY = sal_Int32(pViewShell->GetViewData()->GetCurY());
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRange->getCellRangeByPosition( nCursorX, nCursorY, 
										nCursorX, nCursorY ) ) ); 
}

sal_Bool
ScVbaApplication::getScreenUpdating() throw (uno::RuntimeException)
{
	return !getCurrentDocument()->hasControllersLocked();
}

void
ScVbaApplication::setScreenUpdating(sal_Bool bUpdate) throw (uno::RuntimeException)
{
	if (bUpdate)
		getCurrentDocument()->unlockControllers();
	else
		getCurrentDocument()->lockControllers();
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

