#ifndef STAR_FRAME_WIDGET_H
#define STAR_FRAME_WIDGET_H

#include <gtk/gtksocket.h>

#define TYPE_STAR_FRAME_WIDGET           (star_frame_widget_get_type ())
#define STAR_FRAME_WIDGET(obj)           (GTK_CHECK_CAST ((obj), TYPE_STAR_FRAME_WIDGET, StarFrameWidget))
#define STAR_FRAME_WIDGET_CLASS(klass)   (GTK_CHECK_CLASS_CAST ((klass), TYPE_STAR_FRAME_WIDGET, StarFrameWidgetClass))
#define IS_STAR_FRAME_WIDGET(obj)        (GTK_CHECK_TYPE ((obj), TYPE_STAR_FRAME_WIDGET))
#define IS_STAR_FRAME_WIDGET_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), TYPE_STAR_FRAME_WIDGET))

typedef struct _StarFrameWidget        StarFrameWidget;
typedef struct _StarFrameWidgetClass   StarFrameWidgetClass;
typedef struct _StarFrameWidgetPrivate StarFrameWidgetPrivate;

#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

using com::sun::star::uno::Reference;
using com::sun::star::frame::XFrame;
using com::sun::star::lang::XMultiServiceFactory;
using com::sun::star::uno::XComponentContext;

struct _StarFrameWidget {
    GtkSocket parent;
    StarFrameWidgetPrivate *priv;

	/* Temporary */
	Reference< XMultiServiceFactory > service_manager;
};

struct _StarFrameWidgetClass {
    GtkSocketClass parent_class;
};

GType star_frame_widget_get_type ( void );
GtkWidget *star_frame_widget_new ( Reference< XComponentContext > component_context );

Reference< XFrame > star_frame_widget_get_frame( StarFrameWidget *sfw );
void star_frame_widget_dispatch_slot_url( StarFrameWidget *sfw, ::com::sun::star::util::URL url, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > &properties );
void star_frame_widget_set_fullscreen( StarFrameWidget *sfw, sal_Bool fullscreen );
void star_frame_widget_zoom_100( StarFrameWidget *sfw );
void star_frame_widget_zoom_page_width( StarFrameWidget *sfw );
::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > star_frame_widget_get_view_properties( StarFrameWidget *sfw );
#endif /* STAR_FRAME_WIDGET_H */
