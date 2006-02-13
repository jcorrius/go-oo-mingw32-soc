

#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>
#include<org/openoffice/vba/Excel/XlCalculation.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"
#include "vbawsfunction.hxx"
#include "vbadialogs.hxx"
#include "vbawindow.hxx"
#include "vbawindows.hxx"

#include "tabvwsh.hxx"




using namespace ::org::openoffice;
using namespace ::com::sun::star;

class ActiveWorkbook : public ScVbaWorkbook
{
protected:
	virtual uno::Reference< frame::XModel > getModel()
	{ 	
		return getCurrentDocument(); 
	}
public:
	ActiveWorkbook( uno::Reference< uno::XComponentContext >& xContext) : ScVbaWorkbook(  xContext ){}
};

ScVbaApplication::ScVbaApplication( uno::Reference<uno::XComponentContext >& xContext ): m_xContext( xContext ), m_xCalculation( vba::Excel::XlCalculation::xlCalculationAutomatic )
{
}

ScVbaApplication::~ScVbaApplication()
{
}


uno::Reference< vba::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
	return new ActiveWorkbook( m_xContext ); 
}
uno::Reference< oo::vba::XWorkbook > SAL_CALL 
ScVbaApplication::getThisWorkbook() throw (uno::RuntimeException)
{
	return getActiveWorkbook();
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
	ScTabViewShell* pViewShell = getCurrentBestViewShell();
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
	uno::Reference< vba::XCollection > xWorkBooks( new ScVbaWorkbooks( m_xContext ) );
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
	uno::Any aVoid;
	if ( isRangeShortCut( Name ) )
		return uno::Any( getActiveWorkbook()->getActiveSheet()->Range( uno::Any( Name ), aVoid ) );	
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

uno::Reference< vba::XWindow > SAL_CALL 
ScVbaApplication::getActiveWindow() throw (uno::RuntimeException)
{
	return new ScVbaWindow( m_xContext, getCurrentDocument() );
}

uno::Any SAL_CALL 
ScVbaApplication::getCutCopyMode() throw (uno::RuntimeException)
{
	//# FIXME TODO, implementation
	uno::Any result;
	result <<= sal_False;
	return result;
}
void SAL_CALL 
ScVbaApplication::setCutCopyMode( const uno::Any& _cutcopymode ) throw (uno::RuntimeException)
{
	//# FIXME TODO, implementation
}
double SAL_CALL 
ScVbaApplication::CountA( const uno::Any& arg1 ) throw (uno::RuntimeException)
{
	double result;
	uno::Reference< vba::XWorksheetFunction > xWksFn = WorksheetFunction();
	uno::Reference< script::XInvocation > xInvoc( xWksFn, uno::UNO_QUERY_THROW );
	if  ( xInvoc.is() )
	{
		static rtl::OUString FunctionName( RTL_CONSTASCII_USTRINGPARAM("CountA" ) );
		uno::Sequence< uno::Any > Params(1);
		Params[0] = arg1;
		uno::Sequence< sal_Int16 > OutParamIndex;
		uno::Sequence< uno::Any > OutParam;
		xInvoc->invoke( FunctionName, Params, OutParamIndex, OutParam ) >>= result;
	}
	return result;
}

::sal_Int32 SAL_CALL 
ScVbaApplication::getCalculation() throw (css::uno::RuntimeException)
{
	return m_xCalculation;
}

void SAL_CALL 
ScVbaApplication::setCalculation( ::sal_Int32 _calculation ) throw (css::uno::RuntimeException)
{
	m_xCalculation = _calculation;
}

uno::Any SAL_CALL 
ScVbaApplication::Windows( const uno::Any& aIndex  ) throw (uno::RuntimeException)
{
	uno::Reference< vba::XCollection >  xWindows = ScVbaWindows::Windows( m_xContext );
	if ( aIndex.getValueTypeClass() == uno::TypeClass_VOID )
		return uno::Any( xWindows );
	return uno::Any( xWindows->Item( aIndex ) );	
}


