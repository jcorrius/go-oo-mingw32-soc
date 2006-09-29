#include <stdio.h>


#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>
#include<org/openoffice/vba/Excel/XlCalculation.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <org/openoffice/vba/Excel/XlMousePointer.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"
#include "vbawsfunction.hxx"
#include "vbadialogs.hxx"
#include "vbawindow.hxx"
#include "vbawindows.hxx"
#include "vbaglobals.hxx"
#include "tabvwsh.hxx"

//start test includes
#include <sfx2/objsh.hxx>
#include <sfx2/app.hxx>

#include <toolkit/awt/vclxwindow.hxx>
#include <toolkit/helper/vclunohelper.hxx>

#include <docuno.hxx>

#include <basic/sbx.hxx>
#include <basic/sbstar.hxx>
#include <basic/sbuno.hxx>
#include <basic/sbmeth.hxx>
//end test includes

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
	if ( !xRange.is() )
	{
		uno::Reference< sheet::XSheetCellRangeContainer > xRanges( getCurrentDocument()->getCurrentSelection(), ::uno::UNO_QUERY);
		if ( xRanges.is() )
			return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRanges ) );

	}
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

sal_Bool
ScVbaApplication::getDisplayStatusBar() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XFrame > xFrame( xModel->getCurrentController()->getFrame(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xFrame, uno::UNO_QUERY_THROW );

    if( xProps.is() ){
        uno::Reference< frame::XLayoutManager > xLayoutManager( xProps->getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("LayoutManager")) ), uno::UNO_QUERY_THROW );
        rtl::OUString url(RTL_CONSTASCII_USTRINGPARAM( "private:resource/statusbar/statusbar" ));
        if( xLayoutManager.is() && xLayoutManager->isElementVisible( url ) ){
            return sal_True;
        }
    }
    return sal_False;
}

void
ScVbaApplication::setDisplayStatusBar(sal_Bool bDisplayStatusBar) throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XFrame > xFrame( xModel->getCurrentController()->getFrame(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xFrame, uno::UNO_QUERY_THROW );

    if( xProps.is() ){
        uno::Reference< frame::XLayoutManager > xLayoutManager( xProps->getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("LayoutManager")) ), uno::UNO_QUERY_THROW );
        rtl::OUString url(RTL_CONSTASCII_USTRINGPARAM( "private:resource/statusbar/statusbar" ));
        if( xLayoutManager.is() ){
            if( bDisplayStatusBar && !xLayoutManager->isElementVisible( url ) ){
                if( !xLayoutManager->showElement( url ) )
                    xLayoutManager->createElement( url );
                return;
            }
            else if( !bDisplayStatusBar && xLayoutManager->isElementVisible( url ) ){
                xLayoutManager->hideElement( url ); 
                return;
            }
        }
    }
    return;
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

uno::Any SAL_CALL
ScVbaApplication::Worksheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
    uno::Reference< vba::XWorkbook > xWorkbook( getActiveWorkbook(), uno::UNO_QUERY );
        uno::Any result;
    if ( xWorkbook.is() )
        result  = xWorkbook->Worksheets( aIndex );

    else
        // Fixme - check if this is reasonable/desired behavior
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
            "No ActiveWorkBook available" ), uno::Reference< uno::XInterface >() );

	return result;
}

uno::Any SAL_CALL
ScVbaApplication::WorksheetFunction( ) throw (::com::sun::star::uno::RuntimeException)
{
        return uno::makeAny( uno::Reference< script::XInvocation >( new ScVbaWSFunction(m_xContext) ) );
}

uno::Any SAL_CALL 
ScVbaApplication::Evaluate( const ::rtl::OUString& Name ) throw (uno::RuntimeException)
{
	// #TODO Evaluate allows other things to be evaluated, e.g. functions
	// I think ( like SIN(3) etc. ) need to investigate that
	// named Ranges also? e.g. [MyRange] if so need a list of named ranges
	uno::Any aVoid;
	return uno::Any( getActiveWorkbook()->getActiveSheet()->Range( uno::Any( Name ), aVoid ) );	
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

uno::Any SAL_CALL
ScVbaApplication::getStatusBar() throw (uno::RuntimeException)
{
	uno::Any result;
    if( getDisplayStatusBar() )
	    result <<= sal_False;
    else
        result <<= sal_True;
	return result;
}

void SAL_CALL 
ScVbaApplication::setStatusBar( const uno::Any& _statusbar ) throw (uno::RuntimeException)
{
    rtl::OUString sText;
    sal_Bool bDefault;
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< task::XStatusIndicatorSupplier > xStatusIndicatorSupplier( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    uno::Reference< task::XStatusIndicator > xStatusIndicator( xStatusIndicatorSupplier->getStatusIndicator(), uno::UNO_QUERY_THROW );
    if( _statusbar >>= sText )
    {
        setDisplayStatusBar( sal_True );
        //xStatusIndicator->start( sText, 100 );
        xStatusIndicator->setText( sText );
    }
    else if( _statusbar >>= bDefault )
    {
        if( bDefault == sal_False )
        {
            xStatusIndicator->end();
            setDisplayStatusBar( sal_True );
        }
    }
    else
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "Invalid prarameter. It should be a string or False" ),
            uno::Reference< uno::XInterface >() );
}

