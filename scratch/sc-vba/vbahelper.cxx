#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <sfx2/objsh.hxx>
#include <sfx2/app.hxx>

#include <docuno.hxx>

#include <basic/sbx.hxx>

#include "vbahelper.hxx"
#include "tabvwsh.hxx"
#include "transobj.hxx"

using namespace ::com::sun::star;
using namespace ::org::openoffice;

void unoToSbxValue( SbxVariable* pVar, const uno::Any& aValue );

uno::Any sbxToUnoValue( SbxVariable* pVar );

const ::rtl::OUString REPLACE_CELLS_WARNING(  RTL_CONSTASCII_USTRINGPARAM( "ReplaceCellsWarning"));

class PasteCellsWarningReseter
{
private:
	bool bInitialWarningState;
	static uno::Reference< beans::XPropertySet > getGlobalSheetSettings() throw ( uno::RuntimeException )
	{
		static uno::Reference<uno::XComponentContext > xContext( ::cppu::defaultBootstrap_InitialComponentContext(), uno::UNO_QUERY_THROW );
		static uno::Reference<lang::XMultiComponentFactory > xServiceManager(
				xContext->getServiceManager(), uno::UNO_QUERY_THROW );
		static uno::Reference< beans::XPropertySet > xProps( xServiceManager->createInstanceWithContext( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sheet.GlobalSheetSettings" ) ) ,xContext ), uno::UNO_QUERY_THROW );
		return xProps;
	}	

	bool getReplaceCellsWarning() throw ( uno::RuntimeException )
	{
		sal_Bool res = sal_False;
		getGlobalSheetSettings()->getPropertyValue( REPLACE_CELLS_WARNING ) >>= res;
		return ( res == sal_True );
	}

	void setReplaceCellsWarning( bool bState ) throw ( uno::RuntimeException )
	{
		getGlobalSheetSettings()->setPropertyValue( REPLACE_CELLS_WARNING, uno::makeAny( bState ) );
	}
public:
	PasteCellsWarningReseter() throw ( uno::RuntimeException )
	{
		if ( bInitialWarningState = getReplaceCellsWarning() );
			setReplaceCellsWarning( false );
	}
	~PasteCellsWarningReseter()
	{
		if ( bInitialWarningState )
		{
			// don't allow dtor to throw
			try
			{
				setReplaceCellsWarning( true ); 
			}
			catch ( uno::Exception& e ){}
		}
	}
};
void
org::openoffice::dispatchRequests (uno::Reference< frame::XModel>& xModel,rtl::OUString & aUrl, uno::Sequence< beans::PropertyValue >& sProps ) 
{

	util::URL  url ;
	url.Complete = aUrl;
	rtl::OUString emptyString = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "" ));
	uno::Reference<frame::XController> xController = xModel->getCurrentController();
	uno::Reference<frame::XFrame> xFrame = xController->getFrame();
	uno::Reference<frame::XDispatchProvider> xDispatchProvider (xFrame,uno::UNO_QUERY_THROW);
	try
	{
		uno::Reference<uno::XComponentContext > xContext(  ::cppu::defaultBootstrap_InitialComponentContext());
		if ( !xContext.is() )
		{
			return ;
		}

		uno::Reference<lang::XMultiComponentFactory > xServiceManager(
				xContext->getServiceManager() );
		if ( !xServiceManager.is() )
		{
			return ;
		}
		uno::Reference<util::XURLTransformer> xParser( xServiceManager->createInstanceWithContext(     rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.util.URLTransformer" ) )
			,xContext), uno::UNO_QUERY_THROW );
		if (!xParser.is())
			return;
		xParser->parseStrict (url);
	}
	catch ( ::cppu::BootstrapException & e )
	{
		return ;
	}
	catch ( uno::Exception & e )
	{
		return ;
	}

	uno::Reference<frame::XDispatch> xDispatcher = xDispatchProvider->queryDispatch(url,emptyString,0);

	uno::Sequence<beans::PropertyValue> dispatchProps(1);

	sal_Int32 nProps = sProps.getLength();
	beans::PropertyValue* pDest = dispatchProps.getArray();
	if ( nProps )
	{
		dispatchProps.realloc( nProps + 1 );
		// need to reaccquire pDest after realloc
		pDest = dispatchProps.getArray();
		beans::PropertyValue* pSrc = sProps.getArray();
		for ( sal_Int32 index=0; index<nProps; ++index, ++pSrc, ++pDest )
			*pDest = *pSrc;
	}

	(*pDest).Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Silent" ));
	(*pDest).Value <<= (sal_Bool)sal_True;

