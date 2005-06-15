/*************************************************************************
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include <canvas/debug.hxx>

#include "cairo_helper.hxx"
#include "cairo_windowgraphicdevice.hxx"
#include "cairo_linepolypolygon.hxx"
#include "cairo_parametricpolypolygon.hxx"
#include "cairo_canvasbitmap.hxx"

namespace cairo {

#ifndef CAIRO_XLIB_H
#include <cairo-xlib.h>
#endif

#ifdef CAIRO_HAS_GLITZ_SURFACE
#include <glitz.h>
#include <glitz-glx.h>
#include <cairo-glitz.h>
#endif

}

#include <canvas/canvastools.hxx>

#ifndef _SV_SYSDATA_HXX
#include <vcl/sysdata.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_ 
#include <osl/mutex.hxx>
#endif

#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif

#ifndef _COM_SUN_STAR_RENDERING_XLINEPOLYPOLYGON2D_HPP_
#include <com/sun/star/rendering/XLinePolyPolygon2D.hpp>
#endif

using namespace ::com::sun::star;


namespace vclcanvas
{
    WindowGraphicDevice::WindowGraphicDevice( Window& rOutputWindow, const SystemEnvData* pSysData ) :
        WindowGraphicDevice_Base( m_aMutex ),
        mpOutputWindow( &rOutputWindow ),
        mpWindowSurface( NULL )
    {
        CHECK_AND_THROW( mpOutputWindow != NULL,
                         "WindowGraphicDevice::WindowGraphicDevice(): pWindow is NULL" );
	mpSysData = pSysData;
    }

    WindowGraphicDevice::~WindowGraphicDevice()
    {
    }

    uno::Reference< rendering::XBufferController > SAL_CALL WindowGraphicDevice::getBufferController() throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return uno::Reference< rendering::XBufferController >();
    }

    uno::Reference< rendering::XColorSpace > SAL_CALL WindowGraphicDevice::getDeviceColorSpace() throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return uno::Reference< rendering::XColorSpace >();
    }

    geometry::RealSize2D SAL_CALL WindowGraphicDevice::getPhysicalResolution() throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( !mpOutputWindow )
            return ::canvas::tools::createInfiniteSize2D(); // we're disposed

        // Map a one-by-one millimeter box to pixel
        const MapMode aOldMapMode( mpOutputWindow->GetMapMode() );
        mpOutputWindow->SetMapMode( MapMode(MAP_MM) );
        const Size aPixelSize( mpOutputWindow->LogicToPixel(Size(1,1)) );
        mpOutputWindow->SetMapMode( aOldMapMode );

        return ::vcl::unotools::size2DFromSize( aPixelSize );
    }

    geometry::RealSize2D SAL_CALL WindowGraphicDevice::getSize() throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( !mpOutputWindow )
            return ::canvas::tools::createInfiniteSize2D(); // we're disposed

        // Map the pixel dimensions of the output window to millimeter
        const MapMode aOldMapMode( mpOutputWindow->GetMapMode() );
        mpOutputWindow->SetMapMode( MapMode(MAP_MM) );
        const Size aLogSize( mpOutputWindow->PixelToLogic(mpOutputWindow->GetOutputSizePixel()) );
        mpOutputWindow->SetMapMode( aOldMapMode );

        return ::vcl::unotools::size2DFromSize( aLogSize );
    }

    uno::Reference< rendering::XLinePolyPolygon2D > SAL_CALL WindowGraphicDevice::createCompatibleLinePolyPolygon( const uno::Sequence< uno::Sequence< geometry::RealPoint2D > >& points ) throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( !mpOutputWindow )
            return uno::Reference< rendering::XLinePolyPolygon2D >(); // we're disposed

        return uno::Reference< rendering::XLinePolyPolygon2D >( 
            new LinePolyPolygon( 
                ::basegfx::unotools::polyPolygonFromPoint2DSequenceSequence( points ) ) );
    }

    uno::Reference< rendering::XBezierPolyPolygon2D > SAL_CALL WindowGraphicDevice::createCompatibleBezierPolyPolygon( const uno::Sequence< uno::Sequence< geometry::RealBezierSegment2D > >& points ) throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( !mpOutputWindow )
            return uno::Reference< rendering::XBezierPolyPolygon2D >(); // we're disposed

        return uno::Reference< rendering::XBezierPolyPolygon2D >( 
            new LinePolyPolygon( 
                ::basegfx::unotools::polyPolygonFromBezier2DSequenceSequence( points ) ) );
    }

    uno::Reference< rendering::XBitmap > SAL_CALL WindowGraphicDevice::createCompatibleBitmap( const geometry::IntegerSize2D& size ) throw (lang::IllegalArgumentException,uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( !mpOutputWindow )
            return uno::Reference< rendering::XBitmap >(); // we're disposed

        return uno::Reference< rendering::XBitmap >( new CanvasBitmap( ::vcl::unotools::sizeFromIntegerSize2D(size),
                                                                       false,
                                                                       ImplRef(this) ) );
    }

    uno::Reference< rendering::XVolatileBitmap > SAL_CALL WindowGraphicDevice::createVolatileBitmap( const geometry::IntegerSize2D& size ) throw (lang::IllegalArgumentException,uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return uno::Reference< rendering::XVolatileBitmap >();
    }

    uno::Reference< rendering::XBitmap > SAL_CALL WindowGraphicDevice::createCompatibleAlphaBitmap( const geometry::IntegerSize2D& size ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( !mpOutputWindow )
            return uno::Reference< rendering::XBitmap >(); // we're disposed

        return uno::Reference< rendering::XBitmap >( new CanvasBitmap( ::vcl::unotools::sizeFromIntegerSize2D(size),
                                                                       true,
                                                                       ImplRef(this) ) );
    }

    uno::Reference< rendering::XVolatileBitmap > SAL_CALL WindowGraphicDevice::createVolatileAlphaBitmap( const geometry::IntegerSize2D& size ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return uno::Reference< rendering::XVolatileBitmap >();
    }

    uno::Reference< rendering::XParametricPolyPolygon2DFactory > SAL_CALL WindowGraphicDevice::getParametricPolyPolygonFactory() throw (uno::RuntimeException)
    {
        return uno::Reference< rendering::XParametricPolyPolygon2DFactory >( this );
    }

    sal_Bool SAL_CALL WindowGraphicDevice::hasFullScreenMode() throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return false;
    }

    sal_Bool SAL_CALL WindowGraphicDevice::enterFullScreenMode( sal_Bool bEnter ) throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return false;
    }

    // XParametricPolyPolygon2DFactory
    // =========================================================

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createLinearHorizontalGradient( const uno::Sequence< double >& leftColor, const uno::Sequence< double >& rightColor ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(P1): Could as well use static objects here,
        // XParametricPolyPolygon2D has no (externally visible)
        // state. OTOH, might as well leave that as it is, to allow
        // each XParametricPolyPolygon2D to hold a static
        // XPolyPolygon2D internally, and hand out the same
        // XPolyPolygon2D each time
        // XParametricPolyPolygon2D::getOutline() is called.
        return uno::Reference< rendering::XParametricPolyPolygon2D >( 
            ParametricPolyPolygon::createLinearHorizontalGradient( leftColor, 
                                                                   rightColor,
                                                                   this ) );
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createAxialHorizontalGradient( const uno::Sequence< double >& middleColor, const uno::Sequence< double >& endColor ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(P1): Could as well use static objects here,
        // XParametricPolyPolygon2D has no (externally visible)
        // state. OTOH, might as well leave that as it is, to allow
        // each XParametricPolyPolygon2D to hold a static
        // XPolyPolygon2D internally, and hand out the same
        // XPolyPolygon2D each time
        // XParametricPolyPolygon2D::getOutline() is called.
        return uno::Reference< rendering::XParametricPolyPolygon2D >( 
            ParametricPolyPolygon::createAxialHorizontalGradient( middleColor, 
                                                                  endColor,
                                                                  this ) );
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createEllipticalGradient( const uno::Sequence< double >& centerColor, const uno::Sequence< double >& endColor, const geometry::RealRectangle2D& boundRect ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(P1): Could as well use static objects here,
        // XParametricPolyPolygon2D has no (externally visible)
        // state. OTOH, might as well leave that as it is, to allow
        // each XParametricPolyPolygon2D to hold a static
        // XPolyPolygon2D internally, and hand out the same
        // XPolyPolygon2D each time
        // XParametricPolyPolygon2D::getOutline() is called.
        return uno::Reference< rendering::XParametricPolyPolygon2D >( 
            ParametricPolyPolygon::createEllipticalGradient( centerColor, 
                                                             endColor,
                                                             boundRect,
                                                             this ) );
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createRectangularGradient( const uno::Sequence< double >& centerColor, const uno::Sequence< double >& endColor, const geometry::RealRectangle2D& boundRect ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(P1): Could as well use static objects here,
        // XParametricPolyPolygon2D has no (externally visible)
        // state. OTOH, might as well leave that as it is, to allow
        // each XParametricPolyPolygon2D to hold a static
        // XPolyPolygon2D internally, and hand out the same
        // XPolyPolygon2D each time
        // XParametricPolyPolygon2D::getOutline() is called.
        return uno::Reference< rendering::XParametricPolyPolygon2D >( 
            ParametricPolyPolygon::createRectangularGradient( centerColor, 
                                                              endColor, 
                                                              boundRect,
                                                              this ) );
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createVerticalLinesHatch( const uno::Sequence< double >& leftColor, const uno::Sequence< double >& rightColor ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): hatch factory NYI
        return uno::Reference< rendering::XParametricPolyPolygon2D >();
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createOrthogonalLinesHatch( const uno::Sequence< double >& leftTopColor, const uno::Sequence< double >& rightBottomColor ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): hatch factory NYI
        return uno::Reference< rendering::XParametricPolyPolygon2D >();
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createThreeCrossingLinesHatch( const uno::Sequence< double >& startColor, const uno::Sequence< double >& endColor ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): hatch factory NYI
        return uno::Reference< rendering::XParametricPolyPolygon2D >();
    }

    uno::Reference< rendering::XParametricPolyPolygon2D > SAL_CALL WindowGraphicDevice::createFourCrossingLinesHatch( const uno::Sequence< double >& startColor, const uno::Sequence< double >& endColor ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): hatch factory NYI
        return uno::Reference< rendering::XParametricPolyPolygon2D >();
    }

    // XPropertySet
    // ==============================================

    uno::Reference< beans::XPropertySetInfo > SAL_CALL WindowGraphicDevice::getPropertySetInfo(  ) throw (uno::RuntimeException)
    {
        // This is a stealth property set
        return uno::Reference< beans::XPropertySetInfo >();
    }

    void SAL_CALL WindowGraphicDevice::setPropertyValue( const ::rtl::OUString& aPropertyName, const uno::Any& aValue ) throw (beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
    {
        // all our properties are read-only
        throw beans::PropertyVetoException();
    }

    uno::Any SAL_CALL WindowGraphicDevice::getPropertyValue( const ::rtl::OUString& PropertyName ) throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( PropertyName.equalsAscii( "DeviceHandle" ) )
            return uno::makeAny( reinterpret_cast< sal_Int64 >( mpOutputWindow ) );

        throw beans::UnknownPropertyException();
    }

    void SAL_CALL WindowGraphicDevice::addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& xListener ) throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( aPropertyName.equalsAscii( "DeviceHandle" ) )
            return; // ignore, we only have read-only properties, so
                    // the listener won't notice the difference

        throw beans::UnknownPropertyException();        
    }

    void SAL_CALL WindowGraphicDevice::removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& aListener ) throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( aPropertyName.equalsAscii( "DeviceHandle" ) )
            return; // ignore, we only have read-only properties, so
                    // the listener was not added in the first place

        throw beans::UnknownPropertyException();        
    }

    void SAL_CALL WindowGraphicDevice::addVetoableChangeListener( const ::rtl::OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener ) throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( PropertyName.equalsAscii( "DeviceHandle" ) )
            return; // ignore, we only have read-only properties, so
                    // the listener won't notice the difference

        throw beans::UnknownPropertyException();        
    }

    void SAL_CALL WindowGraphicDevice::removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener ) throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if( PropertyName.equalsAscii( "DeviceHandle" ) )
            return; // ignore, we only have read-only properties, so
                    // the listener was not added in the first place

        throw beans::UnknownPropertyException();        
    }

#define IMPLEMENTATION_NAME "CairoCanvas::WindowGraphicDevice"
#define SERVICE_NAME "com.sun.star.rendering.GraphicDevice"

    ::rtl::OUString SAL_CALL WindowGraphicDevice::getImplementationName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL WindowGraphicDevice::supportsService( const ::rtl::OUString& ServiceName ) throw (uno::RuntimeException)
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL WindowGraphicDevice::getSupportedServiceNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );

        return aRet;
    }
    
    void SAL_CALL WindowGraphicDevice::disposing()
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        // release all references
        mpOutputWindow = NULL;
    }
       
    OutputDevice* WindowGraphicDevice::getOutDev() const
    {
        return mpOutputWindow;
    }

    ::cairo::Surface* WindowGraphicDevice::getSurface( Size aSize )
    {
    	if( !mpWindowSurface ) {
#if 0
#ifdef CAIRO_HAS_GLITZ_SURFACE
	    ::cairo::glitz_surface_t* pGlitzSurface = (::cairo::glitz_surface_t*)
		  cairoHelperGetGlitzSurface( mpSysData,
					      mpOutputWindow->GetOutOffXPixel(), mpOutputWindow->GetOutOffYPixel(),
					      aSize.Width(), aSize.Height() );

	    if( pGlitzSurface )
		mpWindowSurface=::cairo::cairo_glitz_surface_create( pGlitzSurface );
	    printf ("pGlitzSurface %p surface %p\n", pGlitzSurface, mpWindowSurface);
#endif
#endif
	    if( !mpWindowSurface ) {
		mpWindowSurface=::cairo::cairo_xlib_surface_create( (::cairo::Display*) mpSysData->pDisplay,
								    mpSysData->aWindow,
								    (::cairo::Visual*) mpSysData->pVisual,
								    aSize.Width() + mpOutputWindow->GetOutOffXPixel(), aSize.Height() + mpOutputWindow->GetOutOffYPixel() );
	    }

	    ::cairo::cairo_surface_set_device_offset( mpWindowSurface, mpOutputWindow->GetOutOffXPixel(), mpOutputWindow->GetOutOffYPixel() );

	}
    	if( aSize == mpOutputWindow->GetOutputSizePixel() ) {
	    return mpWindowSurface;
    	}
	printf( "called WindowGraphicDevice::getSurface(size) - creating similar surface\n" );
	return ::cairo::cairo_surface_create_similar( mpWindowSurface, ::cairo::CAIRO_FORMAT_ARGB32, aSize.Width(), aSize.Height() );
    }

    Size WindowGraphicDevice::getSurfaceSize()
    {
	return mpOutputWindow->GetOutputSizePixel();
    }

    ::cairo::Surface* WindowGraphicDevice::getSurface()
    {
	printf( "called WindowGraphicDevice::getSurface %d x %d\n", getSurfaceSize().Width(), getSurfaceSize().Height() );
	return getSurface( getSurfaceSize() );
    }

    ::cairo::Surface* WindowGraphicDevice::getSimilarSurface( Size aSize )
    {
	printf( "called WindowGraphicDevice::getSimilarSurface %d x %d\n", aSize.Width(), aSize.Height() );
	return ::cairo::cairo_surface_create_similar( getSurface(), ::cairo::CAIRO_FORMAT_ARGB32, aSize.Width(), aSize.Height() );
    }

    ::cairo::Surface* WindowGraphicDevice::getSimilarSurface()
    {
	return getSimilarSurface( getSurfaceSize() );
    }
}
