#include <string.h>
#include <gtk/gtk.h>
#include <libbonoboui.h>

#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "remote-uno-helper.h"
#include "services.h"
#include "string-macros.h"
#include "star-frame-widget.h"

#define UNO_BOOTSTRAP_INI DECLARE_ASCII( "file://" INIFILE )
#define OAFIID "OAFIID:GNOME_OpenOfficeOrg_Control"
#define FILENAME DECLARE_ASCII( "file:///demo/schmidt.sxw" )

using namespace com::sun::star;
using namespace com::sun::star::beans;

using rtl::OUString;
using com::sun::star::uno::Reference;

extern "C" {

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

static int
load_uri( BonoboPersistFile *pf, const CORBA_char *text_uri,
		  CORBA_Environment *ev, gpointer user_data )
{
	StarFrameWidget *pSocket = STAR_FRAME_WIDGET( user_data );

 	if( !GTK_WIDGET_REALIZED( GTK_WIDGET( pSocket ) ) )
		/* FIXME */
		pSocket->uri = DECLARE_ASCII( "file://" ) + B2U( rtl::OString( text_uri ) );

	g_message( "Load_uri called: %s", text_uri );
}

static void
realize( GtkWidget *widget, gpointer user_data )
{
	StarFrameWidget *pSocket = STAR_FRAME_WIDGET( widget );

	Reference< awt::XWindowPeer > xWindowPeer(
		star_frame_widget_get_window_peer( STAR_FRAME_WIDGET( pSocket ) ) );
	g_assert( xWindowPeer.is() );
	Reference< frame::XFrame > xFrame(
		star_frame_widget_get_frame( STAR_FRAME_WIDGET( pSocket ) ) );
	g_assert( xFrame.is() );
	
	Reference< document::XTypeDetection > xTypeDetection(
		pSocket->service_manager->createInstance( SERVICENAME_TYPEDETECTION ),
		uno::UNO_QUERY );
	g_assert( xTypeDetection.is() );
	
	Reference< XMultiServiceFactory > xFrameLoaderFactory(
		pSocket->service_manager->createInstance(
			SERVICENAME_FRAMELOADERFACTORY ),
		uno::UNO_QUERY );

	OUString sTypeName( xTypeDetection->queryTypeByURL( pSocket->uri ) );
	g_message( "%s", U2B( sTypeName ).pData->buffer);

	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance( sTypeName ),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );
	
	FrameLoaderLoadFileFromUrl(
		xFrameLoader, xFrame, pSocket->uri, sTypeName );
}

static void
activate( BonoboControl *bonobocontrol, gboolean arg1, gpointer user_data)
{
	gtk_widget_show( GTK_WIDGET( user_data ) );
}

static BonoboObject *
factory( BonoboGenericFactory *factory,
		 const char *component_id,
		 gpointer data )
{
	if( !strcmp( component_id, OAFIID ) ) {
		Reference< uno::XComponentContext > xComponentContext =
			::cppu::defaultBootstrap_InitialComponentContext( UNO_BOOTSTRAP_INI );
		g_assert( xComponentContext.is() );

		Reference< lang::XMultiComponentFactory > xMultiComponentFactoryClient(
			xComponentContext->getServiceManager() );
		g_assert( xMultiComponentFactoryClient.is() );

		Reference< uno::XComponentContext > xRemoteContext(
			getRemoteComponentContext( xComponentContext ) );
		
		GtkWidget *pSocket = star_frame_widget_new( xRemoteContext );
		g_signal_connect( pSocket, "realize", G_CALLBACK( realize ), NULL );

		GtkWidget *pHBox = gtk_hbox_new( FALSE, 0 );
 		gtk_box_pack_start( GTK_BOX( pHBox ), pSocket, TRUE, TRUE, 0 );

		BonoboControl *pControl = bonobo_control_new( pHBox );
		g_signal_connect( pControl, "activate", G_CALLBACK( activate ), pSocket );

		STAR_FRAME_WIDGET( pSocket )->uri = FILENAME;
		BonoboPersistFile *pPersistFile =
			bonobo_persist_file_new( load_uri, NULL, OAFIID, pSocket );

		bonobo_object_add_interface( BONOBO_OBJECT( pControl ),
									 BONOBO_OBJECT( pPersistFile ) );

		gtk_widget_show( pHBox );

		return BONOBO_OBJECT( pControl );
	}
}

int
main( int argc, char *argv[] )
{
	BONOBO_FACTORY_INIT( "ooo-bonobo", VERSION, &argc, argv );
	return bonobo_generic_factory_main( "OAFIID:GNOME_OpenOfficeOrg_Factory", factory, NULL );
}

} // extern "C"
