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

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif 

#ifndef _COM_SUN_STAR_RENDERING_TEXTDIRECTION_HPP__
#include <com/sun/star/rendering/TextDirection.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_TEXTURINGMODE_HPP_
#include <com/sun/star/rendering/TexturingMode.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_PATHCAPTYPE_HPP_
#include <com/sun/star/rendering/PathCapType.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_PATHJOINTYPE_HPP_
#include <com/sun/star/rendering/PathJoinType.hpp>
#endif

#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif
#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif
#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_RANGE_B2DRECTANGLE_HXX
#include <basegfx/range/b2drectangle.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_VECTOR_B2DSIZE_HXX
#include <basegfx/vector/b2dsize.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DLINEGEOMETRY_HXX
#include <basegfx/polygon/b2dlinegeometry.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#include <utility>

#include <comphelper/sequence.hxx>
#include <canvas/canvastools.hxx>

#include "cairo_textlayout.hxx"
#include "cairo_parametricpolypolygon.hxx"
#include "cairo_canvashelper.hxx"
#include "cairo_canvasbitmap.hxx"
#include "cairo_impltools.hxx"
#include "cairo_canvasfont.hxx"
#include "cairo_linepolypolygon.hxx"

using namespace ::com::sun::star;
using namespace ::cairo;

namespace cairocanvas
{
    namespace
    {
        ::basegfx::tools::B2DLineJoin b2DJoineFromJoin( sal_Int8 nJoinType )
        {
            switch( nJoinType )
            {                
                case rendering::PathJoinType::NONE:
                    return ::basegfx::tools::B2DLINEJOIN_NONE;

                case rendering::PathJoinType::MITER:
                    return ::basegfx::tools::B2DLINEJOIN_MITER;

                case rendering::PathJoinType::ROUND:
                    return ::basegfx::tools::B2DLINEJOIN_ROUND;

                case rendering::PathJoinType::BEVEL:
                    return ::basegfx::tools::B2DLINEJOIN_BEVEL;

                default:
                    ENSURE_AND_THROW( false,
                                      "b2DJoineFromJoin(): Unexpected join type" );
            }

            return ::basegfx::tools::B2DLINEJOIN_NONE;
        }
    }

    CanvasHelper::CanvasHelper() :
        mxDevice(),
	mpCairo( NULL ),
	mpTextures( NULL ),
        mp2ndOutDev()
    {
    }

    void CanvasHelper::disposing()
    {
	OSL_TRACE("canvas helper disposing %p\n", this);
        mxDevice.reset();
        mp2ndOutDev.reset();
 	if( mpCairo )
 	    cairo_destroy( mpCairo );
 	mpCairo = NULL;
    }

    void CanvasHelper::setGraphicDevice( const WindowGraphicDevice::ImplRef& rDevice )
    {
        mxDevice = rDevice;
    }

    void CanvasHelper::setCairo( Cairo* pCairo )
    {
	OSL_TRACE("set canvas helper %p Cairo to %p", this, pCairo);
	if( pCairo != mpCairo ) {
	    if( pCairo )
		cairo_reference( pCairo );
	    if( mpCairo )
		cairo_destroy( mpCairo );
	    mpCairo = pCairo;
	}
    }

    void CanvasHelper::setBackgroundOutDev( const OutDevProviderSharedPtr& rOutDev )
    {
        mp2ndOutDev = rOutDev;
    }

    void CanvasHelper::drawPoint( const rendering::XCanvas& 	rCanvas, 
                                  const geometry::RealPoint2D& 	aPoint,
                                  const rendering::ViewState& 	viewState,
                                  const rendering::RenderState&	renderState )
    {
	// rodo TODO
//         // are we disposed?
//         if( mpCairo )
//         {
//             // nope, render
//             setupOutDevState( viewState, renderState, LINE_COLOR );

//             const Point aOutPoint( tools::mapRealPoint2D( aPoint,
//                                                           viewState, renderState ) );
//             // TODO(F1): alpha
//             mpOutDev->getOutDev().DrawPixel( aOutPoint );

//             if( mp2ndOutDev.get() )
//                 mp2ndOutDev->getOutDev().DrawPixel( aOutPoint );
//         }
    }