	if (xDispatcher.is())
		xDispatcher->dispatch( url, dispatchProps );
}

void
org::openoffice::dispatchRequests (uno::Reference< frame::XModel>& xModel,rtl::OUString & aUrl) 
{
	uno::Sequence<beans::PropertyValue> dispatchProps;
	dispatchRequests( xModel, aUrl, dispatchProps );
}


void
org::openoffice::implnPaste()
{
	PasteCellsWarningReseter resetWarningBox;
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	if ( pViewShell )
	{
		pViewShell->PasteFromSystem();
		pViewShell->CellContentChanged();
	}
}


void
org::openoffice::implnCopy()
{
	ScTabViewShell* pViewShell = getCurrentBestViewShell();
	if ( pViewShell )
		pViewShell->CopyToClip(NULL,false,false,true);
}

void 
org::openoffice::implnCut()
{
	ScTabViewShell* pViewShell =  getCurrentBestViewShell();
	if ( pViewShell )
		pViewShell->CutToClip( NULL, TRUE );
}

void org::openoffice::implnPasteSpecial(USHORT nFlags,USHORT nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose)
{
	PasteCellsWarningReseter resetWarningBox;
	sal_Bool bAsLink(sal_False), bOtherDoc(sal_False);
	InsCellCmd eMoveMode = INS_NONE;

	ScTabViewShell* pTabViewShell = ScTabViewShell::GetActiveViewShell();
	if ( !pTabViewShell ) 
		// none active, try next best
		pTabViewShell = getCurrentBestViewShell();
	if ( pTabViewShell )
	{
		ScViewData* pView = pTabViewShell->GetViewData();	
		Window* pWin = NULL;
		if ( pView && ( pWin = pView->GetActiveWin() ) )
		{
			if ( bAsLink && bOtherDoc )
				pTabViewShell->PasteFromSystem(0);//SOT_FORMATSTR_ID_LINK
			else 
			{
				ScTransferObj* pOwnClip = ScTransferObj::GetOwnClipboard( pWin );
				ScDocument* pDoc = NULL; 
				if ( pOwnClip )
					pDoc = pOwnClip->GetDocument();	
				pTabViewShell->PasteFromClip( nFlags, pDoc,
					nFunction, bSkipEmpty, bTranspose, bAsLink,
					eMoveMode, IDF_NONE, TRUE );
				pTabViewShell->CellContentChanged();
			}
		}
	}

}

bool
org::openoffice::isRangeShortCut( const ::rtl::OUString& sParam )
{
        // for a ShortCutRange param, I'd expect the first letter to be
        //[A-Z] and the last letter to be a digit 0-9 e.g A10, [A1:A10] etc.
        ::rtl::OString sStr = rtl::OUStringToOString( sParam,
                RTL_TEXTENCODING_UTF8 );
        const sal_Char* pFirst = (const sal_Char*)sStr;
        const sal_Char* pLast = pFirst + ( sStr.getLength() - 1 );
        if ( (( *pFirst >= 'A' && *pFirst <= 'Z' ) || ( *pFirst >= 'a' && *pFirst <= 'Z' ) )  )
                if ( ( *pLast >= '0' ) && ( *pLast <= '9' ) )
                        return true;

        return false;
}

 uno::Reference< frame::XModel > 
org::openoffice::getCurrentDocument() throw (uno::RuntimeException)
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

ScDocShell* 
org::openoffice::getDocShell( css::uno::Reference< css::frame::XModel>& xModel ) 
{
	ScModelObj* pModel = static_cast< ScModelObj* >( xModel.get() );
	ScDocShell* pDocShell = NULL;
	if ( pModel )
		pDocShell = (ScDocShell*)pModel->GetEmbeddedObject();
	return pDocShell;

}

ScTabViewShell* 
org::openoffice::getBestViewShell(  css::uno::Reference< css::frame::XModel>& xModel )
{
	ScDocShell* pDocShell = NULL;
	if ( ( pDocShell = getDocShell( xModel ) ) )
	{
		return pDocShell->GetBestViewShell();
	}
	return NULL;
}

ScTabViewShell* 
org::openoffice::getCurrentBestViewShell()
{ 
	uno::Reference< frame::XModel > xModel = getCurrentDocument();
	return getBestViewShell( xModel );
}

SfxViewFrame* 
org::openoffice::getCurrentViewFrame()
{
	ScTabViewShell* pViewShell = getCurrentBestViewShell();	
	if ( pViewShell )
		return pViewShell->GetViewFrame();
	return NULL;
}

