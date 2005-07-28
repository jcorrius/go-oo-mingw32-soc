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

#ifndef  _USE_MATH_DEFINES
#define  _USE_MATH_DEFINES  // needed by Visual C++ for math constants
#endif
#include <math.h>           // M_PI definition 

#include <canvas/debug.hxx>
#include <canvas/canvastools.hxx>

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif 

#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif
#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_RANGE_B2DRECTANGLE_HXX
#include <basegfx/range/b2drectangle.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif
#ifndef _BGFX_TOOLS_TOOLS_HXX
#include <basegfx/tools/tools.hxx>
#endif

#include <limits>

#include "cairo_impltools.hxx"
#include "cairo_parametricpolypolygon.hxx"


using namespace ::com::sun::star;

namespace cairocanvas
{
    ParametricPolyPolygon* ParametricPolyPolygon::createLinearHorizontalGradient( const uno::Sequence< double >& 						leftColor, 
                                                                                  const uno::Sequence< double >& 						rightColor,
                                                                                  const uno::Reference< rendering::XGraphicDevice >& 	xDevice )
    {
        // TODO(P2): hold gradient brush statically, and only setup
        // the colors
        return new ParametricPolyPolygon( GRADIENT_LINEAR,
                                          ::vcl::unotools::sequenceToColor( xDevice,
                                                                            leftColor ),
                                          ::vcl::unotools::sequenceToColor( xDevice,
                                                                            rightColor ) );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createAxialHorizontalGradient( const uno::Sequence< double >& 					middleColor, 
                                                                                 const uno::Sequence< double >& 					endColor,
                                                                                 const uno::Reference< rendering::XGraphicDevice >& xDevice )
    {
        // TODO(P2): hold gradient brush statically, and only setup
        // the colors
        return new ParametricPolyPolygon( GRADIENT_AXIAL,
                                          ::vcl::unotools::sequenceToColor( xDevice,
                                                                            endColor ),
                                          ::vcl::unotools::sequenceToColor( xDevice,
                                                                            middleColor ) );
    }

    namespace
    {
        double calcAspectRatio( const geometry::RealRectangle2D& rBoundRect )
        {
            const double nWidth( rBoundRect.X2 - rBoundRect.X1 );
            const double nHeight( rBoundRect.Y2 - rBoundRect.Y1 );

            return ::basegfx::fTools::equalZero( nHeight ) ? 1.0 : fabs( nWidth / nHeight );
        }
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createEllipticalGradient( const uno::Sequence< double >& 						centerColor, 
                                                                            const uno::Sequence< double >& 						endColor,
                                                                            const geometry::RealRectangle2D&					boundRect,
                                                                            const uno::Reference< rendering::XGraphicDevice >& 	xDevice )
    {
        // TODO(P2): hold gradient polygon statically, and only setup
        // the colors
        return new ParametricPolyPolygon( 
            ::basegfx::tools::createPolygonFromCircle( 
                ::basegfx::B2DPoint( 0.5, 0.5), 0.5 ),
            ::vcl::unotools::sequenceToColor( xDevice,
                                              endColor ),
            ::vcl::unotools::sequenceToColor( xDevice,
                                              centerColor ),
            calcAspectRatio( boundRect ) );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createRectangularGradient( const uno::Sequence< double >& 					centerColor, 
                                                                             const uno::Sequence< double >& 					endColor,
                                                                             const geometry::RealRectangle2D&					boundRect,
                                                                             const uno::Reference< rendering::XGraphicDevice >& xDevice )
    {
        // TODO(P2): hold gradient polygon statically, and only setup
        // the colors
        return new ParametricPolyPolygon( 
            ::basegfx::tools::createPolygonFromRect( 
                ::basegfx::B2DRectangle( 0.0, 0.0, 1.0, 1.0 ) ),
            ::vcl::unotools::sequenceToColor( xDevice,
                                              endColor ),
            ::vcl::unotools::sequenceToColor( xDevice,
                                              centerColor ),
            calcAspectRatio( boundRect ) );
    }

    void SAL_CALL ParametricPolyPolygon::disposing()
    {
    }

    uno::Reference< rendering::XPolyPolygon2D > SAL_CALL ParametricPolyPolygon::getOutline( double t ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): outline NYI
        return uno::Reference< rendering::XPolyPolygon2D >();
    }

    uno::Sequence< double > SAL_CALL ParametricPolyPolygon::getColor( double t ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): color NYI
        return uno::Sequence< double >();
    }

    uno::Sequence< double > SAL_CALL ParametricPolyPolygon::getPointColor( const geometry::RealPoint2D& point ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): point color NYI
        return uno::Sequence< double >();
    }

#define SERVICE_NAME "com.sun.star.rendering.ParametricPolyPolygon"