    void CanvasHelper::useStates( const rendering::ViewState& viewState,
				  const rendering::RenderState& renderState,
				  bool setColor )
    {
	Matrix aViewMatrix;
	Matrix aRenderMatrix;
	Matrix aCombinedMatrix;

	cairo_matrix_init( &aViewMatrix,
				    viewState.AffineTransform.m00, viewState.AffineTransform.m10, viewState.AffineTransform.m01,
				    viewState.AffineTransform.m11, viewState.AffineTransform.m02, viewState.AffineTransform.m12);
	cairo_matrix_init( &aRenderMatrix,
				    renderState.AffineTransform.m00, renderState.AffineTransform.m10, renderState.AffineTransform.m01,
				    renderState.AffineTransform.m11, renderState.AffineTransform.m02, renderState.AffineTransform.m12);
	cairo_matrix_multiply( &aCombinedMatrix, &aRenderMatrix, &aViewMatrix);

        if( viewState.Clip.is() ) {
	    OSL_TRACE ("view clip\n");

	    aViewMatrix.x0 = round( aViewMatrix.x0 );
	    aViewMatrix.y0 = round( aViewMatrix.y0 );
	    cairo_set_matrix( mpCairo, &aViewMatrix );
	    drawPolyPolygonPath( viewState.Clip, Clip );
	}

	aCombinedMatrix.x0 = round( aCombinedMatrix.x0 );
	aCombinedMatrix.y0 = round( aCombinedMatrix.y0 );
	cairo_set_matrix( mpCairo, &aCombinedMatrix );

        if( renderState.Clip.is() ) {
	    OSL_TRACE ("render clip BEGIN\n");

	    drawPolyPolygonPath( renderState.Clip, Clip );
	    OSL_TRACE ("render clip END\n");
	}

	if( setColor ) {
	    if( renderState.DeviceColor.getLength() > 3 )
		cairo_set_source_rgba( mpCairo,
				       renderState.DeviceColor [0],
				       renderState.DeviceColor [1],
				       renderState.DeviceColor [2],
				       renderState.DeviceColor [3] );
	    else if (renderState.DeviceColor.getLength() == 3)
		cairo_set_source_rgb( mpCairo,
				      renderState.DeviceColor [0],
				      renderState.DeviceColor [1],
				      renderState.DeviceColor [2] );
	}
    }

    void CanvasHelper::drawLine( const rendering::XCanvas& 		rCanvas, 
                                 const geometry::RealPoint2D& 	aStartRealPoint2D,
                                 const geometry::RealPoint2D& 	aEndRealPoint2D,
                                 const rendering::ViewState& 	viewState,
                                 const rendering::RenderState& 	renderState )
    {
	if( mpCairo ) {
	    cairo_save( mpCairo );

	    cairo_set_line_width( mpCairo, 1 );

	    useStates( viewState, renderState, true );

	    cairo_move_to( mpCairo, aStartRealPoint2D.X, aStartRealPoint2D.Y );
	    cairo_line_to( mpCairo, aEndRealPoint2D.X, aEndRealPoint2D.Y );
	    cairo_stroke( mpCairo );

	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");
    }

    void CanvasHelper::drawBezier( const rendering::XCanvas& 			rCanvas, 
                                   const geometry::RealBezierSegment2D&	aBezierSegment,
                                   const geometry::RealPoint2D& 		_aEndPoint,
                                   const rendering::ViewState& 			viewState,
                                   const rendering::RenderState& 		renderState )
    {
	if( mpCairo ) {
	    cairo_save( mpCairo );

	    cairo_set_line_width( mpCairo, 1 );

	    useStates( viewState, renderState, true );
	    cairo_move_to( mpCairo, aBezierSegment.Px, aBezierSegment.Py );
	    cairo_curve_to( mpCairo, aBezierSegment.C1x, aBezierSegment.C1y, aBezierSegment.C2x, aBezierSegment.C2y, _aEndPoint.X, _aEndPoint.Y );
	    cairo_stroke( mpCairo );
	    
	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");
    }

    void CanvasHelper::doOperation( Operation aOperation, Cairo* pCairo, sal_uInt32 nPolygonIndex ) const
    {
	switch( aOperation ) {
	case Fill:
	    if( mpTextures ) {
		::com::sun::star::rendering::Texture aTexture ( (*mpTextures)[nPolygonIndex] );
		if( aTexture.Bitmap.is() ) {
		    unsigned char* data;
		    bool bHasAlpha;
		    Surface* pSurface = tools::surfaceFromXBitmap( (*mpTextures)[nPolygonIndex].Bitmap, mxDevice, data, bHasAlpha );

		    if( pSurface ) {
			cairo_pattern_t* pPattern;

			cairo_save( pCairo );

			::com::sun::star::geometry::AffineMatrix2D aTransform( aTexture.AffineTransform );
			Matrix aScaleMatrix, aTextureMatrix, aScaledTextureMatrix, aOrigMatrix, aNewMatrix;

			cairo_matrix_init( &aTextureMatrix,
					   aTransform.m00, aTransform.m10, aTransform.m01,
					   aTransform.m11, aTransform.m02, aTransform.m12);

			geometry::IntegerSize2D aSize = aTexture.Bitmap->getSize();

			cairo_matrix_init_scale( &aScaleMatrix, 1.0/aSize.Width, 1.0/aSize.Height );
			cairo_matrix_multiply( &aScaledTextureMatrix, &aTextureMatrix, &aScaleMatrix );
			cairo_matrix_invert( &aScaledTextureMatrix );

#if 0
			OSL_TRACE("slow workaround");
			// workaround for X/glitz and/or cairo bug
			// we create big enough temporary surface, copy texture bitmap there and use it for the pattern
			// it only happens on enlargening matrices with REPEAT mode enabled
			Surface* pTmpSurface = mxDevice->getSimilarSurface();
			Cairo* pTmpCairo = cairo_create( pTmpSurface );
			cairo_set_source_surface( pTmpCairo, pSurface, 0, 0 );
			cairo_paint( pTmpCairo );
			pPattern = cairo_pattern_create_for_surface( pTmpSurface );
#else

			// we don't care about repeat mode yet, so the workaround is disabled for now
			pPattern = cairo_pattern_create_for_surface( pSurface );
#endif
 			if( aTexture.RepeatModeX == rendering::TexturingMode::REPEAT &&
			    aTexture.RepeatModeY == rendering::TexturingMode::REPEAT )
			    cairo_pattern_set_extend( pPattern, CAIRO_EXTEND_REPEAT );
			aScaledTextureMatrix.x0 = round( aScaledTextureMatrix.x0 );
			aScaledTextureMatrix.y0 = round( aScaledTextureMatrix.y0 );
			cairo_pattern_set_matrix( pPattern, &aScaledTextureMatrix );

			cairo_set_source( pCairo, pPattern );
			if( !bHasAlpha )
			    cairo_set_operator( pCairo, CAIRO_OPERATOR_SOURCE );
			cairo_fill( pCairo );

			cairo_restore( pCairo );

			cairo_pattern_destroy( pPattern );
			cairo_surface_destroy( pSurface );

#if 0
			cairo_destroy( pTmpCairo );
			cairo_surface_destroy( pTmpSurface );
#endif
		    }

		    if( data )
			free( data );
		} else if( aTexture.Gradient.is() ) {
		    uno::Reference< lang::XServiceInfo > xRef( aTexture.Gradient, uno::UNO_QUERY );

		    OSL_TRACE( "gradient fill\n" );
		    if( xRef.is() && 
			xRef->getImplementationName().equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( PARAMETRICPOLYPOLYGON_IMPLEMENTATION_NAME ) ) ) ) {
				// TODO(Q1): Maybe use dynamic_cast here
				
				// TODO(E1): Return value
				// TODO(F1): FillRule
			OSL_TRACE( "known implementation\n" );

			ParametricPolyPolygon* pPolyImpl = static_cast<ParametricPolyPolygon*>( aTexture.Gradient.get() );
			::com::sun::star::geometry::AffineMatrix2D aTransform( aTexture.AffineTransform );
			Matrix aTextureMatrix;

			cairo_matrix_init( &aTextureMatrix,
					   aTransform.m00, aTransform.m10, aTransform.m01,
					   aTransform.m11, aTransform.m02, aTransform.m12);
			Pattern* pPattern = pPolyImpl->getPattern( aTextureMatrix );

			if( pPattern ) {
			    OSL_TRACE( "filling with pattern\n" );

			    cairo_save( pCairo );

			    cairo_set_source( pCairo, pPattern );
			    cairo_fill( pCairo );
			    cairo_restore( pCairo );

			    cairo_pattern_destroy( pPattern );
			}
		    }
		}
	    } else
		cairo_fill( pCairo );
	    OSL_TRACE("fill\n");
	break;
	case Stroke:
	    cairo_stroke( pCairo );
	    OSL_TRACE("stroke\n");
	break;
	case Clip:
	    cairo_clip( pCairo );
	    OSL_TRACE("clip\n");
	break;
	}
    }

