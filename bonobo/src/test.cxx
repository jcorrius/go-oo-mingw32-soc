#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "remote-uno-helper.h"
#include "string-macros.h"
#include "services.h"
#include "star-frame-widget.h"

using namespace com::sun::star;
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

using rtl::OUString;

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

int
main( int argc, char *argv[] )
{
    if( argc < 2 || !strcmp( argv [1], "--help" ) )
	{
        fprintf( stderr, "Syntax:\n" );
		fprintf( stderr, "  test <document-name>\n" );
		exit( 1 );
	}
    const char *pFileName = argv[ argc - 1 ];
	if( strncmp( pFileName, "file:", 5 ) )
	{
        fprintf( stderr, "test prefers URIs\n" );
		pFileName = g_strconcat( "file://", pFileName, NULL );
	}
	OUString aFileName = OUString( pFileName, strlen( pFileName ),
								   RTL_TEXTENCODING_UTF8 );

    gtk_init( &argc, &argv );

	Reference< uno::XComponentContext > xRemoteContext = getComponentContext();
	g_assert( xRemoteContext.is() );

    GtkWidget *pWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    g_signal_connect (G_OBJECT (pWindow), "destroy",
					  G_CALLBACK (destroy), NULL);

    GtkWidget *pVBox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( pWindow ), pVBox );

	GtkWidget *pText = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( pVBox ), pText, FALSE, FALSE, 5 );

	GtkWidget *pText2 = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( pVBox ), pText2, FALSE, FALSE, 5 );

	GtkWidget *pSocket = star_frame_widget_new( xRemoteContext );
 	gtk_box_pack_start( GTK_BOX( pVBox ), pSocket, TRUE, TRUE, 5 );

	gtk_widget_realize( pSocket );

    Reference< frame::XFrame > xFrame(
		star_frame_widget_get_frame( STAR_FRAME_WIDGET( pSocket ) ) );
	g_assert( xFrame.is() );

	// Loading
    Reference< lang::XMultiServiceFactory > xMultiServiceFactory(
		xRemoteContext->getServiceManager(), uno::UNO_QUERY );
    g_assert (xMultiServiceFactory.is() );

	Reference< document::XTypeDetection > xTypeDetection(
		xMultiServiceFactory->createInstance( SERVICENAME_TYPEDETECTION ),
		uno::UNO_QUERY );
	g_assert( xTypeDetection.is() );

	Reference< XMultiServiceFactory > xFrameLoaderFactory(
		xMultiServiceFactory->createInstance( SERVICENAME_FRAMELOADERFACTORY ),
		uno::UNO_QUERY );
	g_assert( xFrameLoaderFactory.is() );

	OUString sTypeName( xTypeDetection->queryTypeByURL( aFileName ) );

	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance(sTypeName),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );
		
	FrameLoaderLoadFileFromUrl( xFrameLoader, xFrame, aFileName, sTypeName );

	gtk_widget_grab_focus( pSocket );
	gtk_widget_show( pSocket );
	gtk_widget_show( pText );
	gtk_widget_show( pText2 );
	gtk_widget_show( pVBox );
	gtk_widget_show( pWindow );

	gtk_main();
	return 0;
}