double SAL_CALL 
ScVbaApplication::CountA( const uno::Any& arg1 ) throw (uno::RuntimeException)
{
	double result;
	uno::Reference< script::XInvocation > xInvoc( WorksheetFunction(), uno::UNO_QUERY_THROW );
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
void SAL_CALL 
ScVbaApplication::wait( double time ) throw (css::uno::RuntimeException)
{
	StarBASIC* pBasic = SFX_APP()->GetBasic();
	SFX_APP()->EnterBasicCall();
	SbxArrayRef aArgs = new SbxArray;
	SbxVariableRef aRef = new SbxVariable;
	aRef->PutDouble( time );
	aArgs->Put(  aRef, 1 );
	SbMethod* pMeth = (SbMethod*)pBasic->GetRtl()->Find( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Wait") ), SbxCLASS_METHOD );
	
	if ( pMeth )
	{
		pMeth->SetParameters( aArgs );
		SbxVariableRef refTemp = pMeth;
		// forces a broadcast
		SbxVariableRef pNew = new  SbxMethod( *((SbxMethod*)pMeth));
	}
	SFX_APP()->LeaveBasicCall();

}

uno::Any SAL_CALL 
ScVbaApplication::Range( const uno::Any& Cell1, const uno::Any& Cell2 ) throw (uno::RuntimeException)
{
	// Althought the documentation seems clear that Range without a 
	// qualifier then its a shortcut for ActiveSheet.Range
	// however, similarly Application.Range is apparently also a 
	// shortcut for ActiveSheet.Range
	// The is however a subtle behavioural difference I've come across 
	// wrt to named ranges.
	// If a named range "test" exists { Sheet1!$A1 } and the active sheet
	// is Sheet2 then the following will fail
	// msgbox ActiveSheet.Range("test").Address ' failes
	// msgbox WorkSheets("Sheet2").Range("test").Address
	// but !!!
	// msgbox Range("test").Address ' works
	// msgbox Application.Range("test").Address ' works

	// Single param Range 
	rtl::OUString sRangeName;
	Cell1 >>= sRangeName;
	if ( Cell1.hasValue() && !Cell2.hasValue() && sRangeName.getLength() )
	{
		const static rtl::OUString sNamedRanges( RTL_CONSTASCII_USTRINGPARAM("NamedRanges"));
		uno::Reference< beans::XPropertySet > xPropSet( getCurrentDocument(), uno::UNO_QUERY_THROW );
		
		uno::Reference< container::XNameAccess > xNamed( xPropSet->getPropertyValue( sNamedRanges ), uno::UNO_QUERY_THROW );
		uno::Reference< sheet::XCellRangeReferrer > xReferrer;
		try
		{
			xReferrer.set ( xNamed->getByName( sRangeName ), uno::UNO_QUERY );
		}
		catch( uno::Exception& e )
		{
			// do nothing
		}
		if ( xReferrer.is() )
		{
			uno::Reference< table::XCellRange > xRange = xReferrer->getReferredCells();
			if ( xRange.is() )
			{
				uno::Reference< vba::XRange > xVbRange =  new  ScVbaRange( m_xContext, xRange );
				return uno::makeAny( xVbRange );
			}
		}
	}

	return uno::makeAny( getActiveSheet()->Range( Cell1, Cell2 ) ); 
}

uno::Reference< vba::XWorksheet > SAL_CALL 
ScVbaApplication::getActiveSheet() throw (uno::RuntimeException)
{
    uno::Reference< vba::XWorksheet > result;
    uno::Reference< vba::XWorkbook > xWorkbook( getActiveWorkbook(), uno::UNO_QUERY );
    if ( xWorkbook.is() )
    {
        uno::Reference< vba::XWorksheet > xWorksheet( 
            xWorkbook->getActiveSheet(), uno::UNO_QUERY );
        if ( xWorksheet.is() )
        {
            result = xWorksheet;
        }	
    }

    if ( !result.is() )
    {
        // Fixme - check if this is reasonable/desired behavior
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
            "No activeSheet available" ), uno::Reference< uno::XInterface >() );
    }
    return result;
	
}