    static void clipNULL( Cairo *pCairo )
    {
	OSL_TRACE("clipNULL\n");
	Matrix aOrigMatrix, aIdentityMatrix;

	/* we set identity matrix here to overcome bug in cairo 0.9.2
	   where XCreatePixmap is called with zero width and height.

	   it also reaches faster path in cairo clipping code.
	*/
	cairo_matrix_init_identity( &aIdentityMatrix );
	cairo_get_matrix( pCairo, &aOrigMatrix );
	cairo_set_matrix( pCairo, &aIdentityMatrix );

	cairo_reset_clip( pCairo );
	cairo_rectangle( pCairo, 0, 0, 1, 1 );
	cairo_clip( pCairo );
	cairo_rectangle( pCairo, 2, 0, 1, 1 );
	cairo_clip( pCairo );

	/* restore the original matrix */
	cairo_set_matrix( pCairo, &aOrigMatrix );
    }

    void CanvasHelper::drawPolyPolygonImplementation( ::basegfx::B2DPolyPolygon aPolyPolygon, Operation aOperation, Cairo* pCairo ) const
    {
	bool bOpToDo = false;
	Matrix aOrigMatrix, aIdentityMatrix;
	double nX, nY, nBX, nBY, nPX, nPY;

	cairo_get_matrix( pCairo, &aOrigMatrix );
	cairo_matrix_init_identity( &aIdentityMatrix );
	cairo_set_matrix( pCairo, &aIdentityMatrix );

	for( sal_uInt32 nPolygonIndex = 0; nPolygonIndex < aPolyPolygon.count(); nPolygonIndex++ ) {
	    ::basegfx::B2DPolygon aPolygon = aPolyPolygon.getB2DPolygon( nPolygonIndex );

	    if( aPolygon.count() > 1) {
		bool bIsBezier = aPolygon.areControlPointsUsed();
		::basegfx::B2DPoint aA, aB, aP;

		aP = aPolygon.getB2DPoint( 0 );
		nX = aP.getX();
		nY = aP.getY();
		cairo_matrix_transform_point( &aOrigMatrix, &nX, &nY );
		if( bIsBezier )
		    cairo_move_to( pCairo, nX, nY );
		else
		    cairo_move_to( pCairo, round(nX), round(nY) );
		OSL_TRACE( "move to %f,%f\n", round(nX), round(nY) );

		if( bIsBezier ) {
		    aA = aPolygon.getControlPointA( 0 );
		    aB = aPolygon.getControlPointB( 0 );
		}

		for( sal_uInt32 j = 1; j < aPolygon.count(); j++ ) {
		    aP = aPolygon.getB2DPoint( j );

		    nX = aP.getX();
		    nY = aP.getY();
		    cairo_matrix_transform_point( &aOrigMatrix, &nX, &nY );

		    if( bIsBezier ) {
			nBX = aB.getX();
			nBY = aB.getY();
			nPX = aP.getX();
			nPY = aP.getY();
			cairo_matrix_transform_point( &aOrigMatrix, &nBX, &nBY );
			cairo_matrix_transform_point( &aOrigMatrix, &nPX, &nPY );
			cairo_curve_to( pCairo, nX, nY, nBX, nBY, nPX, nPY );
			
			aA = aPolygon.getControlPointA( j );
			aB = aPolygon.getControlPointB( j );
		    } else {
			cairo_line_to( pCairo, round(nX), round(nY) );
			OSL_TRACE( "line to %f,%f\n", round(nX), round(nY) );
		    }
		    bOpToDo = true;
		}

		if( aPolygon.isClosed() )
		    cairo_close_path( pCairo );

		if( aOperation == Fill && mpTextures )
		    doOperation( aOperation, pCairo, nPolygonIndex );
	    } else {
		OSL_TRACE( "empty polygon for op: %d\n\n", aOperation );
		if( aOperation == Clip ) {
		    clipNULL( pCairo );

		    return;
		}
	    }
	}
	if( bOpToDo && ( aOperation != Fill || !mpTextures ) )
	    doOperation( aOperation, pCairo );

	cairo_set_matrix( pCairo, &aOrigMatrix );

// fixme, spec says even clipping polypolygon with zero polygons means NULL clip, but it breaks animations with sprites
// 	if( aPolyPolygon.count() == 0 && aOperation == Clip )
// 	    clipNULL( pCairo );
    }

