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
#include <com/sun/star/view/DocumentZoomType.hpp>

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
	rtl::OUString uri;
};

BONOBO_CLASS_BOILERPLATE( OOoBonoboControl, ooo_bonobo_control,
						  BonoboControl, BONOBO_TYPE_CONTROL);

#define URL_SAVEASDOC			DECLARE_ASCII( "slot:5502" )
#define URL_EXPORTDOC			DECLARE_ASCII( "slot:5829" )
#define URL_EXPORTDOCASPDF		DECLARE_ASCII( "slot:6673" )
#define URL_PRINTDOC			DECLARE_ASCII( "slot:5504" )
#define URL_DOCINFO				DECLARE_ASCII( "slot:5535" )
#define URL_COPY				DECLARE_ASCII( "slot:5711" )

static void
verb_FileSaveAs_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	util::URL url;
	url.Complete = URL_SAVEASDOC;

	star_frame_widget_dispatch_slot_url( control->priv->sfw, url,
										 uno::Sequence< PropertyValue >(0) );
}

static void
verb_FileExport_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	util::URL url;
	url.Complete = URL_EXPORTDOC;

	star_frame_widget_dispatch_slot_url( control->priv->sfw, url,
										 uno::Sequence< PropertyValue >(0) );
}

static void
verb_FileExportPDF_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	util::URL url;
	url.Complete = URL_EXPORTDOCASPDF;

	star_frame_widget_dispatch_slot_url( control->priv->sfw, url,
										 uno::Sequence< PropertyValue >(0) );
}

static void
verb_FilePrint_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	util::URL url;
	url.Complete = URL_PRINTDOC;

	star_frame_widget_dispatch_slot_url( control->priv->sfw, url,
										 uno::Sequence< PropertyValue >(0) );
}

static void
verb_FileProperties_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	util::URL url;
	url.Complete = URL_DOCINFO;

	star_frame_widget_dispatch_slot_url( control->priv->sfw, url,
										 uno::Sequence< PropertyValue >(0) );
}


// FIXME make insensitive when selection is empty
static void
verb_EditCopy_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	util::URL url;
	url.Complete = URL_COPY;

	star_frame_widget_dispatch_slot_url( control->priv->sfw, url,
										 uno::Sequence< PropertyValue >(0) );
}

static void
verb_ZoomIn_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	Reference< XPropertySet > view_properties(
		star_frame_widget_get_view_properties( control->priv->sfw ) );

	if( !view_properties.is() )
		return;

	uno::Any a = view_properties->getPropertyValue( DECLARE_ASCII( "ZoomValue" ) );
	sal_Int16 view_zoom;
	a >>= view_zoom;

	view_zoom = ( sal_Int16 )( view_zoom * 1.2 );

	view_properties->setPropertyValue(
		DECLARE_ASCII( "ZoomType" ),
		uno::makeAny( (sal_Int16) view::DocumentZoomType::BY_VALUE ));
	view_properties->setPropertyValue( DECLARE_ASCII( "ZoomValue" ),
									   uno::makeAny( view_zoom ) );
}

static void
verb_ZoomOut_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	Reference< XPropertySet > view_properties(
		star_frame_widget_get_view_properties( control->priv->sfw ) );

	if( !view_properties.is() )
		return;

	uno::Any a = view_properties->getPropertyValue( DECLARE_ASCII( "ZoomValue" ) );
	sal_Int16 view_zoom;
	a >>= view_zoom;

	view_zoom = ( sal_Int16 )( view_zoom / 1.2 );

	view_properties->setPropertyValue(
		DECLARE_ASCII( "ZoomType" ),
		uno::makeAny( (sal_Int16) view::DocumentZoomType::BY_VALUE ));
	view_properties->setPropertyValue( DECLARE_ASCII( "ZoomValue" ),
									   uno::makeAny( view_zoom ) );
}

static void
verb_ZoomNormal_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	star_frame_widget_zoom_100( control->priv->sfw );
}

