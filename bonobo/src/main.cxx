#include <string.h>
#include <gtk/gtk.h>
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

#include "ooo-bonobo-control.h"
#include "remote-uno-helper.h"
#include "services.h"
#include "string-macros.h"
#include "star-frame-widget.h"

#define OAFIID "OAFIID:GNOME_OpenOfficeOrg_Control"

using namespace com::sun::star;
using namespace com::sun::star::beans;

using rtl::OUString;
using com::sun::star::uno::Reference;

extern "C" {

static BonoboObject *
factory( BonoboGenericFactory *factory,
		 const char *component_id,
		 gpointer data )
{
	if( !strcmp( component_id, OAFIID ) )
	{
		Reference< uno::XComponentContext > xRemoteContext = getComponentContext();
		g_assert( xRemoteContext.is() );

		BonoboControl *pControl =
			ooo_bonobo_control_new( xRemoteContext );

		return BONOBO_OBJECT( pControl );
	}
	return NULL;
}

int
main( int argc, char *argv[] )
{
	bindtextdomain( GETTEXT_PACKAGE, GNOMELOCALEDIR );
	bind_textdomain_codeset( GETTEXT_PACKAGE, "UTF-8" );
	textdomain( GETTEXT_PACKAGE );

	BONOBO_FACTORY_INIT( "ooo-bonobo", VERSION, &argc, argv );
	return bonobo_generic_factory_main( "OAFIID:GNOME_OpenOfficeOrg_Factory", factory, NULL );
}

} // extern "C"