/*******************************************************************************
 *  In msdn: 
 *  Reference   Optional Variant. The destination. Can be a Range 
 *  object, a string that contains a cell reference in R1C1-style notation,
 *  or a string that contains a Visual Basic procedure name.
 *  Scroll   Optional Variant. True to scrol, False to not scroll through 
 *  the window. The default is False.
 *  Parser is split to three parts, Range, R1C1 string and procedure name.
 *  by test excel, it seems Scroll no effect. ??? 
*******************************************************************************/
void SAL_CALL 
ScVbaApplication::GoTo( const uno::Any& Reference, const uno::Any& Scroll ) throw (uno::RuntimeException)
{
    //test Scroll is a boolean
    sal_Bool bScroll;
    if(!( Scroll >>= bScroll ))
    {
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "sencond parameter should be boolean" ),
                    uno::Reference< uno::XInterface >() );
    }
    //R1C1-style string or a string of procedure name.
    rtl::OUString sRangeName;
    if( Reference >>= sRangeName )
    {
        //TODO
        //R1C1-style
        //procedure name
        printf("\nGoTo excute string\n");
        return;
    }
    uno::Reference< vba::XRange > xRange;
    if( Reference >>= xRange )
    {
        uno::Reference< vba::XRange > xVbaRange( Reference, uno::UNO_QUERY );
        if ( xVbaRange.is() )
        {
            //TODO bScroll should be using, In this time, it doesenot have effection
            if( bScroll )
                xVbaRange->Select();
            else
                xVbaRange->Activate();
        }
        printf("\nGoTo excute range\n");
        return;
    }
    throw uno::RuntimeException( rtl::OUString::createFromAscii( "invalid reference or name" ),
            uno::Reference< uno::XInterface >() );
}

sal_Int32 SAL_CALL
ScVbaApplication::getCursor() throw (uno::RuntimeException)
{
    SfxViewShell* pView = SfxViewShell::Current();
    if( pView ){
        //printf("\nget Cursor...%d\n", pView->GetWindow()->GetSystemWindow()->GetPointer().GetStyle());
        //return pView->GetWindow()->GetPointer().GetStyle();
        return pView->GetWindow()->GetSystemWindow()->GetPointer().GetStyle();
    }
    return vba::Excel::XlMousePointer::xlDefault;
}

void SAL_CALL 
ScVbaApplication::setCursor( sal_Int32 _cursor ) throw (uno::RuntimeException)
{
    SfxViewShell* pView;// = SfxViewShell::Current();
    pView = SfxViewShell::GetFirst();
    switch( _cursor )
    {
        case vba::Excel::XlMousePointer::xlNorthwestArrow:
        case vba::Excel::XlMousePointer::xlWait:
        case vba::Excel::XlMousePointer::xlIBeam:
            while( pView ){
                const Pointer& rPointer( _cursor );
                //It will set the edit window, toobar and statusbar's mouse pointer.
                pView->GetWindow()->GetSystemWindow()->SetPointer( rPointer );
                pView->GetWindow()->GetSystemWindow()->EnableChildPointerOverwrite( sal_True );
                //It only set the edit window's mouse pointer
                //pView->GetWindow()->SetPointer( rPointer );
                //pView->GetWindow()->EnableChildPointerOverwrite( sal_True );
                //printf("\nset Cursor...%d\n", pView->GetWindow()->GetType()/*GetPointer().GetStyle()*/);
                pView = SfxViewShell::GetNext( *pView );
            }
            break;
        case vba::Excel::XlMousePointer::xlDefault:
            while( pView ){
                const Pointer& rPointer( 0 );
                pView->GetWindow()->GetSystemWindow()->SetPointer( rPointer );
                pView->GetWindow()->GetSystemWindow()->EnableChildPointerOverwrite( sal_False );
                pView = SfxViewShell::GetNext( *pView );
            }
            break;
        default:
            throw uno::RuntimeException( rtl::OUString( 
                RTL_CONSTASCII_USTRINGPARAM("Unknown value for Cursor pointer")), uno::Reference< uno::XInterface >() );
    }
}