    void CanvasHelper::drawPolyPolygonFallback( const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon, Operation aOperation, Cairo* pCairo ) const
    {
	const sal_Int32 nPolys( xPolyPolygon->getNumberOfPolygons() );

	uno::Reference< rendering::XLinePolyPolygon2D > xLinePoly( xPolyPolygon, uno::UNO_QUERY );
	if( xLinePoly.is() ) {
			    
	    bool bOpToDo = false;
	    uno::Sequence< uno::Sequence< ::com::sun::star::geometry::RealPoint2D > > aPoints = xLinePoly->getPoints( 0, nPolys, 0, -1 );

	    for( sal_uInt32 nPolygonIndex = 0; nPolygonIndex < nPolys; nPolygonIndex++ ) {

		if( aPoints[nPolygonIndex].getLength() > 1) {
		    cairo_move_to( pCairo, aPoints[nPolygonIndex][0].X, aPoints[nPolygonIndex][0].Y );

		    for( sal_Int32 j = 0; j < aPoints[nPolygonIndex].getLength(); j ++ ) {
			cairo_line_to( pCairo, aPoints[nPolygonIndex][j].X, aPoints[nPolygonIndex][j].Y );
			bOpToDo = true;
		    }

		    if( xPolyPolygon->isClosed( nPolygonIndex ) )
			cairo_close_path( pCairo );

		    if( aOperation == Fill &&  mpTextures )
			doOperation( aOperation, pCairo, nPolygonIndex );
		} else {
		    OSL_TRACE( "empty polygon for op: %d\n\n", aOperation );
		    if( aOperation == Clip ) {
			clipNULL( pCairo );

			return;
		    }
		}
	    }
	    if( bOpToDo && ( aOperation != Fill || !mpTextures ) )
		doOperation( aOperation, pCairo );

// fixme, spec says even clipping polypolygon with zero polygons means NULL clip, but it breaks animations with sprites
// 	    if( nPolys == 0 && aOperation == Clip )
// 		clipNULL( pCairo );
	} else {

	    uno::Reference< rendering::XBezierPolyPolygon2D > xBezierPoly( xPolyPolygon, uno::UNO_QUERY );
	    if( xBezierPoly.is() ) {

		bool bOpToDo = false;
		uno::Sequence< uno::Sequence< ::com::sun::star::geometry::RealBezierSegment2D > > aSegments = xBezierPoly->getBezierSegments( 0, nPolys, 0, -1 );
		double nC1x, nC1y, nC2x, nC2y;

		for( sal_uInt32 nPolygonIndex = 0; nPolygonIndex < nPolys; nPolygonIndex++ ) {

		    if( aSegments[nPolygonIndex].getLength() > 1 ) {
			cairo_move_to( pCairo, aSegments[nPolygonIndex][0].Px, aSegments[nPolygonIndex][0].Py );

			for( sal_Int32 j = 1; j < aSegments[nPolygonIndex].getLength(); j ++ ) {
			    cairo_curve_to( pCairo,
					    aSegments[nPolygonIndex][j - 1].C1x, aSegments[nPolygonIndex][j - 1].C1y,
					    aSegments[nPolygonIndex][j - 1].C2x, aSegments[nPolygonIndex][j - 1].C2y,
					    aSegments[nPolygonIndex][j].Px, aSegments[nPolygonIndex][j].Py );
			    bOpToDo = true;
			}
			if( xPolyPolygon->isClosed( nPolygonIndex ) )
			    cairo_close_path( pCairo );

			if( aOperation == Fill &&  mpTextures )
			    doOperation( aOperation, pCairo, nPolygonIndex );
		    } else {
			OSL_TRACE( "empty polygon for op: %d\n\n", aOperation );
			if( aOperation == Clip ) {
			    clipNULL( pCairo );

			    return;
			}
		    }
		}
		if( bOpToDo && ( aOperation != Fill || !mpTextures ) )
		    doOperation( aOperation, pCairo );

// fixme, spec says even clipping polypolygon with zero polygons means NULL clip, but it breaks animations with sprites
// 		if( nPolys == 0 && aOperation == Clip )
// 		    clipNULL( pCairo );
	    }
	}
    }

