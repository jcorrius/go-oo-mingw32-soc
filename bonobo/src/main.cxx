#include <string.h>
#include <gtk/gtk.h>
#include <libbonoboui.h>

#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "star-frame-widget.h"

#define UNO_BOOTSTRAP_INI "file://" INIFILE
#define OAFIID "OAFIID:GNOME_OpenOfficeOrg_Control"
#define FILENAME "file:///demo/schmidt.sxw"

using namespace com::sun::star;
using namespace com::sun::star::beans;

using rtl::OUString;
using com::sun::star::uno::Reference;

extern "C" {

static void
FrameLoaderLoadFileFromUrl( Reference< frame::XSynchronousFrameLoader > xFrameLoader,
							Reference< frame::XFrame > xFrame,
							OUString sUrl)
{
	uno::Sequence< beans::PropertyValue > aProperties( 3 );

	aProperties[ 0 ] = PropertyValue( OUString::createFromAscii("FileName"),
									  0,
									  uno::makeAny(OUString::createFromAscii( FILENAME) ),
									  PropertyState_DIRECT_VALUE ); 
	
	aProperties[ 1 ] = PropertyValue( OUString::createFromAscii( "TypeName" ),
									  0,
									  uno::makeAny( OUString::createFromAscii( "writer_StarOffice_XML_Writer" ) ),
									  PropertyState_DIRECT_VALUE ); 
	
	aProperties[ 2 ] = PropertyValue( OUString::createFromAscii( "ReadOnly" ),
									  0,
									  uno::makeAny( sal_True ),
									  PropertyState_DIRECT_VALUE ); 

	xFrameLoader->load( aProperties, xFrame );
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
	
	Reference< XMultiServiceFactory > xFrameLoaderFactory(
		pSocket->service_manager->createInstance(
			OUString::createFromAscii( "com.sun.star.frame.FrameLoaderFactory" ) ),
			uno::UNO_QUERY );
	
	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance(
			OUString::createFromAscii( "writer_StarOffice_XML_Writer" ) ),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );
	
	FrameLoaderLoadFileFromUrl(
		xFrameLoader, xFrame,
		OUString::createFromAscii( FILENAME ) );
}

static void
activate( BonoboControl *bonobocontrol, gboolean arg1, gpointer user_data)
{
	gtk_widget_show( GTK_WIDGET( user_data ) );
}

static void
clicked( GtkWidget *widget, gpointer user_data )
{
	gtk_widget_show( GTK_WIDGET( user_data ) );
	gtk_widget_hide( GTK_WIDGET( widget ) );
}

static BonoboObject *
factory( BonoboGenericFactory *factory,
		 const char *component_id,
		 gpointer data )
{
	if( !strcmp( component_id, OAFIID ) ) {
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
		g_assert ( xMultiServiceFactory.is() );

		GtkWidget *pSocket = star_frame_widget_new();
		STAR_FRAME_WIDGET( pSocket )->service_manager = xMultiServiceFactory;
		g_signal_connect( pSocket, "realize", G_CALLBACK( realize ), NULL );

		GtkWidget *pHBox = gtk_hbox_new( FALSE, 0 );
 		gtk_box_pack_start( GTK_BOX( pHBox ), pSocket, TRUE, TRUE, 0 );

		GtkWidget *pButton = gtk_button_new_with_label( "Click me" );
		gtk_box_pack_start( GTK_BOX( pHBox ), pButton, FALSE, FALSE, 0 );
		g_signal_connect( pButton, "clicked", G_CALLBACK( clicked ), pSocket );

		BonoboControl *pControl = bonobo_control_new( pHBox );
		g_signal_connect( pControl, "activate", G_CALLBACK( activate ), pSocket );

		BonoboPersistFile *pPersistFile =
			bonobo_persist_file_new( NULL, NULL, OAFIID, NULL );

		bonobo_object_add_interface( BONOBO_OBJECT( pControl ),
									 BONOBO_OBJECT( pPersistFile ) );

		gtk_widget_show( pButton );
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
