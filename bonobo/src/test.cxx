#include <stdio.h>
#include <unistd.h>

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

#include "string-macros.h"
#include "services.h"
#include "star-frame-widget.h"

#define UNO_BOOTSTRAP_INI DECLARE_ASCII( "file://" INIFILE )
#define FILENAME DECLARE_ASCII( "file:///demo/schmidt.sxw" )

using namespace com::sun::star;
using namespace com::sun::star::beans;
using namespace com::sun::star::bridge;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

using rtl::OUString;
using com::sun::star::uno::Reference;

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

	aProperties[ 0 ] = PropertyValue( DECLARE_ASCII( "FileName" ),
									  0,
									  uno::makeAny( sUrl ),
									  PropertyState_DIRECT_VALUE ); 
	
	aProperties[ 1 ] = PropertyValue( DECLARE_ASCII( "TypeName" ),
									  0,
									  uno::makeAny( sTypeName ),
									  PropertyState_DIRECT_VALUE ); 
	
	aProperties[ 2 ] = PropertyValue( DECLARE_ASCII( "ReadOnly" ),
									  0,
									  uno::makeAny( sal_True ),
									  PropertyState_DIRECT_VALUE ); 

	xFrameLoader->load( aProperties, xFrame );
}

static OUString
getPipeName()
{
	return 
		OUString::createFromAscii( g_get_user_name() ) +
		DECLARE_ASCII( "_ooo_bonobo" );
}

static Reference< XMultiComponentFactory >
getRemoteServiceManager( const Reference< XComponentContext >& xComponentContext )
{
	Reference< XMultiComponentFactory > xLocalSMgr(
		xComponentContext->getServiceManager() );

	Reference< XUnoUrlResolver > xUnoUrlResolver(
		xLocalSMgr->createInstanceWithContext( SERVICENAME_UNOURLRESOLVER,
											   xComponentContext ),
		UNO_QUERY );

	Reference< XInterface > xInterface;
	OUString sConnect( DECLARE_ASCII( "uno:pipe,name=" ) +
					   getPipeName() +
					   DECLARE_ASCII( ";urp;StarOffice.ServiceManager" ) );
	try {
		xInterface = xUnoUrlResolver->resolve( sConnect );
	} catch( uno::Exception ) {
		g_message( "Trying to start OOo" );

		gchar *pArgv[4];
		
		pArgv[ 0 ] = "ooffice";
		pArgv[ 1 ] = g_strconcat( "-accept=pipe,name=",
								  U2B( getPipeName() ).pData->buffer,
								  ";urp;StarOffice.ServiceManager",
								  NULL );
		pArgv[ 2 ] = "-bean";
		pArgv[ 3 ] = NULL;

		gboolean result = g_spawn_async( NULL,	// Working directory
										 pArgv,	// Child's argv
										 NULL,	// Child's envp
										 G_SPAWN_SEARCH_PATH,
										 NULL,	// Child setup function
										 NULL,	// User data for child_setup
										 NULL,	// &child_pid
										 NULL);	// GError ** FIXME

		g_free( pArgv[ 1 ] );

		if( !result ) {
			g_warning( "Unable to start OpenOffice.org" );
			exit(1);
		}

		for( int counter = 0; counter < 30; ++counter ) {
			try {
				g_message( "Trying to connect to OOo (%d)", counter + 2 );
				xInterface = xUnoUrlResolver->resolve( sConnect );
				break;
			} catch( uno::Exception ) {
				sleep( 1 );
			}
		}
	}
	
	if( !xInterface.is() ) {
		g_warning( "Unable to connect to OpenOffice.org" );
		exit( 2 );
	}

	Reference< XMultiComponentFactory > xRemoteComponentFactory(
		xInterface, UNO_QUERY );

	return xRemoteComponentFactory;
}

int
main( int argc, char *argv[] )
{
    gtk_init( &argc, &argv );

    Reference< uno::XComponentContext > xComponentContext =
		::cppu::defaultBootstrap_InitialComponentContext( UNO_BOOTSTRAP_INI );
    g_assert( xComponentContext.is() );

    Reference< lang::XMultiComponentFactory > xMultiComponentFactoryClient(
		xComponentContext->getServiceManager() );
    g_assert( xMultiComponentFactoryClient.is() );

	Reference< XInterface > xInterface(
		getRemoteServiceManager( xComponentContext ),
		UNO_QUERY );

    Reference< beans::XPropertySet > xPropSet( xInterface, uno::UNO_QUERY );
    xPropSet->getPropertyValue( DECLARE_ASCII( "DefaultContext") )
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
		xMultiServiceFactory->createInstance( SERVICENAME_TYPEDETECTION ),
		uno::UNO_QUERY );
	g_assert( xTypeDetection.is() );

	Reference< XMultiServiceFactory > xFrameLoaderFactory(
		xMultiServiceFactory->createInstance( SERVICENAME_FRAMELOADERFACTORY ),
		uno::UNO_QUERY );
	g_assert( xFrameLoaderFactory.is() );

	OUString sTypeName( xTypeDetection->queryTypeByURL( FILENAME ) );

	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance(sTypeName),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );
		
	FrameLoaderLoadFileFromUrl( xFrameLoader, xFrame, FILENAME, sTypeName );

	gtk_widget_grab_focus( pSocket );
	gtk_widget_show( pSocket );
	gtk_widget_show( pText );
	gtk_widget_show( pText2 );
	gtk_widget_show( pVBox );
	gtk_widget_show( pWindow );

	gtk_main();
	return 0;
}