    void CanvasHelper::drawPolyPolygonPath( const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon, Operation aOperation, Cairo* pCairo ) const
    {
	LinePolyPolygon* pPolyImpl = dynamic_cast< LinePolyPolygon* >( xPolyPolygon.get() );

	if( !pCairo )
	    pCairo = mpCairo;

	if( pPolyImpl )
	    drawPolyPolygonImplementation( pPolyImpl->getPolyPolygon(), aOperation, pCairo );
	else
	    drawPolyPolygonFallback( xPolyPolygon, aOperation, pCairo );
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawPolyPolygon( const rendering::XCanvas& 							rCanvas, 
										 const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon,
										 const rendering::ViewState& 						viewState,
										 const rendering::RenderState& 						renderState )
    {
	//return uno::Reference< rendering::XCachedPrimitive >(NULL);

	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if( mpCairo ) {
	    cairo_save( mpCairo );

	    cairo_set_line_width( mpCairo, 1 );

	    useStates( viewState, renderState, true );
	    drawPolyPolygonPath( xPolyPolygon, Stroke );
	    
	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "drawPolyPolygon" );
        #endif

	// TODO(P1): Provide caching here.
	return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::strokePolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                   const uno::Reference< rendering::XPolyPolygon2D >& 	xPolyPolygon,
                                                                                   const rendering::ViewState& 							viewState,
                                                                                   const rendering::RenderState& 						renderState,
                                                                                   const rendering::StrokeAttributes& 					strokeAttributes )
    {
	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if( mpCairo ) {
	    cairo_save( mpCairo );

	    useStates( viewState, renderState, true );

	    cairo_set_line_width( mpCairo, strokeAttributes.StrokeWidth );
	    cairo_set_miter_limit( mpCairo, strokeAttributes.MiterLimit );

	    // FIXME: cairo doesn't handle end cap so far (rodo)
	    switch( strokeAttributes.StartCapType ) {
	    case rendering::PathCapType::BUTT:
		cairo_set_line_cap( mpCairo, CAIRO_LINE_CAP_BUTT );
		break;
	    case rendering::PathCapType::ROUND:
		cairo_set_line_cap( mpCairo, CAIRO_LINE_CAP_ROUND );
		break;
	    case rendering::PathCapType::SQUARE:
		cairo_set_line_cap( mpCairo, CAIRO_LINE_CAP_SQUARE );
		break;
	    }

	    switch( strokeAttributes.JoinType ) {
		// cairo doesn't have join type NONE so we use MITTER as it's pretty close
	    case rendering::PathJoinType::NONE:
	    case rendering::PathJoinType::MITER:
		cairo_set_line_join( mpCairo, CAIRO_LINE_JOIN_MITER );
		break;
	    case rendering::PathJoinType::ROUND:
		cairo_set_line_join( mpCairo, CAIRO_LINE_JOIN_ROUND );
		break;
	    case rendering::PathJoinType::BEVEL:
		cairo_set_line_join( mpCairo, CAIRO_LINE_JOIN_BEVEL );
		break;
	    }

	    if( strokeAttributes.DashArray.getLength() > 0 ) {
		double* pDashArray = new double[ strokeAttributes.DashArray.getLength() ];
		for( sal_Int32 i=0; i<strokeAttributes.DashArray.getLength(); i++ )
		    pDashArray[i]=strokeAttributes.DashArray[i];
		cairo_set_dash( mpCairo, pDashArray, strokeAttributes.DashArray.getLength(), 0 );
		delete[] pDashArray;
	    }

	    // TODO(rodo) use LineArray of strokeAttributes

	    drawPolyPolygonPath( xPolyPolygon, Stroke );

	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "strokePolyPolygon" );
        #endif

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::strokeTexturedPolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                           const uno::Reference< rendering::XPolyPolygon2D >& 	xPolyPolygon,
                                                                                           const rendering::ViewState& 							viewState,
                                                                                           const rendering::RenderState& 						renderState,
                                                                                           const uno::Sequence< rendering::Texture >& 			textures,
                                                                                           const rendering::StrokeAttributes& 					strokeAttributes )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::strokeTextureMappedPolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                                const uno::Reference< rendering::XPolyPolygon2D >& 	xPolyPolygon,
                                                                                                const rendering::ViewState& 						viewState,
                                                                                                const rendering::RenderState& 						renderState,
                                                                                                const uno::Sequence< rendering::Texture >& 			textures,
                                                                                                const uno::Reference< geometry::XMapping2D >& 		xMapping,
                                                                                                const rendering::StrokeAttributes& 					strokeAttributes )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XPolyPolygon2D >   CanvasHelper::queryStrokeShapes( const rendering::XCanvas& 							rCanvas, 
                                                                                   const uno::Reference< rendering::XPolyPolygon2D >& 	xPolyPolygon,
                                                                                   const rendering::ViewState& 							viewState,
                                                                                   const rendering::RenderState& 						renderState,
                                                                                   const rendering::StrokeAttributes& 					strokeAttributes )
    {
        return uno::Reference< rendering::XPolyPolygon2D >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::fillPolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                 const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon,
                                                                                 const rendering::ViewState& 						viewState,
                                                                                 const rendering::RenderState& 						renderState )
    {
	//return uno::Reference< rendering::XCachedPrimitive >(NULL);

	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if( mpCairo ) {
	    cairo_save( mpCairo );

	    useStates( viewState, renderState, true );
	    drawPolyPolygonPath( xPolyPolygon, Fill );
	    
	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "fillPolyPolygon" );
        #endif

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::fillTextureMappedPolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                              const uno::Reference< rendering::XPolyPolygon2D >& 	xPolyPolygon,
                                                                                              const rendering::ViewState& 							viewState,
                                                                                              const rendering::RenderState& 						renderState,
                                                                                              const uno::Sequence< rendering::Texture >& 			textures,
                                                                                              const uno::Reference< geometry::XMapping2D >& 		xMapping	 )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCanvasFont > CanvasHelper::createFont( const rendering::XCanvas& 						rCanvas, 
                                                                       const rendering::FontRequest& 					fontRequest,
                                                                       const uno::Sequence< beans::PropertyValue >& 	extraFontProperties,
                                                                       const geometry::Matrix2D& 						fontMatrix )
    {
	// rodo TODO
        if( mpCairo )
        {
            // TODO(F2): font properties and font matrix
            return uno::Reference< rendering::XCanvasFont >(
                    new CanvasFont( fontRequest, extraFontProperties, fontMatrix, mpCairo ) );
        } else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

        return uno::Reference< rendering::XCanvasFont >();
    }

