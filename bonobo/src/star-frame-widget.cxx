#include <stdio.h>
#include "star-frame-widget.h"

#include <libgnome/gnome-macros.h>

#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XSystemChildFactory.hpp>
#include <com/sun/star/lang/SystemDependent.hpp>

using rtl::OUString;
using namespace com::sun::star::awt;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

#define CLASS_BOILERPLATE(type, type_as_function,			\
						  parent_type, parent_type_macro)		\
	BONOBO_BOILERPLATE(type, type_as_function, type,		\
					   parent_type, parent_type_macro,		\
					   REGISTER_TYPE)

#define REGISTER_TYPE(type, type_as_function, corba_type,		\
					  parent_type, parent_type_macro)		\
	g_type_register_static (parent_type_macro, #type, &object_info, \
							(GTypeFlags)0)


CLASS_BOILERPLATE( StarFrameWidget, star_frame_widget,
				   GtkSocket, GTK_TYPE_SOCKET );

struct _StarFrameWidgetPrivate {
    Reference< XWindowPeer > x_window_peer;
	Reference< XFrame > x_frame;
};

Reference< XWindowPeer >
star_frame_widget_get_window_peer( StarFrameWidget *sfw )
{
	if( ! GTK_WIDGET_REALIZED( GTK_WIDGET( sfw ) ) )
		gtk_widget_realize( GTK_WIDGET( sfw ) );

	g_assert( sfw->service_manager.is() );
	Reference< XSystemChildFactory > xChildFactory(
		sfw->service_manager->createInstance(
			OUString::createFromAscii( "com.sun.star.awt.Toolkit" ) ),
		UNO_QUERY );
	g_assert( xChildFactory.is() );

	sfw->priv->x_window_peer.set(
		xChildFactory->createSystemChild(
			makeAny( (const sal_Int32) gtk_socket_get_id( GTK_SOCKET( sfw ) ) ),
			Sequence< sal_Int8 >(),
			SystemDependent::SYSTEM_XWINDOW ) );
	g_assert( sfw->priv->x_window_peer.is() );

	return sfw->priv->x_window_peer;		
}

Reference< XFrame >
star_frame_widget_get_frame( StarFrameWidget *sfw )
{
	g_assert( sfw->priv->x_window_peer.is() );
	g_assert( sfw->service_manager.is() );

	sfw->priv->x_frame.set(
		sfw->service_manager->createInstance(
			OUString::createFromAscii( "com.sun.star.frame.Frame" ) ),
		UNO_QUERY );

    sfw->priv->x_frame->setName( OUString::createFromAscii( "A Frame in GTK+" ) );

	Reference< XWindow > xWindow( sfw->priv->x_window_peer, UNO_QUERY );
	sfw->priv->x_frame->initialize( xWindow );
	
    Reference< XFramesSupplier > xTreeRoot(
		sfw->service_manager->createInstance(
			OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ),
		UNO_QUERY );
    g_assert( xTreeRoot.is() );

    Reference< XFrames > xChildContainer = xTreeRoot->getFrames();
    xChildContainer->append( sfw->priv->x_frame );

    xWindow->setVisible( sal_True ); // FIXME

	return sfw->priv->x_frame;
}

static void
star_frame_widget_class_init( StarFrameWidgetClass * )
{
}

static void
star_frame_widget_instance_init( StarFrameWidget *sfw )
{
	sfw->priv = new StarFrameWidgetPrivate;
}

GtkWidget *
star_frame_widget_new ( Reference< XMultiServiceFactory > service_factory )
{
    StarFrameWidget *swf = (StarFrameWidget *)g_object_new( TYPE_STAR_FRAME_WIDGET, NULL );
	swf->service_manager = service_factory;
	return GTK_WIDGET( swf );
}
