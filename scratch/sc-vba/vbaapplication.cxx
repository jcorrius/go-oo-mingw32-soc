

#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"
#include "vbawsfunction.hxx"
#include "vbadialogs.hxx"

#include "tabvwsh.hxx"




using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaApplication::ScVbaApplication( uno::Reference<uno::XComponentContext >& xContext ): m_xContext( xContext )
{
}

ScVbaApplication::~ScVbaApplication()
{
}


uno::Reference< vba::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );

	uno::Reference< vba::XWorkbook > xWrkBk( new ScVbaWorkbook( m_xContext,
		xModel )); 
	return xWrkBk; 
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
	if ( !pViewShell )
		throw uno::RuntimeException( rtl::OUString::createFromAscii("No ViewShell available"), uno::Reference< uno::XInterface >() );
	ScViewData* pTabView = pViewShell->GetViewData();
	if ( !pTabView )
		throw uno::RuntimeException( rtl::OUString::createFromAscii("No ViewData available"), uno::Reference< uno::XInterface >() );

	sal_Int32 nCursorX = pTabView->GetCurX();
	sal_Int32 nCursorY = pTabView->GetCurY();

	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRange->getCellRangeByPosition( nCursorX, nCursorY, 
										nCursorX, nCursorY ) ) ); 
}

sal_Bool
ScVbaApplication::getScreenUpdating() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
	return !xModel->hasControllersLocked();
}

void
ScVbaApplication::setScreenUpdating(sal_Bool bUpdate) throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
	if (bUpdate)
		xModel->unlockControllers();
	else
		xModel->lockControllers();
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

uno::Reference< vba::XWorksheetFunction > SAL_CALL
ScVbaApplication::WorksheetFunction( ) throw (::com::sun::star::uno::RuntimeException)
{
        return new ScVbaWSFunction(m_xContext);
}

uno::Any SAL_CALL 
ScVbaApplication::Evaluate( const ::rtl::OUString& Name ) throw (uno::RuntimeException)
{
	// #TODO Evaluate allows other things to be evaluated, e.g. functions
	// I think ( like SIN(3) etc. ) need to investigate that
	// named Ranges also? e.g. [MyRange] if so need a list of named ranges
	if ( isRangeShortCut( Name ) )
		return uno::Any( getActiveWorkbook()->getActiveSheet()->Range( uno::Any( Name ) ) );	
	return uno::Any();
}

uno::Any 
ScVbaApplication::Dialogs( const uno::Any &aIndex ) throw (uno::RuntimeException)
{
	uno::Reference< vba::XDialogs > xDialogs( new ScVbaDialogs( m_xContext ) );
	if( !aIndex.hasValue() )
		return uno::Any( xDialogs );
	return uno::Any( xDialogs->Item( aIndex ) );
}
	
