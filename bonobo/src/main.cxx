#include <string.h>
#include <gtk/gtk.h>
#include <libbonoboui.h>

#include <cppuhelper/bootstrap.hxx>

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

#define UNO_BOOTSTRAP_INI DECLARE_ASCII( "file://" INIFILE )
#define OAFIID "OAFIID:GNOME_OpenOfficeOrg_Control"
#define FILENAME DECLARE_ASCII( "file:///demo/schmidt.sxw" )

using namespace com::sun::star;
using namespace com::sun::star::beans;

using rtl::OUString;
using com::sun::star::uno::Reference;

extern "C" {

static int
load_uri( BonoboPersistFile *pf, const CORBA_char *text_uri,
		  CORBA_Environment *ev, gpointer user_data )
{
	OOoBonoboControl *pControl = OOO_BONOBO_CONTROL( user_data );

	pControl->uri = DECLARE_ASCII( "file://" ) + B2U( rtl::OString( text_uri ) );

	g_message( "Load_uri called: %s", text_uri );
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
		
		BonoboControl *pControl = ooo_bonobo_control_new( xRemoteContext );

		BonoboPersistFile *pPersistFile =
			bonobo_persist_file_new( load_uri, NULL, OAFIID, pControl );

		bonobo_object_add_interface( BONOBO_OBJECT( pControl ),
									 BONOBO_OBJECT( pPersistFile ) );

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
