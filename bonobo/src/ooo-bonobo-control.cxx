#include "ooo-bonobo-control.h"

#include <libbonoboui.h>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>

#include "services.h"
#include "star-frame-widget.h"

#define OAFIID "OAFIID:GNOME_OpenOfficeOrg_Control"

using namespace com::sun::star;
using namespace com::sun::star::beans;

using rtl::OUString;
using com::sun::star::uno::Reference;

struct _OOoBonoboControlPrivate {
	GtkWidget *hbox;
	StarFrameWidget *sfw;
};

BONOBO_CLASS_BOILERPLATE( OOoBonoboControl, ooo_bonobo_control,
						  BonoboControl, BONOBO_TYPE_CONTROL);

static int
load_uri( BonoboPersistFile *pf, const CORBA_char *text_uri,
		  CORBA_Environment *ev, gpointer user_data )
{
	OOoBonoboControl *pControl = OOO_BONOBO_CONTROL( user_data );

	pControl->uri = DECLARE_ASCII( "file://" ) + B2U( rtl::OString( text_uri ) );

	g_message( "Load_uri called: %s", text_uri );
}

static void
FrameLoaderLoadFileFromUrl( Reference< frame::XSynchronousFrameLoader > xFrameLoader,
							Reference< frame::XFrame > xFrame,
							OUString sUrl,
							OUString sTypeName)
{
	uno::Sequence< PropertyValue > aProperties( 3 );

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

static void
frame_widget_realize( GtkWidget *widget, gpointer user_data )
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

	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );
	OUString sTypeName( xTypeDetection->queryTypeByURL( control->uri ) );
	g_message( "%s", U2B( sTypeName ).pData->buffer);

	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance( sTypeName ),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );
	
	FrameLoaderLoadFileFromUrl(
		xFrameLoader, xFrame, control->uri, sTypeName );
	
	// change to full screen mode (the frame)
	Reference< util::XURLTransformer > xURLTransformer(
		pSocket->service_manager->createInstance( SERVICENAME_URLTRANSFORMER ),
		uno::UNO_QUERY );

	util::URL url;

	url.Complete = DECLARE_ASCII( "slot:5627" );
	url.Port = 0;

	xURLTransformer->parseSmart( url, DECLARE_ASCII( "slot" ) );

	Reference< frame::XDispatchProvider > xDispProv( xFrame, uno::UNO_QUERY );
	Reference< frame::XDispatch > xDispatch =
		xDispProv->queryDispatch( url, OUString(), 0);

	uno::Sequence< beans::PropertyValue > aProperties( 1 );
	aProperties[ 0 ] = PropertyValue( DECLARE_ASCII( "FullScreen" ),
									  0,
									  uno::makeAny( sal_True ),
									  PropertyState_DIRECT_VALUE );

	g_message( "switch to full screen" );
	xDispatch->dispatch( url, aProperties );
}


static void
ooo_bonobo_control_activate( BonoboControl *control, gboolean activate )
{
	gtk_widget_show( GTK_WIDGET( OOO_BONOBO_CONTROL( control )->priv->sfw ) );
}

static void
ooo_bonobo_control_finalize( GObject *object )
{
	delete OOO_BONOBO_CONTROL( object )->priv;
	BONOBO_CALL_PARENT( G_OBJECT_CLASS, finalize, ( object ) );
}

static void
ooo_bonobo_control_instance_init( OOoBonoboControl *control )
{
	control->priv = new OOoBonoboControlPrivate;
}

static void
ooo_bonobo_control_class_init( OOoBonoboControlClass *klass )
{
	GObjectClass *object_class = G_OBJECT_CLASS( klass );

	object_class->finalize = ooo_bonobo_control_finalize;

	BonoboControlClass *control_class = BONOBO_CONTROL_CLASS( klass );

	control_class->activate = ooo_bonobo_control_activate;
}

BonoboControl *
ooo_bonobo_control_new( Reference< XComponentContext > component_context )
{
	g_return_val_if_fail( component_context.is(), NULL );

	OOoBonoboControl *control = 
		OOO_BONOBO_CONTROL( g_object_new( TYPE_OOO_BONOBO_CONTROL, NULL ) );
	
	control->priv->hbox = gtk_hbox_new( FALSE, 0 );
	control->priv->sfw =
		STAR_FRAME_WIDGET( star_frame_widget_new( component_context ) );
	g_signal_connect( control->priv->sfw, "realize",
					  G_CALLBACK( frame_widget_realize ), control );

	gtk_box_pack_start( GTK_BOX( control->priv->hbox ),
						GTK_WIDGET( control->priv->sfw ), TRUE, TRUE, 0 );

	gtk_widget_show( control->priv->hbox );

	BonoboPersistFile *persist_file =
		bonobo_persist_file_new( load_uri, NULL, OAFIID, control );

	bonobo_object_add_interface( BONOBO_OBJECT( control ),
								 BONOBO_OBJECT( persist_file ) );

	return bonobo_control_construct( BONOBO_CONTROL( control ),
									 control->priv->hbox );
}
