#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "star-frame-widget.h"

#define UNO_BOOTSTRAP_INI "file://" INIFILE
#define FILENAME "file:///demo/schmidt.sxw"

using namespace com::sun::star;
using namespace com::sun::star::beans;

using rtl::OUString;
using com::sun::star::uno::Reference;

static char *
gimme_utf8_please (const rtl::OUString &oustring)
{
	rtl::OString ostring;

	ostring = ::rtl::OUStringToOString (oustring, RTL_TEXTENCODING_UTF8);
        return g_strdup (ostring.pData->buffer);
}

static void
destroy( GtkWidget *widget, gpointer data )
{
    gtk_main_quit();
}

static void
FrameLoaderLoadFileFromUrl( Reference< frame::XSynchronousFrameLoader > xFrameLoader,
							Reference< frame::XFrame > xFrame,
							OUString sUrl,
							OUString sTypeName)
{
	uno::Sequence< beans::PropertyValue > aProperties( 3 );

	aProperties[ 0 ] = PropertyValue( OUString::createFromAscii("FileName"),
									  0,
									  uno::makeAny( sUrl ),
									  PropertyState_DIRECT_VALUE ); 
	
	aProperties[ 1 ] = PropertyValue( OUString::createFromAscii( "TypeName" ),
									  0,
									  uno::makeAny( sTypeName ),
									  PropertyState_DIRECT_VALUE ); 
	
	aProperties[ 2 ] = PropertyValue( OUString::createFromAscii( "ReadOnly" ),
									  0,
									  uno::makeAny( sal_True ),
									  PropertyState_DIRECT_VALUE ); 

	xFrameLoader->load( aProperties, xFrame );
}

int
main( int argc, char *argv[] )
{
    gtk_init( &argc, &argv );

    Reference< uno::XComponentContext > xComponentContext =
		::cppu::defaultBootstrap_InitialComponentContext(
			OUString::createFromAscii( UNO_BOOTSTRAP_INI ) );
    g_assert( xComponentContext.is() );

    Reference< lang::XMultiComponentFactory > xMultiComponentFactoryClient(
		xComponentContext->getServiceManager() );
    g_assert( xMultiComponentFactoryClient.is() );

    Reference< uno::XInterface > xInterface =
		xMultiComponentFactoryClient->createInstanceWithContext(
			OUString::createFromAscii( "com.sun.star.bridge.UnoUrlResolver" ),
			xComponentContext );
    g_assert( xInterface.is() );

    Reference< bridge::XUnoUrlResolver > xUnoUrlResolver(
		xInterface, uno::UNO_QUERY );
    g_assert( xUnoUrlResolver.is() );

    OUString sConnectionString = OUString::createFromAscii(
		"uno:pipe,name=martin_ooo_bonobo;"
		"urp;StarOffice.ServiceManager" );

    xInterface = Reference< uno::XInterface >(
		xUnoUrlResolver->resolve( sConnectionString ), uno::UNO_QUERY );
    g_assert( xInterface.is() );

    Reference< beans::XPropertySet > xPropSet( xInterface, uno::UNO_QUERY );
    xPropSet->getPropertyValue( OUString::createFromAscii( "DefaultContext") )
		>>= xComponentContext;

    Reference< lang::XMultiServiceFactory > xMultiServiceFactory(
		xComponentContext->getServiceManager(), uno::UNO_QUERY );
    g_assert (xMultiServiceFactory.is() );

    GtkWidget *pWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    g_signal_connect (G_OBJECT (pWindow), "destroy",
					  G_CALLBACK (destroy), NULL);

    GtkWidget *pVBox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( pWindow ), pVBox );

	GtkWidget *pText = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( pVBox ), pText, FALSE, FALSE, 5 );

	GtkWidget *pText2 = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( pVBox ), pText2, FALSE, FALSE, 5 );

	GtkWidget *pSocket = star_frame_widget_new( xMultiServiceFactory );
 	gtk_box_pack_start( GTK_BOX( pVBox ), pSocket, TRUE, TRUE, 5 );

	Reference< awt::XWindowPeer > xWindowPeer(
		star_frame_widget_get_window_peer( STAR_FRAME_WIDGET( pSocket ) ) );
	g_assert( xWindowPeer.is() );

    Reference< frame::XFrame > xFrame(
		star_frame_widget_get_frame( STAR_FRAME_WIDGET( pSocket ) ) );
	g_assert( xFrame.is() );

	// Loading
	Reference< document::XTypeDetection > xTypeDetection(
		xMultiServiceFactory->createInstance(
			OUString::createFromAscii( "com.sun.star.document.TypeDetection" ) ),
		uno::UNO_QUERY );
	g_assert( xTypeDetection.is() );

	Reference< XMultiServiceFactory > xFrameLoaderFactory(
		xMultiServiceFactory->createInstance(
			OUString::createFromAscii( "com.sun.star.frame.FrameLoaderFactory" ) ),
		uno::UNO_QUERY );
	g_assert( xFrameLoaderFactory.is() );

	OUString sTypeName( xTypeDetection->queryTypeByURL( OUString::createFromAscii( FILENAME ) ) );

	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance(sTypeName),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );
		
	FrameLoaderLoadFileFromUrl(
		xFrameLoader, xFrame,
		OUString::createFromAscii( FILENAME ),
		sTypeName );

	gtk_widget_grab_focus( pSocket );
	gtk_widget_show( pSocket );
	gtk_widget_show( pText );
	gtk_widget_show( pText2 );
	gtk_widget_show( pVBox );
	gtk_widget_show( pWindow );

	gtk_main();
	return 0;
}