    ::rtl::OUString SAL_CALL ParametricPolyPolygon::getImplementationName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( PARAMETRICPOLYPOLYGON_IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL ParametricPolyPolygon::supportsService( const ::rtl::OUString& ServiceName ) throw (uno::RuntimeException)
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL ParametricPolyPolygon::getSupportedServiceNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );

        return aRet;
    }

    namespace
    {
        /** Fill linear or axial gradient

        	Since most of the code for linear and axial gradients are
        	the same, we've a unified method here
         */
        void fillGeneralLinearGradient( OutputDevice&					rOutDev,
                                        const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                        const ::Rectangle&				rBounds,
                                        int								nStepCount,
                                        const ::Color& 					rColor1,
                                        const ::Color& 					rColor2,
                                        bool							bFillNonOverlapping,
                                        bool							bAxialGradient )
        {
            // determine general position of gradient in relation to
            // the bound rect
            // =====================================================

            ::basegfx::B2DPoint aLeftTop( 0.0, 0.0 );
            ::basegfx::B2DPoint aLeftBottom( 0.0, 1.0 );
            ::basegfx::B2DPoint aRightTop( 1.0, 0.0 );
            ::basegfx::B2DPoint aRightBottom( 1.0, 1.0 );

            aLeftTop	*= rTextureTransform;
            aLeftBottom *= rTextureTransform;
            aRightTop 	*= rTextureTransform;
            aRightBottom*= rTextureTransform;

            // calc length of bound rect diagonal
            const ::basegfx::B2DVector aBoundRectDiagonal( 
                ::vcl::unotools::b2DPointFromPoint( rBounds.TopLeft() ) -
                ::vcl::unotools::b2DPointFromPoint( rBounds.BottomRight() ) );
            const double nDiagonalLength( aBoundRectDiagonal.getLength() );

            // create direction of gradient:
            //     _______
            //     |  |  | 
            // ->  |  |  | ...
            //     |  |  | 
            //     -------            
            ::basegfx::B2DVector aDirection( aRightTop - aLeftTop );
            aDirection.normalize();

            // now, we potentially have to enlarge our gradient area
            // atop and below the transformed [0,1]x[0,1] unit rect,
            // for the gradient to fill the complete bound rect.
            ::basegfx::tools::infiniteLineFromParallelogram( aLeftTop,
                                                             aLeftBottom,
                                                             aRightTop,
                                                             aRightBottom,
                                                             ::vcl::unotools::b2DRectangleFromRectangle( rBounds ) );
            

            // render gradient
            // ===============

            // for linear gradients, it's easy to render
            // non-overlapping polygons: just split the gradient into
            // nStepCount small strips. Prepare the strip now.

            // For performance reasons, we create a temporary VCL
            // polygon here, keep it all the way and only change the
            // vertex values in the loop below (as ::Polygon is a
            // pimpl class, creating one every loop turn would really
            // stress the mem allocator)
            ::Polygon aTempPoly( static_cast<USHORT>(5) );

            OSL_ENSURE( nStepCount >= 3,
                        "fillLinearGradient(): stepcount smaller than 3" );

        
            // fill initial strip (extending two times the bound rect's
            // diagonal to the 'left'
            // ------------------------------------------------------

            // calculate left edge, by moving left edge of the
            // gradient rect two times the bound rect's diagonal to
            // the 'left'. Since we postpone actual rendering into the
            // loop below, we set the _right_ edge here, which will be
            // readily copied into the left edge in the loop below
            const ::basegfx::B2DPoint& rPoint1( aLeftTop - 2.0*nDiagonalLength*aDirection );
            aTempPoly[1] = ::Point( ::basegfx::fround( rPoint1.getX() ),
                                    ::basegfx::fround( rPoint1.getY() ) );

            const ::basegfx::B2DPoint& rPoint2( aLeftBottom - 2.0*nDiagonalLength*aDirection );
            aTempPoly[2] = ::Point( ::basegfx::fround( rPoint2.getX() ),
                                    ::basegfx::fround( rPoint2.getY() ) );


            // iteratively render all other strips
            // -----------------------------------
            
            // ensure that nStepCount is odd, to have a well-defined
            // middle index for axial gradients.
            if( bAxialGradient && !(nStepCount % 2) )
                ++nStepCount;

            const int nStepCountHalved( nStepCount / 2 );

            // only iterate nStepCount-1 steps, as the last strip is
            // explicitely painted below
            for( int i=0; i<nStepCount-1; ++i )
            {
                // lerp color
                if( bAxialGradient )
                {
                    // axial gradient has a triangle-like interpolation function
                    const int iPrime( i<=nStepCountHalved ? i : nStepCount-i-1);

                    rOutDev.SetFillColor( 
                        Color( ((nStepCountHalved - iPrime)*rColor1.GetRed() + iPrime*rColor2.GetRed())/nStepCountHalved,
                               ((nStepCountHalved - iPrime)*rColor1.GetGreen() + iPrime*rColor2.GetGreen())/nStepCountHalved,
                               ((nStepCountHalved - iPrime)*rColor1.GetBlue() + iPrime*rColor2.GetBlue())/nStepCountHalved ) );
                }
                else
                {
                    // linear gradient has a plain lerp between start and end color
                    rOutDev.SetFillColor( 
                        Color( ((nStepCount - i)*rColor1.GetRed() + i*rColor2.GetRed())/nStepCount,
                               ((nStepCount - i)*rColor1.GetGreen() + i*rColor2.GetGreen())/nStepCount,
                               ((nStepCount - i)*rColor1.GetBlue() + i*rColor2.GetBlue())/nStepCount ) );
                }

                // copy right egde of polygon to left edge (and also
                // copy the closing point)
                aTempPoly[0] = aTempPoly[4] = aTempPoly[1];
                aTempPoly[3] = aTempPoly[2];

                // calculate new right edge, from interpolating
                // between start and end line. Note that i is
                // increased by one, to account for the fact that we
                // calculate the right border here (whereas the fill
                // color is governed by the left edge)
                const ::basegfx::B2DPoint& rPoint1( 
                    (nStepCount - i-1)/(double)nStepCount*aLeftTop + 
                    (i+1)/(double)nStepCount*aRightTop );
                aTempPoly[1] = ::Point( ::basegfx::fround( rPoint1.getX() ),
                                        ::basegfx::fround( rPoint1.getY() ) );

                const ::basegfx::B2DPoint& rPoint2( 
                    (nStepCount - i-1)/(double)nStepCount*aLeftBottom + 
                    (i+1)/(double)nStepCount*aRightBottom );
                aTempPoly[2] = ::Point( ::basegfx::fround( rPoint2.getX() ),
                                        ::basegfx::fround( rPoint2.getY() ) );
                
                rOutDev.DrawPolygon( aTempPoly );
            }

            // fill final strip (extending two times the bound rect's
            // diagonal to the 'right'
            // ------------------------------------------------------

            // copy right egde of polygon to left edge (and also
            // copy the closing point)
            aTempPoly[0] = aTempPoly[4] = aTempPoly[1];
            aTempPoly[3] = aTempPoly[2];

            // calculate new right edge, by moving right edge of the
            // gradient rect two times the bound rect's diagonal to
            // the 'right'.
            const ::basegfx::B2DPoint& rPoint3( aRightTop + 2.0*nDiagonalLength*aDirection );
            aTempPoly[0] = aTempPoly[4] = ::Point( ::basegfx::fround( rPoint3.getX() ),
                                                   ::basegfx::fround( rPoint3.getY() ) );

            const ::basegfx::B2DPoint& rPoint4( aRightBottom + 2.0*nDiagonalLength*aDirection );
            aTempPoly[3] = ::Point( ::basegfx::fround( rPoint4.getX() ),
                                    ::basegfx::fround( rPoint4.getY() ) );

            if( bAxialGradient )
                rOutDev.SetFillColor( rColor1 );
            else
                rOutDev.SetFillColor( rColor2 );

            rOutDev.DrawPolygon( aTempPoly );
        }
    }

	void ParametricPolyPolygon::fillLinearGradient( OutputDevice&					rOutDev,
                                                    const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                                    const ::Rectangle&				rBounds,
                                                    int								nStepCount,
                                                    bool							bFillNonOverlapping ) const
	{
        fillGeneralLinearGradient( rOutDev,
                                   rTextureTransform,
                                   rBounds,
                                   nStepCount,
                                   maColor1,
                                   maColor2,
                                   bFillNonOverlapping,
                                   false );
    }

	void ParametricPolyPolygon::fillAxialGradient( OutputDevice&					rOutDev,
                                                   const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                                   const ::Rectangle&				rBounds,
                                                   int								nStepCount,
                                                   bool								bFillNonOverlapping ) const
	{
        fillGeneralLinearGradient( rOutDev,
                                   rTextureTransform,
                                   rBounds,
                                   nStepCount,
                                   maColor1,
                                   maColor2,
                                   bFillNonOverlapping,
                                   true );
    }

	void ParametricPolyPolygon::fillPolygonalGradient( OutputDevice&					rOutDev,
                                                       const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                                       const ::Rectangle&				rBounds,
                                                       int								nStepCount,
                                                       bool								bFillNonOverlapping ) const
	{
        ENSURE_AND_THROW( maGradientPoly.count() > 2,
                          "ParametricPolyPolygon::fillPolygonalGradient(): polygon without area given" );

        // For performance reasons, we create a temporary VCL polygon
        // here, keep it all the way and only change the vertex values
        // in the loop below (as ::Polygon is a pimpl class, creating
        // one every loop turn would really stress the mem allocator)
        ::basegfx::B2DPolygon 	aOuterPoly( maGradientPoly );
        ::basegfx::B2DPolygon 	aInnerPoly;

        // subdivide polygon _before_ rendering, would otherwise have
        // to be performed on every loop turn.
        if( aOuterPoly.areControlPointsUsed() )
            aOuterPoly = ::basegfx::tools::adaptiveSubdivideByAngle(aOuterPoly);

        aInnerPoly = aOuterPoly;
        
        // only transform outer polygon _after_ copying it into
        // aInnerPoly, because inner polygon has to be scaled before
        // the actual texture transformation takes place
        aOuterPoly.transform( rTextureTransform );

        // determine overall transformation for inner polygon (might
        // have to be prefixed by anisotrophic scaling)
        ::basegfx::B2DHomMatrix aInnerPolygonTransformMatrix;

        
        // apply scaling (possibly anisotrophic) to inner polygon
        // ------------------------------------------------------

        // move center of scaling to origin
        aInnerPolygonTransformMatrix.translate( -0.5, -0.5 );
        
        // scale inner polygon according to aspect ratio: for
        // wider-than-tall bounds (nAspectRatio > 1.0), the inner
        // polygon, representing the gradient focus, must have
        // non-zero width. Specifically, a bound rect twice as wide as
        // tall has a focus polygon of half it's width.
        if( mnAspectRatio > 1.0 )
        {
            // width > height case
            aInnerPolygonTransformMatrix.scale( 1.0 - 1.0/mnAspectRatio,
                                                0.0 );
        }
        else if( mnAspectRatio < 1.0 )
        {
            // width < height case
            aInnerPolygonTransformMatrix.scale( 0.0, 
                                                1.0 - mnAspectRatio );
        }
        else
        {
            // isotrophic case
            aInnerPolygonTransformMatrix.scale( 0.0, 0.0 );
        }

        // move origin back to former center of polygon
        aInnerPolygonTransformMatrix.translate( 0.5, 0.5 );

        // and finally, add texture transform to it.
        aInnerPolygonTransformMatrix *= rTextureTransform;

        // apply final matrix to polygon
        aInnerPoly.transform( aInnerPolygonTransformMatrix );
        

        const sal_Int32			nNumPoints( aOuterPoly.count() );
        ::Polygon				aTempPoly( static_cast<USHORT>(nNumPoints+1) );
        
        // increase number of steps by one: polygonal gradients have
        // the outermost polygon rendered in maColor2, and the
        // innermost in maColor1. The innermost polygon will never
        // have zero area, thus, we must divide the interval into
        // nStepCount+1 steps. For example, to create 3 steps:
        //
        // |                       |
        // |-------|-------|-------|
        // |                       |
        // 3       2       1       0 
        //
        // This yields 4 tick marks, where 0 is never attained (since
        // zero-area polygons typically don't display perceivable
        // color).
        ++nStepCount;

        if( !bFillNonOverlapping )
        {
            // fill background
            rOutDev.SetFillColor( maColor1 );
            rOutDev.DrawRect( rBounds );

            // render polygon
            // ==============

            for( int i=1,p; i<nStepCount; ++i )
            {
                // lerp color
                rOutDev.SetFillColor( 
                    Color( ((nStepCount - i)*maColor1.GetRed() + i*maColor2.GetRed())/nStepCount,
                           ((nStepCount - i)*maColor1.GetGreen() + i*maColor2.GetGreen())/nStepCount,
                           ((nStepCount - i)*maColor1.GetBlue() + i*maColor2.GetBlue())/nStepCount ) );

                // scale and render polygon, by interpolating between
                // outer and inner polygon. 

                // calc interpolation parameter in [0,1] range
                const double nT( (nStepCount-i)/(double)nStepCount );
            
                for( p=0; p<nNumPoints; ++p )
                {
                    const ::basegfx::B2DPoint& rOuterPoint( aOuterPoly.getB2DPoint(p) );
                    const ::basegfx::B2DPoint& rInnerPoint( aInnerPoly.getB2DPoint(p) );

                    aTempPoly[p] = ::Point( 
                        ::basegfx::fround( (1.0-nT)*rInnerPoint.getX() + nT*rOuterPoint.getX() ),
                        ::basegfx::fround( (1.0-nT)*rInnerPoint.getY() + nT*rOuterPoint.getY() ) );
                }

                // close polygon explicitely
                aTempPoly[p] = aTempPoly[0];

                // TODO(P1): compare with vcl/source/gdi/outdev4.cxx,
                // OutputDevice::ImplDrawComplexGradient(), there's a note
                // that on some VDev's, rendering disjunct poly-polygons
                // is faster!
                rOutDev.DrawPolygon( aTempPoly );
            }
        }
        else
        {
            // render polygon
            // ==============

            // For performance reasons, we create a temporary VCL polygon
            // here, keep it all the way and only change the vertex values
            // in the loop below (as ::Polygon is a pimpl class, creating
            // one every loop turn would really stress the mem allocator)
            ::PolyPolygon			aTempPolyPoly;
            ::Polygon				aTempPoly2( static_cast<USHORT>(nNumPoints+1) );

            aTempPoly2[0] = rBounds.TopLeft();
            aTempPoly2[1] = rBounds.TopRight();
            aTempPoly2[2] = rBounds.BottomRight();
            aTempPoly2[3] = rBounds.BottomLeft();
            aTempPoly2[4] = rBounds.TopLeft();

            aTempPolyPoly.Insert( aTempPoly );
            aTempPolyPoly.Insert( aTempPoly2 );

            for( int i=0,p; i<nStepCount; ++i )
            {
                // lerp color
                rOutDev.SetFillColor( 
                    Color( ((nStepCount - i)*maColor1.GetRed() + i*maColor2.GetRed())/nStepCount,
                           ((nStepCount - i)*maColor1.GetGreen() + i*maColor2.GetGreen())/nStepCount,
                           ((nStepCount - i)*maColor1.GetBlue() + i*maColor2.GetBlue())/nStepCount ) );

#if defined(VERBOSE) && defined(DBG_UTIL)        
                if( i && !(i % 10) )
                    rOutDev.SetFillColor( COL_RED );
#endif

                // scale and render polygon. Note that here, we
                // calculate the inner polygon, which is actually the
                // start of the _next_ color strip. Thus, i+1

                // calc interpolation parameter in [0,1] range
                const double nT( (nStepCount-i-1)/(double)nStepCount );
            
                for( p=0; p<nNumPoints; ++p )
                {
                    const ::basegfx::B2DPoint& rOuterPoint( aOuterPoly.getB2DPoint(p) );
                    const ::basegfx::B2DPoint& rInnerPoint( aInnerPoly.getB2DPoint(p) );

                    aTempPoly[p] = ::Point( 
                        ::basegfx::fround( (1.0-nT)*rInnerPoint.getX() + nT*rOuterPoint.getX() ),
                        ::basegfx::fround( (1.0-nT)*rInnerPoint.getY() + nT*rOuterPoint.getY() ) );
                }

                // close polygon explicitely
                aTempPoly[p] = aTempPoly[0];

                // swap inner and outer polygon
                aTempPolyPoly.Replace( aTempPolyPoly.GetObject( 1 ), 0 );

                if( i+1<nStepCount )
                {
                    // assign new inner polygon. Note that with this
                    // formulation, the internal pimpl objects for both
                    // temp polygons and the polypolygon remain identical,
                    // minimizing heap accesses (only a Polygon wrapper
                    // object is freed and deleted twice during this swap).
                    aTempPolyPoly.Replace( aTempPoly, 1 );
                }
                else
                {
                    // last, i.e. inner strip. Now, the inner polygon
                    // has zero area anyway, and to not leave holes in
                    // the gradient, finally render a simple polygon:
                    aTempPolyPoly.Remove( 1 );
                }

                rOutDev.DrawPolyPolygon( aTempPolyPoly );
            }
        }
    }

    void ParametricPolyPolygon::fill( OutputDevice&						rOutDev,
                                      const ::basegfx::B2DHomMatrix&	rTextureTransform,
                                      const ::Rectangle&				rBounds,
                                      int								nStepCount,
                                      bool								bFillNonOverlapping ) const
	{
        switch( meType )
        {
            case GRADIENT_LINEAR:
                fillLinearGradient( rOutDev,
                                    rTextureTransform,
                                    rBounds,
                                    nStepCount,
                                    bFillNonOverlapping );
                break;
               
            case GRADIENT_AXIAL:
                fillAxialGradient( rOutDev,
                                   rTextureTransform,
                                   rBounds,
                                   nStepCount,
                                   bFillNonOverlapping );
                break;

            case GRADIENT_POLYGON:
                fillPolygonalGradient( rOutDev,
                                       rTextureTransform,
                                       rBounds,
                                       nStepCount,
                                       bFillNonOverlapping );
                break;

            default:
                ENSURE_AND_THROW( false,
                                  "ParametricPolyPolygon::fill(): Unexpected case" );
        }
    }

    bool ParametricPolyPolygon::fill( OutputDevice&					rOutDev,
                                      OutputDevice*					p2ndOutDev,
                                      const PolyPolygon& 			rPoly,
                                      const rendering::ViewState& 	viewState, 
                                      const rendering::RenderState&	renderState,
                                      const rendering::Texture& 	texture,
                                      int 							nTransparency ) const
    {
        // TODO(T2): It is maybe necessary to lock here, should
        // maGradientPoly someday cease to be const. But then, beware of
        // deadlocks, canvashelper calls this method with locked own
        // mutex.

        // calculate overall texture transformation (directly from
        // texture to device space).
        ::basegfx::B2DHomMatrix aMatrix;
        ::basegfx::B2DHomMatrix aTextureTransform;

        ::basegfx::unotools::homMatrixFromAffineMatrix( aTextureTransform, 
                                                        texture.AffineTransform );
        ::canvas::tools::mergeViewAndRenderTransform(aMatrix,
                                                     viewState,
                                                     renderState);
        aTextureTransform *= aMatrix; // prepend total view/render transformation

        // determine maximal bound rect of gradient-filled polygon
        const ::Rectangle aPolygonDeviceRectOrig( 
            rPoly.GetBoundRect() );

        // determine size of gradient in device coordinate system
        // (to e.g. determine sensible number of gradient steps)
        ::basegfx::B2DPoint aLeftTop( 0.0, 0.0 );
        ::basegfx::B2DPoint aLeftBottom( 0.0, 1.0 );
        ::basegfx::B2DPoint aRightTop( 1.0, 0.0 );
        ::basegfx::B2DPoint aRightBottom( 1.0, 1.0 );

        aLeftTop	*= aTextureTransform;
        aLeftBottom *= aTextureTransform;
        aRightTop 	*= aTextureTransform;
        aRightBottom*= aTextureTransform;


        // calc step size
        // --------------
        const int nColorSteps( 
            ::std::max( 
                labs( maColor1.GetRed() - maColor2.GetRed() ),
                ::std::max(                    
                    labs( maColor1.GetGreen() - maColor2.GetGreen() ),
                    labs( maColor1.GetBlue() - maColor2.GetBlue() ) ) ) );

        // longest line in gradient bound rect
        const int nGradientSize( 
            static_cast<int>( 
                ::std::max( 
                    ::basegfx::B2DVector(aRightBottom-aLeftTop).getLength(),
                    ::basegfx::B2DVector(aRightTop-aLeftBottom).getLength() ) + 1.0 ) );

        // typical number for pixel of the same color (strip size)
        const int nStripSize( nGradientSize < 50 ? 2 : 4 );

        // use at least three steps, and at utmost the number of color
        // steps
        const int nStepCount( 
            ::std::max( 
                3,
                ::std::min( 
                    nGradientSize / nStripSize,
                    nColorSteps ) ) );        

        rOutDev.SetLineColor(); 

        if( tools::isPolyPolygonEqualRectangle( rPoly, 
                                                aPolygonDeviceRectOrig ) )
        {
            // use optimized output path
            // -------------------------

            // this distinction really looks like a
            // micro-optimisation, but in fact greatly speeds up
            // especially complex gradients. That's because when using
            // clipping, we can output polygons instead of
            // poly-polygons, and don't have to output the gradient
            // twice for XOR

            rOutDev.Push( PUSH_CLIPREGION );
            rOutDev.IntersectClipRegion( aPolygonDeviceRectOrig );
            fill( rOutDev,
                  aTextureTransform,
                  aPolygonDeviceRectOrig,
                  nStepCount,
                  false );
            rOutDev.Pop();

            if( p2ndOutDev )
            {
                p2ndOutDev->Push( PUSH_CLIPREGION );
                p2ndOutDev->IntersectClipRegion( aPolygonDeviceRectOrig );
                fill( *p2ndOutDev,
                      aTextureTransform,
                      aPolygonDeviceRectOrig,
                      nStepCount,
                      false );
                p2ndOutDev->Pop();
            }
        }
        else
        {
            // output gradient the hard way: XORing out the polygon
            rOutDev.Push( PUSH_RASTEROP );
            rOutDev.SetRasterOp( ROP_XOR );
            fill( rOutDev,
                  aTextureTransform,
                  aPolygonDeviceRectOrig,
                  nStepCount,
                  true );
            rOutDev.SetFillColor( COL_BLACK );
            rOutDev.SetRasterOp( ROP_0 );
            rOutDev.DrawPolyPolygon( rPoly );
            rOutDev.SetRasterOp( ROP_XOR );
            fill( rOutDev,
                  aTextureTransform,
                  aPolygonDeviceRectOrig,
                  nStepCount,
                  true );
            rOutDev.Pop();

            if( p2ndOutDev )
            {
                p2ndOutDev->Push( PUSH_RASTEROP );
                p2ndOutDev->SetRasterOp( ROP_XOR );
                fill( *p2ndOutDev,
                      aTextureTransform,
                      aPolygonDeviceRectOrig,
                      nStepCount,
                      true );
                p2ndOutDev->SetFillColor( COL_BLACK );
                p2ndOutDev->SetRasterOp( ROP_0 );
                p2ndOutDev->DrawPolyPolygon( rPoly );
                p2ndOutDev->SetRasterOp( ROP_XOR );
                fill( *p2ndOutDev,
                      aTextureTransform,
                      aPolygonDeviceRectOrig,
                      nStepCount,
                      true );
                p2ndOutDev->Pop();
            }
        }

#if defined(VERBOSE) && defined(DBG_UTIL)        
        {
            ::basegfx::B2DRectangle aRect(0.0, 0.0, 1.0, 1.0);
            ::basegfx::B2DRectangle aTextureDeviceRect;
            ::canvas::tools::calcTransformedRectBounds( aTextureDeviceRect, 
                                                        aRect, 
                                                        aTextureTransform );
            rOutDev.SetLineColor( COL_RED ); 
            rOutDev.SetFillColor();
            rOutDev.DrawRect( ::vcl::unotools::rectangleFromB2DRectangle( aTextureDeviceRect ) );

            rOutDev.SetLineColor( COL_BLUE ); 
            ::basegfx::B2DPolygon aPoly( 
                ::Polygon( 
                    ::vcl::unotools::rectangleFromB2DRectangle( 
                        aRect ) ).getB2DPolygon() );
            aPoly.transform( aTextureTransform );
            ::Polygon aPoly2( aPoly );
            rOutDev.DrawPolygon( aPoly2 );
        }
#endif

        return true;
    }

    ParametricPolyPolygon::~ParametricPolyPolygon()
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( const ::basegfx::B2DPolygon&	rGradientPoly,
                                                  const ::Color& 				rColor1,
                                                  const ::Color& 				rColor2 ) :
        ParametricPolyPolygon_Base( m_aMutex ),        
        maGradientPoly( rGradientPoly ),
        mnAspectRatio( 1.0 ),
        maColor1( rColor1 ),
        maColor2( rColor2 ),
        meType( GRADIENT_POLYGON )
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( const ::basegfx::B2DPolygon&	rGradientPoly,
                                                  const ::Color& 				rColor1,
                                                  const ::Color& 				rColor2,
                                                  double						nAspectRatio ) :
        ParametricPolyPolygon_Base( m_aMutex ),        
        maGradientPoly( rGradientPoly ),
        mnAspectRatio( nAspectRatio ),
        maColor1( rColor1 ),
        maColor2( rColor2 ),
        meType( GRADIENT_POLYGON )
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( GradientType	 eType,
                                                  const ::Color& rColor1,
                                                  const ::Color& rColor2 ) :
        ParametricPolyPolygon_Base( m_aMutex ),        
        maGradientPoly(),
        mnAspectRatio( 1.0 ),
        maColor1( rColor1 ),
        maColor2( rColor2 ),
        meType( eType )
    {
    }

}