static void
verb_ZoomFit_cb( BonoboUIComponent *uic, gpointer user_data, const char *cname)
{
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	star_frame_widget_zoom_page_width( control->priv->sfw );
}

static int
load_uri( BonoboPersistFile *pf, const CORBA_char *text_uri,
		  CORBA_Environment *ev, gpointer user_data )
{
	OOoBonoboControl *pControl = OOO_BONOBO_CONTROL( user_data );

	pControl->priv->uri =
		DECLARE_ASCII( "file://" ) + B2U( rtl::OString( text_uri ) );

	g_message( "Load_uri called: %s", text_uri );
}

static void
FrameLoadFileFromUrl( Reference< frame::XFrame > xFrame,
					  Reference< lang::XMultiServiceFactory > xSMgr,
					  OUString sUrl )
{
	Reference< document::XTypeDetection > xTypeDetection(
		xSMgr->createInstance( SERVICENAME_TYPEDETECTION ), uno::UNO_QUERY );
	g_assert( xTypeDetection.is() );
	
	OUString sTypeName( xTypeDetection->queryTypeByURL( sUrl ) );

	Reference< lang::XMultiServiceFactory > xFrameLoaderFactory(
		xSMgr->createInstance( SERVICENAME_FRAMELOADERFACTORY ),
		uno::UNO_QUERY );

	Reference< frame::XSynchronousFrameLoader > xFrameLoader(
		xFrameLoaderFactory->createInstance( sTypeName ),
		uno::UNO_QUERY );
	g_assert( xFrameLoader.is() );

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
	OOoBonoboControl *control = OOO_BONOBO_CONTROL( user_data );

	Reference< frame::XFrame > xFrame(
		star_frame_widget_get_frame( STAR_FRAME_WIDGET( pSocket ) ) );
	g_assert( xFrame.is() );
		
	FrameLoadFileFromUrl( xFrame, pSocket->service_manager, control->priv->uri );
	
	// change to full screen mode (the frame)
	star_frame_widget_set_fullscreen( pSocket, sal_True );
}

static BonoboUIVerb verbs[] = {
	BONOBO_UI_VERB( "FileSaveAs",		verb_FileSaveAs_cb ),
	BONOBO_UI_VERB( "FileExport",		verb_FileExport_cb ),
	BONOBO_UI_VERB( "FileExportPDF",	verb_FileExportPDF_cb ),
	BONOBO_UI_VERB( "FilePrint",		verb_FilePrint_cb ),
	BONOBO_UI_VERB( "FileProperties",	verb_FileProperties_cb ),

	BONOBO_UI_VERB( "EditCopy",			verb_EditCopy_cb ),

 	BONOBO_UI_VERB( "ZoomIn",			verb_ZoomIn_cb ),
 	BONOBO_UI_VERB( "ZoomOut",			verb_ZoomOut_cb ),
 	BONOBO_UI_VERB( "ZoomNormal",		verb_ZoomNormal_cb ),
 	BONOBO_UI_VERB( "ZoomFit",			verb_ZoomFit_cb ),

	BONOBO_UI_VERB_END
};

static void
ooo_bonobo_control_activate( BonoboControl *control, gboolean activate )
{
	gtk_widget_show( GTK_WIDGET( OOO_BONOBO_CONTROL( control )->priv->sfw ) );

	BonoboUIComponent *ui_component = bonobo_control_get_ui_component( control );
	if( activate ) {
		Bonobo_UIContainer ui_container =
			bonobo_control_get_remote_ui_container( control, NULL );

		if( ui_container != CORBA_OBJECT_NIL ) {
			bonobo_ui_component_set_container( ui_component, ui_container, NULL );
			bonobo_ui_component_add_verb_list_with_data(
				ui_component, verbs, control );
			bonobo_ui_util_set_ui( ui_component, DATADIR,
								   "ooo-bonobo-control-ui.xml",
								   "ooo-bonobo", NULL );			
		}
	} else {
		bonobo_ui_component_unset_container( ui_component, NULL );
	}

	BONOBO_CALL_PARENT( BONOBO_CONTROL_CLASS, activate, ( control, activate ) );
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
