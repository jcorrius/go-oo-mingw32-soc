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

#ifndef _VCLCANVAS_WINDOWGRAPHICDEVICE_HXX
#define _VCLCANVAS_WINDOWGRAPHICDEVICE_HXX

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XGRAPHICDEVICE_HPP_
#include <com/sun/star/rendering/XGraphicDevice.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XPARAMETRICPOLYPOLYGON2DFACTORY_HPP_
#include <com/sun/star/rendering/XParametricPolyPolygon2DFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XCANVAS_HPP_
#include <com/sun/star/rendering/XCanvas.hpp>
#endif

#ifndef _CPPUHELPER_COMPBASE4_HXX_
#include <cppuhelper/compbase4.hxx>
#endif
#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif

#ifndef _COMPHELPER_IMPLEMENTATIONREFERENCE_HXX
#include <comphelper/implementationreference.hxx>
#endif

#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif
#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif

#include "cairo_cairo.hxx"

namespace cairo {

// including glitz here conflicts with boost headers
// #ifdef CAIRO_HAS_GLITZ_SURFACE
// #include <glitz.h>
// #endif

    typedef struct _glitz_surface glitz_surface_t;
    typedef struct _glitz_drawable glitz_drawable_t;
}

/* Definition of WindowGraphicDevice class */

namespace cairocanvas
{
    typedef ::cppu::WeakComponentImplHelper4< ::com::sun::star::rendering::XGraphicDevice,
                                              ::com::sun::star::rendering::XParametricPolyPolygon2DFactory,
                                              ::com::sun::star::beans::XPropertySet,
            		   			  		      ::com::sun::star::lang::XServiceInfo > WindowGraphicDevice_Base;

    class WindowGraphicDevice : public ::comphelper::OBaseMutex, public WindowGraphicDevice_Base
    {
    public:

		typedef ::comphelper::ImplementationReference< WindowGraphicDevice, 
                                                       ::com::sun::star::rendering::XGraphicDevice > ImplRef;
 
        /** Create an XWindowGraphicDevice for given VCL window
         */
        WindowGraphicDevice( Window& rOutputWindow, const SystemEnvData* pSysData );

        /// Dispose all internal references
        virtual void SAL_CALL disposing();

        // XWindowGraphicDevice
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBufferController > SAL_CALL getBufferController(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XColorSpace > SAL_CALL getDeviceColorSpace(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::geometry::RealSize2D SAL_CALL getPhysicalResolution(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::geometry::RealSize2D SAL_CALL getSize(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XLinePolyPolygon2D > SAL_CALL createCompatibleLinePolyPolygon( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::geometry::RealPoint2D > >& points ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBezierPolyPolygon2D > SAL_CALL createCompatibleBezierPolyPolygon( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::geometry::RealBezierSegment2D > >& points ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmap > SAL_CALL createCompatibleBitmap( const ::com::sun::star::geometry::IntegerSize2D& size ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XVolatileBitmap > SAL_CALL createVolatileBitmap( const ::com::sun::star::geometry::IntegerSize2D& size ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmap > SAL_CALL createCompatibleAlphaBitmap( const ::com::sun::star::geometry::IntegerSize2D& size ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XVolatileBitmap > SAL_CALL createVolatileAlphaBitmap( const ::com::sun::star::geometry::IntegerSize2D& size ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2DFactory > SAL_CALL getParametricPolyPolygonFactory(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasFullScreenMode(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL enterFullScreenMode( sal_Bool bEnter ) throw (::com::sun::star::uno::RuntimeException);

        // XParametricPolyPolygon2DFactory
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createLinearHorizontalGradient( const ::com::sun::star::uno::Sequence< double >& leftColor, const ::com::sun::star::uno::Sequence< double >& rightColor ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createAxialHorizontalGradient( const ::com::sun::star::uno::Sequence< double >& middleColor, const ::com::sun::star::uno::Sequence< double >& endColor ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createEllipticalGradient( const ::com::sun::star::uno::Sequence< double >& centerColor, const ::com::sun::star::uno::Sequence< double >& endColor, const ::com::sun::star::geometry::RealRectangle2D& boundRect ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createRectangularGradient( const ::com::sun::star::uno::Sequence< double >& centerColor, const ::com::sun::star::uno::Sequence< double >& endColor, const ::com::sun::star::geometry::RealRectangle2D& boundRect ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createVerticalLinesHatch( const ::com::sun::star::uno::Sequence< double >& leftColor, const ::com::sun::star::uno::Sequence< double >& rightColor ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createOrthogonalLinesHatch( const ::com::sun::star::uno::Sequence< double >& leftTopColor, const ::com::sun::star::uno::Sequence< double >& rightBottomColor ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createThreeCrossingLinesHatch( const ::com::sun::star::uno::Sequence< double >& startColor, const ::com::sun::star::uno::Sequence< double >& endColor ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XParametricPolyPolygon2D > SAL_CALL createFourCrossingLinesHatch( const ::com::sun::star::uno::Sequence< double >& startColor, const ::com::sun::star::uno::Sequence< double >& endColor ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

        // XPropertySet
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);

        OutputDevice* getOutDev() const;

	Size getSurfaceSize() const;
        ::cairo::Surface* getSurface();
	::cairo::Surface* getSurface( Bitmap& rBitmap );

        ::cairo::Surface* getSimilarSurface( ::cairo::Content aContent = ::cairo::CAIRO_CONTENT_COLOR_ALPHA ) const;
        ::cairo::Surface* getSimilarSurface( Size aSize, ::cairo::Content aContent = ::cairo::CAIRO_CONTENT_COLOR_ALPHA ) const;

	void flush();

    protected:
        ~WindowGraphicDevice(); // we're a ref-counted UNO class. _We_ destroy ourselves.

    private:
        // default: disabled copy/assignment
        WindowGraphicDevice(const WindowGraphicDevice&);
        WindowGraphicDevice& operator=( const WindowGraphicDevice& );

        // TODO(Q3): Lifetime issue. Cannot control pointer validity
        // over object lifetime, since we're a UNO component. Now that
        // we've changed the ::Window canvas reference to a weak ref,
        // might be okay to hold a uno::Reference to the VCL window
        // here.
        Window* mpOutputWindow;

        const SystemEnvData* mpSysData;
        ::cairo::Surface* mpWindowSurface;
    };
}

#endif /* _VCLCANVAS_WINDOWGRAPHICDEVICE_HXX */
