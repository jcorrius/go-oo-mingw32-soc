#include <sfx2/objsh.hxx>
#include <sfx2/app.hxx>

#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"

#include "tabvwsh.hxx"

#include <basic/sbx.hxx>

void unoToSbxValue( SbxVariable* pVar, const uno::Any& aValue );
uno::Any sbxToUnoValue( SbxVariable* pVar );

using namespace ::org::openoffice;
using namespace ::com::sun::star;

inline uno::Reference< frame::XModel > 
ScVbaApplication::getCurrentDocument() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel;
	SbxObject* pBasic = reinterpret_cast< SbxObject* > ( SFX_APP()->GetBasic() );
	SbxObject* basicChosen =  pBasic ;
	if ( basicChosen == NULL)
	{
		OSL_TRACE("getModelFromBasic() StarBASIC* is NULL" );
		return xModel;
	}    
    SbxObject* p = pBasic;
    SbxObject* pParent = p->GetParent();
    SbxObject* pParentParent = pParent ? pParent->GetParent() : NULL;

    if( pParentParent )
    {
        basicChosen = pParentParent;
    }
    else if( pParent )
    {
        basicChosen = pParent;
    }


    uno::Any aModel; 
    SbxVariable *pCompVar = basicChosen->Find(  UniString(RTL_CONSTASCII_USTRINGPARAM("ThisComponent")), SbxCLASS_OBJECT );

	if ( pCompVar )
	{
		aModel = sbxToUnoValue( pCompVar );
		if ( sal_False == ( aModel >>= xModel ) ||
			!xModel.is() )
		{
			OSL_TRACE("Failed to extract model from thisComponent ");
			return xModel;
		}
		else
		{
			OSL_TRACE("Have model ThisComponent points to url %s",
			::rtl::OUStringToOString( xModel->getURL(),
				RTL_TEXTENCODING_ASCII_US ).pData->buffer );
		}
	}
	else
	{
		OSL_TRACE("Failed to get ThisComponent");
	}
	return xModel;
}
ScVbaApplication::ScVbaApplication( uno::Reference<uno::XComponentContext >& xContext ): m_xContext( xContext )
{
}

ScVbaApplication::~ScVbaApplication()
{
}


uno::Reference< vba::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY );

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

