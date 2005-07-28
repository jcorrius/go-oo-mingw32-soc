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

#ifndef _VCLCANVAS_PARAMETRICPOLYPOLYGON_HXX
#define _VCLCANVAS_PARAMETRICPOLYPOLYGON_HXX

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XGRAPHICDEVICE_HPP_
#include <com/sun/star/rendering/XGraphicDevice.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XPARAMETRICPOLYPOLYGON2DFACTORY_HPP_
#include <com/sun/star/rendering/XParametricPolyPolygon2DFactory.hpp>
#endif

#ifndef _CPPUHELPER_COMPBASE2_HXX_
#include <cppuhelper/compbase2.hxx>
#endif
#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

class OutputDevice;
class PolyPolygon;

namespace basegfx
{
    class B2DPolygon;
    class B2DHomMatrix;
}

#define PARAMETRICPOLYPOLYGON_IMPLEMENTATION_NAME "CairoCanvas::ParametricPolyPolygon"

/* Definition of ParametricPolyPolygon class */

namespace cairocanvas
{
    typedef ::cppu::WeakComponentImplHelper2< ::com::sun::star::rendering::XParametricPolyPolygon2D,
            		   			  		      ::com::sun::star::lang::XServiceInfo > ParametricPolyPolygon_Base;

    class ParametricPolyPolygon : public ::comphelper::OBaseMutex, public ParametricPolyPolygon_Base
    {
    public:

        static ParametricPolyPolygon* createLinearHorizontalGradient( const ::com::sun::star::uno::Sequence< double >& 			leftColor, 
                                                                      const ::com::sun::star::uno::Sequence< double >& 			rightColor,
                                                                      const ::com::sun::star::uno::Reference< 
                                                                      	::com::sun::star::rendering::XGraphicDevice >&  xDevice );
        static ParametricPolyPolygon* createAxialHorizontalGradient( const ::com::sun::star::uno::Sequence< double >& 			middleColor, 
                                                                     const ::com::sun::star::uno::Sequence< double >& 			endColor,
                                                                     const ::com::sun::star::uno::Reference< 
                                                                      	::com::sun::star::rendering::XGraphicDevice >&  xDevice );
        static ParametricPolyPolygon* createEllipticalGradient( const ::com::sun::star::uno::Sequence< double >& 				centerColor, 
                                                                const ::com::sun::star::uno::Sequence< double >& 				endColor,
                                                                const ::com::sun::star::geometry::RealRectangle2D&		boundRect,
                                                                const ::com::sun::star::uno::Reference< 
	                                                             	 ::com::sun::star::rendering::XGraphicDevice >& 	xDevice );
        static ParametricPolyPolygon* createRectangularGradient( const ::com::sun::star::uno::Sequence< double >& 			centerColor, 
                                                                 const ::com::sun::star::uno::Sequence< double >& 			endColor,
                                                                 const ::com::sun::star::geometry::RealRectangle2D&	boundRect,
                                                                 const ::com::sun::star::uno::Reference< 
	                                                             	 ::com::sun::star::rendering::XGraphicDevice >& xDevice );

        /// Dispose all internal references
        virtual void SAL_CALL disposing();

        // XParametricPolyPolygon2D
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D > SAL_CALL getOutline( double t ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< double > SAL_CALL getColor( double t ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< double > SAL_CALL getPointColor( const ::com::sun::star::geometry::RealPoint2D& point ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);

        bool fill( OutputDevice&											rOutDev,
                   OutputDevice*											p2ndOutDev,
                   const PolyPolygon& 										rPoly,
                   const ::com::sun::star::rendering::ViewState& 	viewState, 
                   const ::com::sun::star::rendering::RenderState&	renderState,
                   const ::com::sun::star::rendering::Texture&	    texture,
                   int 														nTransparency ) const;

    protected:
        ~ParametricPolyPolygon(); // we're a ref-counted UNO class. _We_ destroy ourselves.

    private:
        // default: disabled copy/assignment
        ParametricPolyPolygon(const ParametricPolyPolygon&);
        ParametricPolyPolygon& operator=( const ParametricPolyPolygon& );

        enum GradientType
        {
            GRADIENT_LINEAR,
            GRADIENT_AXIAL,
            GRADIENT_POLYGON
        };

        /// Private, because objects can only be created from the static factories
        ParametricPolyPolygon( const ::basegfx::B2DPolygon& rGradientPoly,
                               const ::Color& 				rColor1,
                               const ::Color& 				rColor2 );
        ParametricPolyPolygon( const ::basegfx::B2DPolygon& rGradientPoly,
                               const ::Color& 				rColor1,
                               const ::Color& 				rColor2,
                               double						nAspectRatio );
        ParametricPolyPolygon( GradientType	  eType,
                               const ::Color& rColor1,
                               const ::Color& rColor2 );

        void fillLinearGradient( OutputDevice&					rOutDev,
                                 const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                 const ::Rectangle&				rBounds,
                                 int							nStepCount,
                                 bool							bFillNonOverlapping ) const;

        void fillAxialGradient( OutputDevice&					rOutDev,
                                const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                const ::Rectangle&				rBounds,
                                int								nStepCount,
                                bool							bFillNonOverlapping ) const;

        void fillPolygonalGradient( OutputDevice&					rOutDev,
                                    const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                    const ::Rectangle&				rBounds,
                                    int								nStepCount,
                                    bool							bFillNonOverlapping ) const;

        void fill( OutputDevice&					rOutDev,
                   const ::basegfx::B2DHomMatrix&	rTextureTransform,
                   const ::Rectangle&				rBounds,
                   int								nStepCount,
                   bool								bFillNonOverlapping ) const;


        /// Polygon gradient shape
        const ::basegfx::B2DPolygon	maGradientPoly;

        /// Aspect ratio of gradient, affects scaling of innermost gradient polygon
        const double				mnAspectRatio;

        /// First gradient color 
        const ::Color				maColor1;

        /// Second gradient color
        const ::Color				maColor2;

        /// Type of gradient to render (as e.g. linear grads are not represented by maGradientPoly)
        const GradientType			meType;
    };
}

#endif /* _VCLCANVAS_PARAMETRICPOLYPOLYGON_HXX */
