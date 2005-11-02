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
#include <canvas/verbosetrace.hxx>
#include <canvas/canvastools.hxx>
#include <canvas/base/linepolypolygonbase.hxx>

#include <osl/mutex.hxx>
#include <cppuhelper/compbase1.hxx>

#include <com/sun/star/lang/NoSupportException.hpp>

#include <basegfx/tools/canvastools.hxx>

#include <vcl/syschild.hxx>
#include <vcl/canvastools.hxx>

#include "cairo_helper.hxx"
#include "cairo_spritecanvas.hxx"
#include "cairo_canvasbitmap.hxx"
#include "cairo_devicehelper.hxx"

using namespace ::cairo;
using namespace ::com::sun::star;

namespace cairocanvas
{
    DeviceHelper::DeviceHelper() :
        mpSpriteCanvas( NULL ),
        maSize(),
        mbFullScreen( false ),
	mpBufferSurface( NULL ),
	mpBufferCairo( NULL )
    {
    }

    void DeviceHelper::init( Window& 				rOutputWindow,
			     SpriteCanvas&				rSpriteCanvas,
                             const ::basegfx::B2ISize&	rSize,
                             bool                       bFullscreen )
    {
	mpOutputWindow = &rOutputWindow;
        mpSpriteCanvas = &rSpriteCanvas;
        mbFullScreen = bFullscreen;

	mpWindowSurface = NULL;
	mpBufferSurface = NULL;
	mpBufferCairo = NULL;

	// check whether we're a SysChild: have to fetch system data
	// directly from SystemChildWindow, because the GetSystemData
	// method is unfortunately not virtual
	const SystemChildWindow* pSysChild = dynamic_cast< const SystemChildWindow* >( mpOutputWindow );
	if( pSysChild )
	    mpSysData = pSysChild->GetSystemData();
	else
	    mpSysData = mpOutputWindow->GetSystemData();

	setSize( rSize );
    }

    void DeviceHelper::disposing()
    {
        // release all references
        mpSpriteCanvas = NULL;

	if( mpWindowSurface ) {
	    cairo_surface_destroy( mpWindowSurface );
	    mpWindowSurface = NULL;
	}

	if( mpBufferCairo ) {
	    cairo_destroy( mpBufferCairo );
	    mpBufferCairo = NULL;
	}

	if( mpBufferSurface ) {
	    cairo_surface_destroy( mpBufferSurface );
	    mpBufferSurface = NULL;
	}
    }

    geometry::RealSize2D DeviceHelper::getPhysicalResolution()
    {
        if( !mpOutputWindow )
            return ::canvas::tools::createInfiniteSize2D(); // we're disposed

        // Map a one-by-one millimeter box to pixel
        const MapMode aOldMapMode( mpOutputWindow->GetMapMode() );
        mpOutputWindow->SetMapMode( MapMode(MAP_MM) );
        const Size aPixelSize( mpOutputWindow->LogicToPixel(Size(1,1)) );
        mpOutputWindow->SetMapMode( aOldMapMode );

        return ::vcl::unotools::size2DFromSize( aPixelSize );
    }

    geometry::RealSize2D DeviceHelper::getPhysicalSize()
    {
        if( !mpOutputWindow )
            return ::canvas::tools::createInfiniteSize2D(); // we're disposed

        // Map the pixel dimensions of the output window to millimeter
        const MapMode aOldMapMode( mpOutputWindow->GetMapMode() );
        mpOutputWindow->SetMapMode( MapMode(MAP_MM) );
        const Size aLogSize( mpOutputWindow->PixelToLogic(mpOutputWindow->GetOutputSizePixel()) );
        mpOutputWindow->SetMapMode( aOldMapMode );

        return ::vcl::unotools::size2DFromSize( aLogSize );
    }
//     geometry::RealSize2D DeviceHelper::getPhysicalResolution()
//     {
//         return geometry::RealSize2D( 75, 75 );
//     }

//     geometry::RealSize2D DeviceHelper::getPhysicalSize()
//     {
//         return geometry::RealSize2D( 210, 280 );
//     }

    uno::Reference< rendering::XLinePolyPolygon2D > DeviceHelper::createCompatibleLinePolyPolygon( 
        const uno::Reference< rendering::XGraphicDevice >& 				rDevice,
        const uno::Sequence< uno::Sequence< geometry::RealPoint2D > >&	points )
    {
        // disposed?
        if( !mpSpriteCanvas )
            return uno::Reference< rendering::XLinePolyPolygon2D >(); // we're disposed

        return uno::Reference< rendering::XLinePolyPolygon2D >( 
            new ::canvas::LinePolyPolygonBase( 
                ::basegfx::unotools::polyPolygonFromPoint2DSequenceSequence( points ) ) );
    }

    uno::Reference< rendering::XBezierPolyPolygon2D > DeviceHelper::createCompatibleBezierPolyPolygon( 
        const uno::Reference< rendering::XGraphicDevice >& 						rDevice,
        const uno::Sequence< uno::Sequence< geometry::RealBezierSegment2D > >&	points )
    {
        // disposed?
        if( !mpSpriteCanvas )
            return uno::Reference< rendering::XBezierPolyPolygon2D >(); // we're disposed

        return uno::Reference< rendering::XBezierPolyPolygon2D >( 
            new ::canvas::LinePolyPolygonBase( 
                ::basegfx::unotools::polyPolygonFromBezier2DSequenceSequence( points ) ) );
    }

