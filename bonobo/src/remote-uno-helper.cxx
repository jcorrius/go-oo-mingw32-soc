#include "remote-uno-helper.h"

#include <unistd.h>

#include <glib.h>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include "services.h"

#define UNO_BOOTSTRAP_INI DECLARE_ASCII( "file://" INIFILE )

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

static Reference< XComponentContext >
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

::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >
getComponentContext()
{
    if( !g_file_test( INIFILE, G_FILE_TEST_EXISTS ) )
        g_error( "Without '%s' installed nothing will work", INIFILE );

    if( !g_file_test( RDBFILE, G_FILE_TEST_EXISTS ) )
        g_error( "Without '%s' installed nothing will work", RDBFILE );

	Reference< uno::XComponentContext > xComponentContext;
	try {
		xComponentContext =
			::cppu::defaultBootstrap_InitialComponentContext( UNO_BOOTSTRAP_INI );
	} catch( registry::InvalidRegistryException &ire ) {
		g_error( "InvalidRegistryException."
				 " %s is invalid or %s doesn't contain the correct path.",
				 RDBFILE, INIFILE );
	}

	g_assert( xComponentContext.is() );


	Reference< lang::XMultiComponentFactory > xMultiComponentFactoryClient(
        xComponentContext->getServiceManager() );
	g_assert( xMultiComponentFactoryClient.is() );

	return getRemoteComponentContext( xComponentContext );
}