    uno::Sequence< rendering::FontInfo > CanvasHelper::queryAvailableFonts( const rendering::XCanvas& 						rCanvas, 
                                                                            const rendering::FontInfo& 						aFilter,
                                                                            const uno::Sequence< beans::PropertyValue >& 	aFontProperties )
    {
        // TODO
        return uno::Sequence< rendering::FontInfo >();
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawText( const rendering::XCanvas& 						rCanvas, 
                                                                          const rendering::StringContext& 					text,
                                                                          const uno::Reference< rendering::XCanvasFont >& 	xFont,
                                                                          const rendering::ViewState& 						viewState,
                                                                          const rendering::RenderState& 					renderState,
                                                                          sal_Int8				 							textDirection )
    {
	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if ( mpCairo ) {
	    cairo_save( mpCairo );

	    useStates( viewState, renderState, true );

	    // fixme (rodo) - use xFont
	    cairo_select_font_face( mpCairo, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
	    /* move to 0, 0 as cairo_show_text advances current point and current point is not restored by cairo_restore.
	       before we were depending on unmodified current point which I believed was preserved by save/restore */
	    cairo_move_to( mpCairo, 0, 0 );
	    cairo_show_text( mpCairo, ::rtl::OUStringToOString( text.Text, RTL_TEXTENCODING_UTF8 ) );

	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "drawText" );
        #endif

	return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawTextLayout( const rendering::XCanvas& 						rCanvas, 
                                                                                const uno::Reference< rendering::XTextLayout >& xLayoutedText,
                                                                                const rendering::ViewState& 					viewState,
                                                                                const rendering::RenderState& 					renderState )
    {
	//return uno::Reference< rendering::XCachedPrimitive >(NULL);

	CHECK_AND_THROW( xLayoutedText.is(),
			 "CanvasHelper::drawTextLayout(): layout is NULL");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if( mpCairo ) {
	    cairo_save( mpCairo );

	    uno::Reference< lang::XServiceInfo > xRef( xLayoutedText,
						       uno::UNO_QUERY );

	    TextLayout* pTextLayout = NULL;

	    if( xRef.is() &&
		xRef->getImplementationName().equals( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(TEXTLAYOUT_IMPLEMENTATION_NAME))) ) {
		// TODO(P2): Maybe use dynamic_cast here (saves us a queryInterface)
		pTextLayout = static_cast<TextLayout*>(xLayoutedText.get());
	    } else {
		CHECK_AND_THROW( false,
				 "CanvasHelper::drawTextLayout(): TextLayout not compatible with this canvas" );
	    }

	    useStates( viewState, renderState, true );

	    pTextLayout->draw( mpCairo );

	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "drawTextLayout" );
        #endif

	return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::implDrawBitmapSurface( Surface* pSurface,
										       const rendering::ViewState& viewState,
										       const rendering::RenderState& renderState,
										       bool bModulateColors,
										       bool bHasAlpha )
    {
	if( mpCairo ) {
	    cairo_save( mpCairo );

	    Size aSize = mxDevice->getSurfaceSize();

 	    cairo_rectangle( mpCairo, 0, 0, aSize.Width(), aSize.Height() );
 	    cairo_clip( mpCairo );

	    useStates( viewState, renderState, true );

//  	    if( !bHasAlpha )
//  		cairo_set_operator( mpCairo, CAIRO_OPERATOR_SOURCE );

	    cairo_set_source_surface( mpCairo, pSurface, 0, 0 );
	    cairo_paint( mpCairo );
	    cairo_restore( mpCairo );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawBitmap( const rendering::XCanvas& 					rCanvas, 
                                                                            const uno::Reference< rendering::XBitmap >& xBitmap,
                                                                            const rendering::ViewState& 				viewState,
                                                                            const rendering::RenderState& 				renderState )
    {
	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	uno::Reference< rendering::XCachedPrimitive > rv;
	unsigned char* data;
	bool bHasAlpha;
	Surface* pSurface = tools::surfaceFromXBitmap( xBitmap, mxDevice, data, bHasAlpha );

	if( pSurface ) {
	    rv = implDrawBitmapSurface( pSurface, viewState, renderState, false, bHasAlpha );

	    cairo_surface_destroy( pSurface );

	    if( data )
		free( data );
	} else
	    rv = uno::Reference< rendering::XCachedPrimitive >(NULL);

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "drawBitmap" );
        #endif

	return rv;
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawBitmapModulated( const rendering::XCanvas& 						rCanvas, 
                                                                                     const uno::Reference< rendering::XBitmap >& 	xBitmap,
                                                                                     const rendering::ViewState& 					viewState,
                                                                                     const rendering::RenderState& 					renderState )
    {
	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	uno::Reference< rendering::XCachedPrimitive > rv;
	unsigned char* data;
	bool bHasAlpha;
	Surface* pSurface = tools::surfaceFromXBitmap( xBitmap, mxDevice, data, bHasAlpha );

	if( pSurface ) {
	    rv = implDrawBitmapSurface( pSurface, viewState, renderState, true, bHasAlpha );
	    cairo_surface_destroy( pSurface );

	    if( data )
		free( data );
	} else
	    rv = uno::Reference< rendering::XCachedPrimitive >(NULL);

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "drawBitmapModulated" );
        #endif

	return rv;
    }

    uno::Reference< rendering::XGraphicDevice > CanvasHelper::getDevice()
    {
        return mxDevice.get();
    }

    void CanvasHelper::copyRect( const rendering::XCanvas& 							rCanvas, 
                                 const uno::Reference< rendering::XBitmapCanvas >& 	sourceCanvas, 
                                 const geometry::RealRectangle2D& 					sourceRect, 
                                 const rendering::ViewState& 						sourceViewState, 
                                 const rendering::RenderState& 						sourceRenderState, 
                                 const geometry::RealRectangle2D& 					destRect, 
                                 const rendering::ViewState& 						destViewState, 
                                 const rendering::RenderState& 						destRenderState )
    {
        // TODO(F1)
    }
            
    geometry::IntegerSize2D CanvasHelper::getSize()
    {
        if( !mxDevice.get() )
            return geometry::IntegerSize2D(); // we're disposed

	Size aSize = mxDevice->getSurfaceSize();

        return geometry::IntegerSize2D( aSize.Width(), aSize.Height() );
    }

    uno::Reference< rendering::XBitmap > CanvasHelper::getScaledBitmap( const geometry::RealSize2D& newSize, 
                                                                        sal_Bool 					beFast )
    {
	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if( mpCairo ) {
	    Surface *pSurface = cairo_get_target( mpCairo );
	    return uno::Reference< rendering::XBitmap >( new CanvasBitmap( newSize, pSurface, mxDevice, beFast ) );
	} else
	    OSL_TRACE ("CanvasHelper called after it was disposed");

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "getScaledBitmap" );
        #endif

	return uno::Reference< rendering::XBitmap >(); // we're disposed
    }

    uno::Sequence< sal_Int8 > CanvasHelper::getData( const geometry::IntegerRectangle2D& rect )
    {
	OSL_TRACE ("CanvasHelper::getData returns empty sequence, TODO\n");

        return uno::Sequence< sal_Int8 >();
    }

    void CanvasHelper::setData( const uno::Sequence< sal_Int8 >& 	data, 
                                const geometry::IntegerRectangle2D&	rect )
    {
	OSL_TRACE ("canvas helper set data called, TODO\n");
    }

    void CanvasHelper::setPixel( const uno::Sequence< sal_Int8 >& 	color, 
                                 const geometry::IntegerPoint2D& 	pos )
    {
	// rodo TODO
	OSL_TRACE ("CanvasHelper::setPixel called TODO\n");
//         if( !mpOutDev.get() )
//             return; // we're disposed

//         OutputDevice& rOutDev( mpOutDev->getOutDev() );

//         const Size aBmpSize( rOutDev.GetOutputSizePixel() );

//         CHECK_AND_THROW( pos.X >= 0 && pos.X < aBmpSize.Width(), 
//                          "CanvasHelper::setPixel: X coordinate out of bounds" );
//         CHECK_AND_THROW( pos.Y >= 0 && pos.Y < aBmpSize.Height(), 
//                          "CanvasHelper::setPixel: Y coordinate out of bounds" );
//         CHECK_AND_THROW( color.getLength() > 3, 
//                          "CanvasHelper::setPixel: not enough color components" );

//         tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

//         rOutDev.EnableMapMode( FALSE );

//         // TODO(F2): Support alpha canvas here
//         rOutDev.DrawPixel( ::vcl::unotools::pointFromIntegerPoint2D( pos ), 
//                            ::vcl::unotools::sequenceToColor( 
//                                mxDevice.getRef(),
//                                color ) );
    }

    uno::Sequence< sal_Int8 > CanvasHelper::getPixel( const geometry::IntegerPoint2D& pos )
    {
	OSL_TRACE ("CanvasHelper::getPixel called TODO\n");
            return uno::Sequence< sal_Int8 >();

//         if( !mpOutDev.get() )
//             return uno::Sequence< sal_Int8 >(); // we're disposed

//         OutputDevice& rOutDev( mpOutDev->getOutDev() );

//         const Size aBmpSize( rOutDev.GetOutputSizePixel() );

//         CHECK_AND_THROW( pos.X >= 0 && pos.X < aBmpSize.Width(), 
//                          "CanvasHelper::getPixel: X coordinate out of bounds" );
//         CHECK_AND_THROW( pos.Y >= 0 && pos.Y < aBmpSize.Height(), 
//                          "CanvasHelper::getPixel: Y coordinate out of bounds" );

//         tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

//         rOutDev.EnableMapMode( FALSE );

//         // TODO(F2): Support alpha canvas here
//         return ::vcl::unotools::colorToIntSequence( mxDevice.getRef(),
//                                                     rOutDev.GetPixel( 
//                                                         ::vcl::unotools::pointFromIntegerPoint2D( pos ) ) ); 
    }

    uno::Reference< rendering::XBitmapPalette > CanvasHelper::getPalette()
    {
        // TODO(F1): Provide palette support
        return uno::Reference< rendering::XBitmapPalette >();
    }

    rendering::IntegerBitmapLayout CanvasHelper::getMemoryLayout()
    {
        // TODO(F1): finish that one
        rendering::IntegerBitmapLayout aLayout;

	OSL_TRACE ("CanvasHelper::getMemoryLayout called, TODO\n");

	return aLayout;


//         if( !mpOutDev.get() )
//             return aLayout; // we're disposed

//         const Size aBmpSize( mpOutDev->getOutDev().GetOutputSizePixel() );

//         aLayout.ScanLines = aBmpSize.Height();
//         aLayout.ScanLineBytes = aBmpSize.Width()*4;
//         aLayout.ScanLineStride = aLayout.ScanLineBytes;
//         aLayout.Format = 0;
//         aLayout.NumComponents = 4;
//         aLayout.ComponentMasks = uno::Sequence<sal_Int64>();
//         aLayout.Endianness = 0;
//         aLayout.IsPseudoColor = false;

//         return aLayout;
    }

    bool CanvasHelper::repaint( const GraphicObjectSharedPtr&	rGrf,
                                const ::Point& 					rPt, 
                                const ::Size& 					rSz,
                                const GraphicAttr&				rAttr ) const
    {
        ENSURE_AND_RETURN( rGrf,
                           "CanvasHelper::repaint(): Invalid Graphic" );

	OSL_TRACE ("repaint called, TODO\n");

	return true;

//         if( !mpOutDev )
//             return false; // disposed
//         else
//             return rGrf->Draw( &mpOutDev->getOutDev(), rPt, rSz, &rAttr );
    }
}
