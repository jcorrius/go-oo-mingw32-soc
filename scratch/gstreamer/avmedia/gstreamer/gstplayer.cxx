/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#include "player.hxx"
#include "framegrabber.hxx"
#include "window.hxx"

#define AVMEDIA_GST_PLAYER_IMPLEMENTATIONNAME "com.sun.star.comp.avmedia.Player_GStreamer"
#define AVMEDIA_GST_PLAYER_SERVICENAME "com.sun.star.media.Player_GStreamer"

using namespace ::com::sun::star;

namespace avmedia { namespace gstreamer {

// ----------------
// - Player -
// ----------------

Player::Player( const uno::Reference< lang::XMultiServiceFactory >& rxMgr ) :
    mxMgr( rxMgr ),
    mnUnmutedVolume( 0 ),
    mbMuted( false ),
    mbLooping( false ),
    mbInitialized( false )
{
    // Initialize GStreamer library
    int argc = 1;
    char arguments[] = { "openoffice.org" };
    char** argv = arguments;

    GError* pError = g_error_new( 1, 0, "Failed to initialize GStreamer" );

    mbInitialized = gst_init_check( &argc, &arguments, &pError );

    g_error_free( pError );
}

// ------------------------------------------------------------------------------

Player::~Player()
{
    // Release the elements and pipeline
    if( mbInitialized )
    {
        if( NULL != mpPlaybin )
        {
            gst_element_set_state( mpPlaybin, GST_STATE_NULL );
            gst_object_unref( GST_OBJECT( mpPlaybin ) );
        }
    }
}

// ------------------------------------------------------------------------------

bool Player::create( const ::rtl::OUString& rURL )
{
    bool    bRet = false;

    // create all the elements and link them     
    
    if( mbInitialized )
    {
        mpLoop = g_main_loop_new( NULL, FALSE ); 

        mpPlaybin = gst_element_factory_make( "playbin", "player" );
        OString ascURL = OUStringToOString( rURL, RTL_TEXTENCODING_ASCII_US );
        g_object_set( G_OBJECT( mpPlaybin ), "uri", ascURL.getStr() , NULL );

        bRet = true;
    }


    if( bRet )
        maURL = rURL;
    else
        maURL = ::rtl::OUString();

    return bRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::start(  )
    throw (uno::RuntimeException)
{
    // set the pipeline state to READY and run the loop
    if( mbInitialized && NULL != mpPipeline )
    {
        GstBus* pBus = gst_pipeline_get_bus( GST_PIPELINE( mpPlaybin ) );
        gst_bus_add_watch( pBus, gst_bus_callback, mpLoop );
        gst_object_unref( pBus );

        gst_element_set_state( mpPlaybin, GST_STATE_PLAYING );
        
        // TODO embed in a sal_Thread
        g_main_loop_run( mpLoop );
    }
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::stop(  )
    throw (uno::RuntimeException)
{
    // set the pipeline in PAUSED STATE
    g_main_loop_quit( mpLoop );

    gst_element_set_state( mpPlaybin, GST_STATE_READY );
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::isPlaying()
    throw (uno::RuntimeException)
{
    bool            bRet = false;

    // return whether the pipeline is in READY STATE or not
    if( mbInitialized && NULL != mpPlaybin )
    {
        bRet = GST_STATE_PLAYING == GST_STATE( mpPlaybin );
    }

    return bRet;
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getDuration(  )
    throw (uno::RuntimeException)
{
    double duration = 0.0;

    // return media duration
    if( isPlaying() )
    {
        GstFormat format = GST_FORMAT_TIME;
        gint64 length;
        gst_element_query_duration( mpPlaybin, &format, &length);
        
        if( length < 0 )
        {
            duration = length / 1E9;
        }
    }

    return duration;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setMediaTime( double fTime )
    throw (uno::RuntimeException)
{
    // TODO implement
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getMediaTime(  )
    throw (uno::RuntimeException)
{
    double position = 0.0;

    // get current position in the stream
    if( isPlaying() )
    {
        GstFormat format = GST_FORMAT_TIME;
        gint64 pos;
        gst_element_query_position( mpPlaybin, &format, &pos );

        position = pos / 1E9;
    }
    
    return position; 
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setStopTime( double fTime )
    throw (uno::RuntimeException)
{
    // TODO implement
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getStopTime(  )
    throw (uno::RuntimeException)
{
    REFTIME aRefTime( 0.0 );

    // Get the time at which to stop

    return aRefTime; 
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setRate( double fRate )
    throw (uno::RuntimeException)
{
    // TODO set the window rate
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getRate(  )
    throw (uno::RuntimeException)
{
    double rate = 0.0;

    // TODO get the window rate
    if( mbInitialized )
    {
        
    }
    
    return rate;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setPlaybackLoop( sal_Bool bSet )
    throw (uno::RuntimeException)
{
    // TODO check how to do with GST
    mbLooping = bSet;
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::isPlaybackLoop(  )
    throw (uno::RuntimeException)
{
    // TODO check how to do with GST
    return mbLooping;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setMute( sal_Bool bSet )
    throw (uno::RuntimeException)
{
    // change the volume to 0 or the unmuted volume
    if( mpBA && ( mbMuted != bSet ) )
    {
        double nVolume = mnUnmutedVolume;
        if( bSet )
        {
            nVolume = 0.0;
        }
        g_object_set( G_OBJECT( mpPlaybin ), "volume", nVolume, NULL );
    }
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::isMute(  )
    throw (uno::RuntimeException)
{
    return mbMuted;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setVolumeDB( sal_Int16 nVolumeDB ) 
    throw (uno::RuntimeException)
{
    mnUnmutedVolume = ( static_cast< long >( nVolumeDB ) * 4 ) / 25;

    // change volume
    if( !mbMuted )
    {
        g_object_set( G_OBJECT( mpPlaybin ), "volume", mnUnmutedVolume );
    }
}

// ------------------------------------------------------------------------------
    
sal_Int16 SAL_CALL Player::getVolumeDB(  ) 
    throw (uno::RuntimeException)
{
    double nGstVolume = 0.0;
    g_object_get( G_OBJECT( mpPlaybin ), "volume", &nGstVolume, NULL );

    short nVolume = reinterpret_cast<short>( ( nGstVolume * 25 ) / 4 );

    return nVolume;
}

// ------------------------------------------------------------------------------

awt::Size SAL_CALL Player::getPreferredPlayerWindowSize(  )
    throw (uno::RuntimeException)
{
    awt::Size aSize( 0, 0 );
    
    if( mpBV )
    { 
        long nWidth = 0, nHeight = 0;

        // TODO fill nWidth and nHeight with the current player size

        aSize.Width = nWidth;
        aSize.Height = nHeight;
    }

    return aSize;
}

// ------------------------------------------------------------------------------

uno::Reference< ::media::XPlayerWindow > SAL_CALL Player::createPlayerWindow( const uno::Sequence< uno::Any >& aArguments )
    throw (uno::RuntimeException)
{
    uno::Reference< ::media::XPlayerWindow >    xRet;
    awt::Size                                   aSize( getPreferredPlayerWindowSize() );

    if( mpVW && aSize.Width > 0 && aSize.Height > 0 )
    {
        // TODO understand what needs to change
        ::avmedia::gstreamer::Window* pWindow = new ::avmedia::gstreamer::Window( mxMgr, *this );

        xRet = pWindow;

        if( !pWindow->create( aArguments ) )
            xRet = uno::Reference< ::media::XPlayerWindow >();
    }

    return xRet;
}

// ------------------------------------------------------------------------------

uno::Reference< media::XFrameGrabber > SAL_CALL Player::createFrameGrabber(  )
    throw (uno::RuntimeException)
{
    uno::Reference< media::XFrameGrabber > xRet;

    if( maURL.getLength() > 0 )
    {
        FrameGrabber* pGrabber = new FrameGrabber( mxMgr );
        
        xRet = pGrabber;
        
        if( !pGrabber->create( maURL ) )
            xRet.clear();
    }
    
    return xRet;
}

// ------------------------------------------------------------------------------

::rtl::OUString SAL_CALL Player::getImplementationName(  )
    throw (uno::RuntimeException)
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( AVMEDIA_GST_PLAYER_IMPLEMENTATIONNAME ) );
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::supportsService( const ::rtl::OUString& ServiceName )
    throw (uno::RuntimeException)
{
    return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( AVMEDIA_GST_PLAYER_SERVICENAME ) );
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL Player::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    uno::Sequence< ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( AVMEDIA_GST_PLAYER_SERVICENAME ) );

    return aRet;
}

} // namespace gstreamer
} // namespace avmedia
