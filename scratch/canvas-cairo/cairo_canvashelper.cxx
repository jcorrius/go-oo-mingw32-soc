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


using namespace ::com::sun::star;

namespace vclcanvas
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
        mp2ndOutDev()
    {
    }

    void CanvasHelper::disposing()
    {
        mxDevice.reset();
        mp2ndOutDev.reset();
    }

    void CanvasHelper::setGraphicDevice( const WindowGraphicDevice::ImplRef& rDevice )
    {
        mxDevice = rDevice;
    }

    void CanvasHelper::setCairo( ::cairo::Cairo* pCairo )
    {
	mpCairo = pCairo;
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
	    //   if( viewState.Clip.is() ) {
	    ::cairo::Matrix aViewMatrix;
	    ::cairo::Matrix aRenderMatrix;
	    ::cairo::Matrix aCombinedMatrix;

	    ::cairo::cairo_matrix_init( &aViewMatrix, viewState.AffineTransform.m00, viewState.AffineTransform.m10, viewState.AffineTransform.m01, viewState.AffineTransform.m11, viewState.AffineTransform.m02, viewState.AffineTransform.m12);
	    ::cairo::cairo_matrix_init( &aRenderMatrix, renderState.AffineTransform.m00, renderState.AffineTransform.m10, renderState.AffineTransform.m01, renderState.AffineTransform.m11, renderState.AffineTransform.m02, renderState.AffineTransform.m12);
	    ::cairo::cairo_matrix_multiply( &aCombinedMatrix, &aRenderMatrix, &aViewMatrix);
	    ::cairo::cairo_set_matrix( mpCairo, &aCombinedMatrix );
	    //   }

	    if (setColor) {
		    if (renderState.DeviceColor.getLength() > 3)
			    ::cairo::cairo_set_source_rgba( mpCairo, renderState.DeviceColor [0], renderState.DeviceColor [1], renderState.DeviceColor [2], renderState.DeviceColor [3] );
		    else if (renderState.DeviceColor.getLength() == 3)
			    ::cairo::cairo_set_source_rgb( mpCairo, renderState.DeviceColor [0], renderState.DeviceColor [1], renderState.DeviceColor [2] );
	    }
    }

    void CanvasHelper::drawLine( const rendering::XCanvas& 		rCanvas, 
                                 const geometry::RealPoint2D& 	aStartRealPoint2D,
                                 const geometry::RealPoint2D& 	aEndRealPoint2D,
                                 const rendering::ViewState& 	viewState,
                                 const rendering::RenderState& 	renderState )
    {
	if(mpCairo) {
	    ::cairo::cairo_save( mpCairo );

	    useStates( viewState, renderState, true );

	    ::cairo::cairo_set_line_width( mpCairo, 1 );
	    ::cairo::cairo_move_to( mpCairo, aStartRealPoint2D.X, aStartRealPoint2D.Y );
	    ::cairo::cairo_line_to( mpCairo, aEndRealPoint2D.X, aEndRealPoint2D.Y );
	    ::cairo::cairo_stroke( mpCairo );

	    ::cairo::cairo_restore( mpCairo );
	}
    }

    void CanvasHelper::drawBezier( const rendering::XCanvas& 			rCanvas, 
                                   const geometry::RealBezierSegment2D&	aBezierSegment,
                                   const geometry::RealPoint2D& 		_aEndPoint,
                                   const rendering::ViewState& 			viewState,
                                   const rendering::RenderState& 		renderState )
    {
	// rodo TODO
//         if( mpOutDev.get() )
//         {
//             tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

//             setupOutDevState( viewState, renderState, LINE_COLOR );

//             const Point aStartPoint( tools::mapRealPoint2D( geometry::RealPoint2D(aBezierSegment.Px, aBezierSegment.Py),
//                                                             viewState, renderState ) );
//             const Point aEndPoint( tools::mapRealPoint2D( _aEndPoint,
//                                                           viewState, renderState ) );
//             // TODO(F1): Provide true beziers here!
//             // TODO(F2): alpha
//             mpOutDev->getOutDev().DrawLine( aStartPoint, aEndPoint );
//             if( mp2ndOutDev.get() )
//                 mp2ndOutDev->getOutDev().DrawLine( aStartPoint, aEndPoint );
//         }
    }

    void CanvasHelper::drawPolyPolygonPath( const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon, bool fill )
    {
	    printf ("CanvasHelper::drawPolyPolygonPath, fill: %d number of polygons: %d on surface %p\n", fill, xPolyPolygon->getNumberOfPolygons(), ::cairo::cairo_get_target ( mpCairo ) );
	/* TODO: optimize using known implementation as vcl canvas does (rodo)

	  LinePolyPolygon* pPolyImpl = dynamic_cast< LinePolyPolygon* >( xPoly.get() );

	if( pPolyImpl )
		{
			return pPolyImpl->getPolyPolygon();
		}
		else */
	{
		const sal_Int32 nPolys( xPolyPolygon->getNumberOfPolygons() );

		// not a known implementation object - try data source
		// interfaces
		/*uno::Reference< rendering::XBezierPolyPolygon2D > xBezierPoly( xPolyPolygon, uno::UNO_QUERY );

		if( xBezierPoly.is() )
			printf ("bezier polygon\n");
			//return ::basegfx::unotools::polyPolygonFromBezier2DSequenceSequence( xBezierPoly->getBezierSegments( 0, nPolys, 0, -1 ) );
			else */ {
			uno::Reference< rendering::XLinePolyPolygon2D > xLinePoly( xPolyPolygon, uno::UNO_QUERY );

			// no implementation class and no data provider
			// found - contract violation.
			CHECK_AND_THROW( xLinePoly.is(),
					 "Cairo CanvasHelper::drawPolyPolygon: Invalid input "
					 "poly-polygon, cannot retrieve vertex data" );
			uno::Sequence< uno::Sequence< ::com::sun::star::geometry::RealPoint2D > > aPoints = xLinePoly->getPoints( 0, nPolys, 0, -1 );

			for( sal_Int32 i = 0; i < nPolys; i ++ ) {
				printf ("CanvasHelper::drawPolyPolygonPath, lines: %d\n", aPoints[i].getLength() );

				for( sal_Int32 j = 0; j < aPoints[i].getLength(); j ++ ) {
					if( j == 0 )
						::cairo::cairo_move_to( mpCairo, aPoints[i][j].X, aPoints[i][j].Y );
					else
						::cairo::cairo_line_to( mpCairo, aPoints[i][j].X, aPoints[i][j].Y );
				}
				if( xPolyPolygon->isClosed( i ) )
					::cairo::cairo_close_path( mpCairo );
				if (fill)
					::cairo::cairo_fill( mpCairo );
				else
					::cairo::cairo_stroke( mpCairo );
			}
		}
	}

    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawPolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                 const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon,
                                                                                 const rendering::ViewState& 						viewState,
                                                                                 const rendering::RenderState& 						renderState )
    {
	if( mpCairo ) {
	    ::cairo::cairo_save( mpCairo );

	    useStates( viewState, renderState, true );
	    ::cairo::cairo_set_line_width( mpCairo, 1 );
	    drawPolyPolygonPath( xPolyPolygon, false );
	    
	    ::cairo::cairo_restore( mpCairo );
	}

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::strokePolyPolygon( const rendering::XCanvas& 							rCanvas, 
                                                                                   const uno::Reference< rendering::XPolyPolygon2D >& 	xPolyPolygon,
                                                                                   const rendering::ViewState& 							viewState,
                                                                                   const rendering::RenderState& 						renderState,
                                                                                   const rendering::StrokeAttributes& 					strokeAttributes )
    {
	::cairo::cairo_save( mpCairo );

	useStates( viewState, renderState, true );

	::cairo::cairo_set_line_width( mpCairo, strokeAttributes.StrokeWidth );
	::cairo::cairo_set_miter_limit( mpCairo, strokeAttributes.MiterLimit );

	// FIXME: cairo doesn't handle end cap so far (rodo)
	switch( strokeAttributes.StartCapType ) {
	case rendering::PathCapType::BUTT:
		::cairo::cairo_set_line_cap( mpCairo, ::cairo::CAIRO_LINE_CAP_BUTT );
	        break;
	case rendering::PathCapType::ROUND:
		::cairo::cairo_set_line_cap( mpCairo, ::cairo::CAIRO_LINE_CAP_ROUND );
	        break;
	case rendering::PathCapType::SQUARE:
		::cairo::cairo_set_line_cap( mpCairo, ::cairo::CAIRO_LINE_CAP_SQUARE );
	        break;
	}

	switch( strokeAttributes.JoinType ) {
	// cairo doesn't have join type NONE so we use MITTER as it's pretty close
	case rendering::PathJoinType::NONE:
	case rendering::PathJoinType::MITER:
		::cairo::cairo_set_line_join( mpCairo, ::cairo::CAIRO_LINE_JOIN_MITER );
	        break;
	case rendering::PathJoinType::ROUND:
		::cairo::cairo_set_line_join( mpCairo, ::cairo::CAIRO_LINE_JOIN_ROUND );
	        break;
	case rendering::PathJoinType::BEVEL:
		::cairo::cairo_set_line_join( mpCairo, ::cairo::CAIRO_LINE_JOIN_BEVEL );
	        break;
	}

	if( strokeAttributes.DashArray.getLength() > 0 ) {
		double* pDashArray = new double[ strokeAttributes.DashArray.getLength() ];
		for( sal_Int32 i=0; i<strokeAttributes.DashArray.getLength(); i++ )
			pDashArray[i]=strokeAttributes.DashArray[i];
		::cairo::cairo_set_dash( mpCairo, pDashArray, strokeAttributes.DashArray.getLength(), 0 );
		delete[] pDashArray;
	}

	// TODO(rodo) use LineArray of strokeAttributes

	drawPolyPolygonPath( xPolyPolygon, false );
	    
	::cairo::cairo_restore( mpCairo );

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
	if( mpCairo ) {
	    ::cairo::cairo_save( mpCairo );

	    useStates( viewState, renderState, true );
	    drawPolyPolygonPath( xPolyPolygon, true );
	    
	    ::cairo::cairo_restore( mpCairo );
	}

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
//         if( mpOutDev.get() )
//         {
//             // TODO(F2): font properties and font matrix
//             return uno::Reference< rendering::XCanvasFont >(
//                     new CanvasFont(fontRequest, extraFontProperties, fontMatrix, mpOutDev) );
//         }

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
	 if (mpCairo) {
		 ::cairo::cairo_save( mpCairo );

		 useStates( viewState, renderState, true );

		 // fixme (rodo) - use xFont
		 ::cairo::cairo_select_font_face( mpCairo, "Sans", ::cairo::CAIRO_FONT_SLANT_NORMAL, ::cairo::CAIRO_FONT_WEIGHT_NORMAL );
		 ::cairo::cairo_show_text( mpCairo, ::rtl::OUStringToOString( text.Text, RTL_TEXTENCODING_UTF8 ) );

		 ::cairo::cairo_restore( mpCairo );
	 }


        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawTextLayout( const rendering::XCanvas& 						rCanvas, 
                                                                                const uno::Reference< rendering::XTextLayout >& xLayoutedText,
                                                                                const rendering::ViewState& 					viewState,
                                                                                const rendering::RenderState& 					renderState )
    {
	// rodo TODO
//         CHECK_AND_THROW( xLayoutedText.is(),
//                          "CanvasHelper::drawTextLayout(): layout is NULL");

//         uno::Reference< lang::XServiceInfo > xRef( xLayoutedText,
//                                                    uno::UNO_QUERY );

//         TextLayout* pTextLayout = NULL;

//         if( xRef.is() &&
//             xRef->getImplementationName().equals( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(TEXTLAYOUT_IMPLEMENTATION_NAME))) )
//         {
//             // TODO(P2): Maybe use dynamic_cast here (saves us a queryInterface)
//             pTextLayout = static_cast<TextLayout*>(xLayoutedText.get());
//         }
//         else
//         {
//             CHECK_AND_THROW( false,
//                              "CanvasHelper::drawTextLayout(): TextLayout not compatible with this canvas" );
//         }

//         if( mpOutDev.get() )
//         {
//             tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

//             ::Point aOutpos;
//             if( !setupTextOutput( aOutpos, viewState, renderState, xLayoutedText->getFont() ) )
//                 return uno::Reference< rendering::XCachedPrimitive >(NULL); // no output necessary

//             // TODO(F2): What about the offset scalings?
//             // TODO(F2): alpha
//             pTextLayout->draw( mpOutDev->getOutDev(), aOutpos, viewState, renderState );

//             if( mp2ndOutDev.get() )
//                 pTextLayout->draw( mp2ndOutDev->getOutDev(), aOutpos, viewState, renderState );
//         }

        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::implDrawBitmap( const rendering::XCanvas& 					rCanvas, 
                                                                                const uno::Reference< rendering::XBitmap >& xBitmap,
                                                                                const rendering::ViewState& 				viewState,
                                                                                const rendering::RenderState& 				renderState,
                                                                                bool 										bModulateColors )
    {
	if( mpCairo )
        {
	    useStates( viewState, renderState, true );

	    ::cairo::Cairo* pCairo = tools::cairoFromXBitmap( xBitmap );
	    if( pCairo ) {
		::cairo::Surface* pSurface = ::cairo::cairo_get_target( pCairo );

		printf ("draw bitmap: paint surface %p on surface %p\n", pSurface, ::cairo::cairo_get_target( mpCairo ) );

		::cairo::cairo_save( mpCairo );
		::cairo::cairo_set_source_surface( mpCairo, pSurface, 0, 0 );
		::cairo::cairo_paint( mpCairo );
		::cairo::cairo_restore( mpCairo );
	    } else {
		BitmapEx aBmpEx = tools::bitmapExFromXBitmap(xBitmap);
		Bitmap aBitmap = aBmpEx.GetBitmap();
		AlphaMask aAlpha = aBmpEx.GetAlpha();

		BitmapReadAccess*	pBitmapReadAcc = aBitmap.AcquireReadAccess();
		BitmapReadAccess*	pAlphaReadAcc = aAlpha.AcquireReadAccess();

		const long		nWidth = pBitmapReadAcc->Width();
		const long		nHeight = pBitmapReadAcc->Height();
		long nX, nY;

		unsigned char* data = (unsigned char*) malloc( nWidth*nHeight*4 );
		long nOff = 0;
		Color aColor;
		unsigned int nAlpha;

		for( nY = 0; nY < nHeight; nY++ )
		    for( nX = 0; nX < nWidth; nX++ ) {
			nAlpha = 255 - pAlphaReadAcc->GetColor( nY, nX ).GetBlue();
			aColor = pBitmapReadAcc->GetColor( nY, nX );

			// cairo need premultiplied color values
			// TODO(rodo) handle endianess
			data [nOff++] = ( nAlpha*aColor.GetBlue() )/255;
			data [nOff++] = ( nAlpha*aColor.GetGreen() )/255;
			data [nOff++] = ( nAlpha*aColor.GetRed() )/255;
			data [nOff++] = nAlpha;
		    }

		::cairo::Surface* pImageSurface = ::cairo::cairo_image_surface_create_for_data( data, ::cairo::CAIRO_FORMAT_ARGB32, nWidth, nHeight, nWidth*4 );

		::cairo::cairo_save( mpCairo );
		::cairo::cairo_set_source_surface( mpCairo, pImageSurface, 0, 0 );
		::cairo::cairo_paint( mpCairo );
		::cairo::cairo_restore( mpCairo );

		::cairo::cairo_surface_destroy( pImageSurface );
		free( data );
	    }
	}

        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawBitmap( const rendering::XCanvas& 					rCanvas, 
                                                                            const uno::Reference< rendering::XBitmap >& xBitmap,
                                                                            const rendering::ViewState& 				viewState,
                                                                            const rendering::RenderState& 				renderState )
    {
        return implDrawBitmap( rCanvas, 
                               xBitmap,
                               viewState,
                               renderState,
                               false );
    }

    uno::Reference< rendering::XCachedPrimitive > CanvasHelper::drawBitmapModulated( const rendering::XCanvas& 						rCanvas, 
                                                                                     const uno::Reference< rendering::XBitmap >& 	xBitmap,
                                                                                     const rendering::ViewState& 					viewState,
                                                                                     const rendering::RenderState& 					renderState )
    {
        return implDrawBitmap( rCanvas, 
                               xBitmap,
                               viewState,
                               renderState,
                               true );
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

	geometry::RealSize2D aSize = mxDevice->getSize();

        return geometry::IntegerSize2D( (sal_Int32) aSize.Width, (sal_Int32) aSize.Height );
    }

    uno::Reference< rendering::XBitmap > CanvasHelper::getScaledBitmap( const geometry::RealSize2D& newSize, 
                                                                        sal_Bool 					beFast )
    {
	// rodo FIXME
	printf ("called CanvasHelper::getScaledBitmap, we return NULL, TODO\n");
            return uno::Reference< rendering::XBitmap >(); // we're disposed
//         if( !mpOutDev.get() )
//             return uno::Reference< rendering::XBitmap >(); // we're disposed

//         // TODO(F2): Support alpha vdev canvas here
//         const Point aEmptyPoint(0,0);
//         const Size  aBmpSize( mpOutDev->getOutDev().GetOutputSizePixel() );

//         Bitmap aBitmap( mpOutDev->getOutDev().GetBitmap(aEmptyPoint, aBmpSize) );

//         aBitmap.Scale( ::vcl::unotools::sizeFromRealSize2D(newSize), 
//                        beFast ? BMP_SCALE_FAST : BMP_SCALE_INTERPOLATE );

//         return uno::Reference< rendering::XBitmap >( new CanvasBitmap( aBitmap,
//                                                                        mxDevice ) );
    }

    uno::Sequence< sal_Int8 > CanvasHelper::getData( const geometry::IntegerRectangle2D& rect )
    {
	printf ("CanvasHelper::getData returns empty sequence, TODO\n");
//         if( !mpOutDev.get() )
//             return uno::Sequence< sal_Int8 >(); // we're disposed

//         // TODO(F2): Support alpha canvas here
//         const Rectangle	aRect( ::vcl::unotools::rectangleFromIntegerRectangle2D(rect) );

//         Bitmap aBitmap( mpOutDev->getOutDev().GetBitmap(aRect.TopLeft(), 
//                                                         aRect.GetSize()) );

//         ScopedBitmapReadAccess pReadAccess( aBitmap.AcquireReadAccess(),
//                                             aBitmap );

//         if( pReadAccess.get() != NULL )
//         {
//             // TODO(F1): Support more formats.
//             const sal_Int32 nWidth( rect.X2 - rect.X1 );
//             const sal_Int32 nHeight( rect.Y2 - rect.Y1 );

//             // for the time being, always return as BGRA 
//             uno::Sequence< sal_Int8 > aRes( 4*nWidth*nHeight );
//             sal_Int8* pRes = aRes.getArray();

//             int nCurrPos(0);
//             for( int y=0; y<nHeight; ++y )
//             {
//                 for( int x=0; x<nWidth; ++x )
//                 {
//                     pRes[ nCurrPos++ ] = pReadAccess->GetColor( y, x ).GetBlue();
//                     pRes[ nCurrPos++ ] = pReadAccess->GetColor( y, x ).GetGreen();
//                     pRes[ nCurrPos++ ] = pReadAccess->GetColor( y, x ).GetRed();
//                     pRes[ nCurrPos++ ] = (sal_Int8)255L;
//                 }
//             }

//             return aRes;
//         }

        return uno::Sequence< sal_Int8 >();
    }

    void CanvasHelper::setData( const uno::Sequence< sal_Int8 >& 	data, 
                                const geometry::IntegerRectangle2D&	rect )
    {
	printf ("canvas helper set data called, TODO\n");
//         if( !mpOutDev.get() )
//             return; // we're disposed

//         OutputDevice& rOutDev( mpOutDev->getOutDev() );

//         const Rectangle 		aRect( ::vcl::unotools::rectangleFromIntegerRectangle2D(rect) );
//         const USHORT 			nBitCount( ::std::min( (USHORT)24U, 
//                                                        (USHORT)rOutDev.GetBitCount() ) );
//         const BitmapPalette*	pPalette = NULL;

//         if( nBitCount <= 8 )
//         {
//             // TODO(Q1): Extract this to a common place, e.g. GraphicDevice

//             // try to determine palette from output device (by
//             // extracting a 1,1 bitmap, and querying it)
//             const Point aEmptyPoint;
//             const Size  aSize(1,1);
//             Bitmap aTmpBitmap( rOutDev.GetBitmap( aEmptyPoint,
//                                                   aSize ) );
            
//             ScopedBitmapReadAccess pReadAccess( aTmpBitmap.AcquireReadAccess(),
//                                                 aTmpBitmap );

//             pPalette = &pReadAccess->GetPalette();
//         }

//         // TODO(F2): Support alpha canvas here
//         Bitmap aBitmap( aRect.GetSize(), nBitCount, pPalette );

//         bool bCopyBack( false ); // only copy something back, if we
//                                  // actually changed some pixel

//         {
//             ScopedBitmapWriteAccess pWriteAccess( aBitmap.AcquireWriteAccess(),
//                                                   aBitmap );
            
//             if( pWriteAccess.get() != NULL )
//             {
//                 // for the time being, always read as BGRA 
//                 const sal_Int32 nWidth( rect.X2 - rect.X1 );
//                 const sal_Int32 nHeight( rect.Y2 - rect.Y1 );
//                 int x, y, nCurrPos(0);
//                 for( y=0; y<nHeight; ++y )
//                 {
//                     switch( pWriteAccess->GetScanlineFormat() )
//                     {
//                         case BMP_FORMAT_8BIT_PAL:
//                         {
//                             Scanline pScan = pWriteAccess->GetScanline( y );

//                             for( x=0; x<nWidth; ++x )
//                             {
//                                 *pScan++ = (BYTE)pWriteAccess->GetBestPaletteIndex(
//                                     BitmapColor( data[ nCurrPos+2 ],
//                                                  data[ nCurrPos+1 ],
//                                                  data[ nCurrPos ] ) );

//                                 nCurrPos += 4; // skip three colors, _plus_ alpha
//                             }
//                         }
//                         break;

//                         case BMP_FORMAT_24BIT_TC_BGR:
//                         {
//                             Scanline pScan = pWriteAccess->GetScanline( y );

//                             for( x=0; x<nWidth; ++x )
//                             {
//                                 Scanline pTmp = pScan + x * 3;

//                                 *pScan++ = data[ nCurrPos   ];
//                                 *pScan++ = data[ nCurrPos+1 ];
//                                 *pScan++ = data[ nCurrPos+2 ];

//                                 nCurrPos += 4; // skip three colors, _plus_ alpha
//                             }
//                         }
//                         break;

//                         case BMP_FORMAT_24BIT_TC_RGB:
//                         {
//                             Scanline pScan = pWriteAccess->GetScanline( y );

//                             for( x=0; x<nWidth; ++x )
//                             {
//                                 Scanline pTmp = pScan + x * 3;

//                                 *pScan++ = data[ nCurrPos+2 ];
//                                 *pScan++ = data[ nCurrPos+1 ];
//                                 *pScan++ = data[ nCurrPos   ];

//                                 nCurrPos += 4; // skip three colors, _plus_ alpha
//                             }
//                         }
//                         break;

//                         default:
//                         {
//                             for( x=0; x<nWidth; ++x )
//                             {
//                                 pWriteAccess->SetPixel( y, x, BitmapColor( data[ nCurrPos+2 ],
//                                                                            data[ nCurrPos+1 ],
//                                                                            data[ nCurrPos ] ) );
//                                 nCurrPos += 4; // skip three colors, _plus_ alpha
//                             }
//                         }
//                         break;
//                     }
//                 }

//                 bCopyBack = true;
//             }
//         }

//         // copy back only here, since the BitmapAccessors must be
//         // destroyed beforehand
//         if( bCopyBack )
//         {
//             tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

//             // TODO(F2): Support alpha canvas here
//             rOutDev.EnableMapMode( FALSE );
//             rOutDev.DrawBitmap(aRect.TopLeft(), aBitmap);
//         }
    }

    void CanvasHelper::setPixel( const uno::Sequence< sal_Int8 >& 	color, 
                                 const geometry::IntegerPoint2D& 	pos )
    {
	// rodo TODO
	printf ("CanvasHelper::setPixel called TODO\n");
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
	printf ("CanvasHelper::getPixel called TODO\n");
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

	printf ("CanvasHelper::getMemoryLayout called, TODO\n");

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

	printf ("repaint called, TODO\n");

//         if( !mpOutDev )
//             return false; // disposed
//         else
//             return rGrf->Draw( &mpOutDev->getOutDev(), rPt, rSz, &rAttr );
    }
}
