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

#include <canvas/debug.hxx>
#include <canvas/canvastools.hxx>

#include "cairo_canvasbitmap.hxx"


using namespace ::cairo;
using namespace ::com::sun::star;

namespace cairocanvas
{
    CanvasBitmap::CanvasBitmap( const ::basegfx::B2ISize& rSize,
                                const DeviceRef&          rDevice,
                                bool                      bHasAlpha ) :
        mpDevice( rDevice )
    {
        ENSURE_AND_THROW( mpDevice.is(),
                          "CanvasBitmap::CanvasBitmap(): Invalid surface or device" );

	OSL_TRACE( "bitmap size: %dx%d", rSize.getX(), rSize.getY() );

	mpBufferSurface = mpDevice->getSurface( rSize, bHasAlpha ? CAIRO_CONTENT_COLOR_ALPHA : CAIRO_CONTENT_COLOR );
	mpBufferCairo = cairo_create( mpBufferSurface );

        maCanvasHelper.init( rSize,
                             *mpDevice.get(),
			     mpBufferCairo,
                             bHasAlpha );

	mbHasAlpha = bHasAlpha;
    }

    void SAL_CALL CanvasBitmap::disposing()
    {
        mpDevice.clear();

	if( mpBufferCairo ) {
	    cairo_destroy( mpBufferCairo );
	    mpBufferCairo = NULL;
	}

	if( mpBufferSurface ) {
	    cairo_surface_destroy( mpBufferSurface );
	    mpBufferSurface = NULL;
	}

        // forward to parent
        CanvasBitmap_Base::disposing();
    }

    Cairo* CanvasBitmap::getCairo()
    {
	return mpBufferCairo;
    }

#define IMPLEMENTATION_NAME "CairoCanvas.CanvasBitmap"
#define SERVICE_NAME "com.sun.star.rendering.CanvasBitmap"

    ::rtl::OUString SAL_CALL CanvasBitmap::getImplementationName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL CanvasBitmap::supportsService( const ::rtl::OUString& ServiceName ) throw (uno::RuntimeException)
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL CanvasBitmap::getSupportedServiceNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );
        
        return aRet;
    }

}
