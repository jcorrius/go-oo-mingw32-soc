#include "remote-uno-helper.h"

#include <unistd.h>

#include <glib.h>

#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include "services.h"

using namespace com::sun::star;
using namespace com::sun::star::bridge;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

using rtl::OUString;

static OUString
getPipeName()
{
	return 
		OUString::createFromAscii( g_get_user_name() ) +
		DECLARE_ASCII( "_ooo_bonobo" );
}

Reference< XComponentContext >
getRemoteComponentContext( const Reference< XComponentContext >& xComponentContext )
{
	Reference< XMultiComponentFactory > xLocalSMgr(
		xComponentContext->getServiceManager() );

	Reference< XUnoUrlResolver > xUnoUrlResolver(
		xLocalSMgr->createInstanceWithContext( SERVICENAME_UNOURLRESOLVER,
											   xComponentContext ),
		UNO_QUERY );

	Reference< XInterface > xInterface;
	OUString sConnect( DECLARE_ASCII( "uno:pipe,name=" ) +
					   getPipeName() +
					   DECLARE_ASCII( ";urp;StarOffice.ComponentContext" ) );
	try {
		xInterface = xUnoUrlResolver->resolve( sConnect );
	} catch( uno::Exception ) {
		g_message( "Trying to start OOo" );

		gchar *pArgv[5];
		
		pArgv[ 0 ] = "ooffice";
		pArgv[ 1 ] = g_strconcat( "-accept=pipe,name=",
								  U2B( getPipeName() ).pData->buffer,
								  ";urp;StarOffice.ComponentContext",
								  NULL );
		pArgv[ 2 ] = "-bean";
		pArgv[ 3 ] = "-norestore";
		pArgv[ 4 ] = NULL;

		gboolean result = g_spawn_async( NULL,	// Working directory
										 pArgv,	// Child's argv
										 NULL,	// Child's envp
										 G_SPAWN_SEARCH_PATH,
										 NULL,	// Child setup function
										 NULL,	// User data for child_setup
										 NULL,	// &child_pid
										 NULL);	// GError ** FIXME

		g_free( pArgv[ 1 ] );

		if( !result ) {
			g_warning( "Unable to start OpenOffice.org" );
			exit(1);
		}

		for( int counter = 0; counter < 30; ++counter ) {
			try {
				g_message( "Trying to connect to OOo (%d)", counter + 2 );
				xInterface = xUnoUrlResolver->resolve( sConnect );
				break;
			} catch( uno::Exception ) {
				sleep( 1 );
			}
		}
	}
	
	if( !xInterface.is() ) {
		g_warning( "Unable to connect to OpenOffice.org" );
		exit( 2 );
	}

	Reference< XComponentContext > xRemoteComponentContext(
		xInterface, UNO_QUERY );

	return xRemoteComponentContext;
}