    uno::Reference< rendering::XBitmap > DeviceHelper::createCompatibleBitmap( 
        const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
        const geometry::IntegerSize2D& 						size )
    {
        // disposed?
        if( !mpSpriteCanvas )
            return uno::Reference< rendering::XBitmap >(); // we're disposed

        return uno::Reference< rendering::XBitmap >(
            new CanvasBitmap(
                ::basegfx::unotools::b2ISizeFromIntegerSize2D( size ),
                mpSpriteCanvas,
                false )); 
    }

    uno::Reference< rendering::XVolatileBitmap > DeviceHelper::createVolatileBitmap( 
        const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
        const geometry::IntegerSize2D& 						size )
    {
        return uno::Reference< rendering::XVolatileBitmap >();
    }

    uno::Reference< rendering::XBitmap > DeviceHelper::createCompatibleAlphaBitmap( 
        const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
        const geometry::IntegerSize2D& 						size )
    {
        // disposed?
        if( !mpSpriteCanvas )
            return uno::Reference< rendering::XBitmap >(); // we're disposed

        return uno::Reference< rendering::XBitmap >(
            new CanvasBitmap(
                ::basegfx::unotools::b2ISizeFromIntegerSize2D( size ),
                mpSpriteCanvas,
                true )); 
    }

    uno::Reference< rendering::XVolatileBitmap > DeviceHelper::createVolatileAlphaBitmap( 
        const uno::Reference< rendering::XGraphicDevice >& 	rDevice,
        const geometry::IntegerSize2D& 						size )
    {
        return uno::Reference< rendering::XVolatileBitmap >();
    }

    sal_Bool DeviceHelper::hasFullScreenMode()
    {
        // TODO(F3): offer fullscreen mode the XCanvas way
        return false;
    }

    sal_Bool DeviceHelper::enterFullScreenMode( sal_Bool bEnter )
    {
        // TODO(F3): offer fullscreen mode the XCanvas way
        return false;
    }
    
    ::sal_Int32 DeviceHelper::createBuffers( ::sal_Int32 nBuffers )
    {
        // TODO(F3): implement XBufferStrategy interface. For now, we
        // _always_ will have exactly one backbuffer
        return 1;
    }

    void DeviceHelper::destroyBuffers()
    {
        // TODO(F3): implement XBufferStrategy interface. For now, we
        // _always_ will have exactly one backbuffer
    }

    ::sal_Bool DeviceHelper::showBuffer( ::sal_Bool bUpdateAll )
    {
        // forward to sprite canvas helper
        if( !mpSpriteCanvas )
            return false;

        return mpSpriteCanvas->updateScreen( bUpdateAll );
    }

    ::sal_Bool DeviceHelper::switchBuffer( ::sal_Bool bUpdateAll )
    {
        // no difference for Cairo canvas
        return showBuffer( bUpdateAll );
    }

    const void* DeviceHelper::getDeviceHandle() const
    {
        return NULL;
    }

    const void* DeviceHelper::getSurfaceHandle() const
    {
        return NULL;
    }
    
    void DeviceHelper::setSize( const ::basegfx::B2ISize& rSize )
    {
	OSL_TRACE("set device size %d x %d", rSize.getX(), rSize.getY() );

        maSize = rSize;

	if( mpWindowSurface )
	{
	    cairo_surface_destroy( mpWindowSurface );
	}
	mpWindowSurface = (Surface*) cairoHelperGetSurface( mpSysData,
							    mpOutputWindow->GetOutOffXPixel(), mpOutputWindow->GetOutOffYPixel(),
							    maSize.getX(), maSize.getY() );

	if( mpBufferSurface )
	{
	    cairo_surface_destroy( mpBufferSurface );
	}
	mpBufferSurface = cairo_surface_create_similar( mpWindowSurface, CAIRO_CONTENT_COLOR, maSize.getX(), maSize.getY() );

	if( mpBufferCairo )
	{
	    cairo_destroy( mpBufferCairo );
	}
	mpBufferCairo = cairo_create( mpBufferSurface );

	mpSpriteCanvas->setSizePixel( maSize );
    }

    const ::basegfx::B2ISize& DeviceHelper::getSizePixel()
    {
	return maSize;
    }

    void DeviceHelper::notifySizeUpdate( const awt::Rectangle& rBounds )
    {
	setSize( ::basegfx::B2ISize(rBounds.Width, rBounds.Height) );
    }

    Surface* DeviceHelper::getBufferSurface()
    {
	return mpBufferSurface;
    }

    Surface* DeviceHelper::getWindowSurface()
    {
	return mpWindowSurface;
    }

    Surface* DeviceHelper::getSurface( const ::basegfx::B2ISize& rSize, Content aContent )
    {
	if( mpBufferSurface )
	    return cairo_surface_create_similar( mpBufferSurface, aContent, rSize.getX(), rSize.getY() );

	return NULL;
    }

    Surface* DeviceHelper::getSurface( Content aContent )
    {
	return getSurface( maSize, aContent );
    }

    Surface* DeviceHelper::getSurface( BitmapSystemData& rData, const Size& rSize )
    {
	return (Surface *) cairoHelperGetSurface( mpSysData, rData, rSize.Width(), rSize.Height() );
    }

    void DeviceHelper::flush()
    {
	cairoHelperFlush( mpSysData );
    }
}
