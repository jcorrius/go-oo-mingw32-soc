#ifndef OOO_BONOBO_CONTROL_H
#define OOO_BONOBO_CONTROL_H

#include <bonobo/bonobo-control.h>

#include <rtl/ustring.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>

#define TYPE_OOO_BONOBO_CONTROL            (ooo_bonobo_control_get_type ())
#define OOO_BONOBO_CONTROL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_OOO_BONOBO_CONTROL, OOoBonoboControl))
#define OOO_BONOBO_CONTROL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_OOO_BONOBO_CONTROL, OOoBonoboControlClass))
#define IS_OOO_BONOBO_CONTROL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OOO_BONOBO_CONTROL))
#define IS_OOO_BONOBO_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_OOO_BONOBO_CONTROL))

typedef struct _OOoBonoboControl        OOoBonoboControl;
typedef struct _OOoBonoboControlClass   OOoBonoboControlClass;
typedef struct _OOoBonoboControlPrivate OOoBonoboControlPrivate;

struct _OOoBonoboControl {
	BonoboControl parent;

	OOoBonoboControlPrivate *priv;

	/* Temporary */
	rtl::OUString uri;
};

struct _OOoBonoboControlClass {
	BonoboControlClass parent_class;
};

GType		   ooo_bonobo_control_get_type  ();
BonoboControl *ooo_bonobo_control_new		( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > component_context );

#endif // OOO_BONOBO_CONTROL_H
