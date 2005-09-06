#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include "vbahelper.hxx"
#include "tabvwsh.hxx"

void
org::openoffice::dispatchRequests (uno::Reference< frame::XModel>& xModel,rtl::OUString & aUrl) 
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
	if (xDispatcher.is())
		xDispatcher->dispatch( url,uno::Sequence< beans::PropertyValue >(0) );
}


void
org::openoffice::implnPaste()
{
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	pViewShell->PasteFromSystem();
	pViewShell->CellContentChanged();
}

void
org::openoffice::implnCopy()
{
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	pViewShell->CopyToClip(NULL,false,false,true);
}

void 
org::openoffice::implnCut()
{
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	pViewShell->CutToClip( NULL, TRUE );
}
