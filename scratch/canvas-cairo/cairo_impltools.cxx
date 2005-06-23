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

#ifndef  _USE_MATH_DEFINES
#define  _USE_MATH_DEFINES  // needed by Visual C++ for math constants
#endif
#include <math.h>           // M_PI definition 

#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#ifndef _COM_SUN_STAR_GEOMETRY_REALSIZE2D_HPP__
#include <com/sun/star/geometry/RealSize2D.hpp>
#endif
#ifndef _COM_SUN_STAR_GEOMETRY_REALPOINT2D_HPP__
#include <com/sun/star/geometry/RealPoint2D.hpp>
#endif
#ifndef _COM_SUN_STAR_GEOMETRY_REALRECTANGLE2D_HPP__
#include <com/sun/star/geometry/RealRectangle2D.hpp>
#endif

#ifndef _COM_SUN_STAR_RENDERING_RENDERSTATE_HPP__
#include <com/sun/star/rendering/RenderState.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XCANVAS_HPP__
#include <com/sun/star/rendering/XCanvas.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XBITMAP_HPP__
#include <com/sun/star/rendering/XBitmap.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XPOLYPOLYGON2D_HPP__
#include <com/sun/star/rendering/XPolyPolygon2D.hpp>
#endif
#ifndef _COM_SUN_STAR_GEOMETRY_REALBEZIERSEGMENT2D_HPP__
#include <com/sun/star/geometry/RealBezierSegment2D.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XINTEGERBITMAP_HPP__
#include <com/sun/star/rendering/XIntegerBitmap.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#ifndef _SV_SALBTYPE_HXX
#include <vcl/salbtype.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif
#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif
#ifndef _SV_METRIC_HXX
#include <vcl/metric.hxx>
#endif
#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_TUPLE_B2DTUPLE_HXX
#include <basegfx/tuple/b2dtuple.hxx>
#endif
#ifndef _BGFX_RANGE_B2DRECTANGLE_HXX
#include <basegfx/range/b2drectangle.hxx>
#endif
#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#include <canvas/canvastools.hxx>

#include "cairo_impltools.hxx"
#include "cairo_linepolypolygon.hxx"
#include "cairo_canvasbitmap.hxx"

#include <numeric>


using namespace ::com::sun::star;
using namespace ::cairo;

namespace vclcanvas
{
    namespace tools
    {
        ::basegfx::B2DPolyPolygon polyPolygonFromXPolyPolygon2D( const uno::Reference< rendering::XPolyPolygon2D >& xPoly )
        {    
            LinePolyPolygon* pPolyImpl = dynamic_cast< LinePolyPolygon* >( xPoly.get() );

            if( pPolyImpl )
            {
                return pPolyImpl->getPolyPolygon();
            }
            else
            {
                const sal_Int32 nPolys( xPoly->getNumberOfPolygons() );

                // not a known implementation object - try data source
                // interfaces
                uno::Reference< rendering::XBezierPolyPolygon2D > xBezierPoly(
                    xPoly,
                    uno::UNO_QUERY );

                if( xBezierPoly.is() )
                {
                    return ::basegfx::unotools::polyPolygonFromBezier2DSequenceSequence(
                        xBezierPoly->getBezierSegments( 0,
                                                        nPolys,
                                                        0,
                                                        -1 ) );
                }
                else
                {
                    uno::Reference< rendering::XLinePolyPolygon2D > xLinePoly(
                        xPoly,
                        uno::UNO_QUERY );

                    // no implementation class and no data provider
                    // found - contract violation.
                    CHECK_AND_THROW( xLinePoly.is(),
                                     "CairoCanvas::polyPolygonFromXPolyPolygon2D(): Invalid input "
                                     "poly-polygon, cannot retrieve vertex data" );

                    return ::basegfx::unotools::polyPolygonFromPoint2DSequenceSequence(
                        xLinePoly->getPoints( 0,
                                              nPolys,
                                              0,
                                              -1 ) );
                }
            }
        }

        Cairo* cairoFromXBitmap( const uno::Reference< rendering::XBitmap >& xBitmap )
        {
            uno::Reference< lang::XServiceInfo > xRef( xBitmap, 
                                                       uno::UNO_QUERY );

            if( xRef.is() && 
                xRef->getImplementationName().equals( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(CANVASBITMAP_IMPLEMENTATION_NAME))) )
	    {
                // TODO(Q1): Maybe use dynamic_cast here
                return static_cast<CanvasBitmap*>(xBitmap.get())->getCairo();
            }
 
            return NULL;
        }

        ::BitmapEx bitmapExFromXBitmap( const uno::Reference< rendering::XBitmap >& xBitmap )
        {
	    uno::Reference< lang::XUnoTunnel > xTunnel( xBitmap, uno::UNO_QUERY );
	    if( xTunnel.is() )
            {
		sal_Int64 nPtr = xTunnel->getSomething( vcl::unotools::getTunnelIdentifier( vcl::unotools::Id_BitmapEx ) );
		if( nPtr )
		    return BitmapEx( *(BitmapEx*)nPtr );
	    }

	    // TODO(F1): extract pixel from XBitmap interface
	    ENSURE_AND_THROW( false, 
			      "bitmapExFromXBitmap(): could not extract BitmapEx" );
 
            return ::BitmapEx();
        }

        Surface* surfaceFromXBitmap( const uno::Reference< rendering::XBitmap >& xBitmap, unsigned char*& data )
        {
	    Surface* pSurface = NULL;
	    Cairo* pCairo = cairoFromXBitmap( xBitmap );
	    if( pCairo ) {
		pSurface = cairo_get_target( pCairo );
		cairo_surface_reference( pSurface );
		data = NULL;
	    } else {
		BitmapEx aBmpEx = bitmapExFromXBitmap(xBitmap);
		Bitmap aBitmap = aBmpEx.GetBitmap();
		AlphaMask aAlpha = aBmpEx.GetAlpha();

		BitmapReadAccess*	pBitmapReadAcc = aBitmap.AcquireReadAccess();
		BitmapReadAccess*	pAlphaReadAcc = aAlpha.AcquireReadAccess();

		const long		nWidth = pBitmapReadAcc->Width();
		const long		nHeight = pBitmapReadAcc->Height();
		long nX, nY;

		data = (unsigned char*) malloc( nWidth*nHeight*4 );
		long nOff = 0;
		Color aColor;
		unsigned int nAlpha = 255;

		for( nY = 0; nY < nHeight; nY++ )
		    for( nX = 0; nX < nWidth; nX++ ) {
			if( pAlphaReadAcc)
			    nAlpha = 255 - pAlphaReadAcc->GetColor( nY, nX ).GetBlue();
			aColor = pBitmapReadAcc->GetColor( nY, nX );

			// cairo need premultiplied color values
			// TODO(rodo) handle endianess
			data [nOff++] = ( nAlpha*aColor.GetBlue() )/255;
			data [nOff++] = ( nAlpha*aColor.GetGreen() )/255;
			data [nOff++] = ( nAlpha*aColor.GetRed() )/255;
			data [nOff++] = nAlpha;
		    }

		pSurface = cairo_image_surface_create_for_data( data, CAIRO_FORMAT_ARGB32, nWidth, nHeight, nWidth*4 );
	    }

	    return pSurface;
	}

        bool setupFontTransform( ::Point&						o_rPoint,
                                 ::Font& 						io_rVCLFont, 
                                 const rendering::ViewState& 	rViewState,
                                 const rendering::RenderState& 	rRenderState,
                                 ::OutputDevice&				rOutDev )
        {
            ::basegfx::B2DHomMatrix aMatrix;
            
            ::canvas::tools::mergeViewAndRenderTransform(aMatrix,
                                                         rViewState,
                                                         rRenderState);

            ::basegfx::B2DTuple aScale;
            ::basegfx::B2DTuple aTranslate;
            double nRotate, nShearX;
            
            aMatrix.decompose( aScale, aTranslate, nRotate, nShearX );

            // query font metric _before_ tampering with width and height
            if( !::rtl::math::approxEqual(aScale.getX(), aScale.getY()) )
            {
                // retrieve true font width
                const int nFontWidth( rOutDev.GetFontMetric( io_rVCLFont ).GetWidth() );

                const int nScaledFontWidth( ::basegfx::fround(nFontWidth * aScale.getX()) );

                if( !nScaledFontWidth )
                {
                    // scale is smaller than one pixel - disable text
                    // output altogether
                    return false;
                }

                io_rVCLFont.SetWidth( nScaledFontWidth );
            }

            if( !::rtl::math::approxEqual(aScale.getY(), 1.0) )
            {
                const int nFontHeight( io_rVCLFont.GetHeight() );
                io_rVCLFont.SetHeight( ::basegfx::fround(nFontHeight * aScale.getY()) );
            }

            io_rVCLFont.SetOrientation( static_cast< short >( ::basegfx::fround(-fmod(nRotate, 2*M_PI)*(1800.0/M_PI)) ) );

            // TODO(F2): Missing functionality in VCL: shearing
            o_rPoint.X() = ::basegfx::fround(aTranslate.getX());
            o_rPoint.Y() = ::basegfx::fround(aTranslate.getY());

            return true;
        }

        bool isPolyPolygonEqualRectangle( const PolyPolygon& rPolyPoly,
            							  const Rectangle&	 rRect )
        {
            // exclude some cheap cases first
            if( rPolyPoly.Count() != 1 )
                return false;

            const ::Polygon& rPoly( rPolyPoly[0] );

            USHORT nCount( rPoly.GetSize() );
            if( nCount != 4 && nCount != 5 )
                return false;

            // fill array with rectangle vertices
            const ::Point aPoints[4] = 
              {
                  rRect.TopLeft(),
                  rRect.TopRight(),
                  rRect.BottomRight(),
                  rRect.BottomLeft()
              };

            // now match polygon and rectangle start points, to
            // facilitate point-by-point comparison
            const ::Point* 	     aIter;
            const ::Point* const aEnd( &aPoints[5] );
            if( (aIter=::std::find( aPoints, aEnd,
                                    rPoly[0] )) == aEnd )
                return false; // point not found

            // determine index from iterator
            const ::std::size_t nIndexOfFirstPoint( aIter - aPoints );

            bool bNotMatching( false ); // when true, at least on
                                        // point does not match

            // start point found, now try forward sweep to match
            // points
            for( USHORT i=0; i<4; ++i )
            {
                if( rPoly[i] != aPoints[ (i+nIndexOfFirstPoint)%4 ] )
                {
                    bNotMatching = true;
                    break;
                }
            } 

            if( !bNotMatching )
                return true; // all points match, done

            // at least one point doesn't match, try reverse sweep to
            // match points
            for( USHORT i=0; i<4; ++i )
            {
                if( rPoly[i] != aPoints[ (4-i+nIndexOfFirstPoint)%4 ] )
                    return false; // nothing more to try, exit directly
            } 

            // all points for reverse sweep match
            return true;
        }


        // VCL-Canvas related
        //---------------------------------------------------------------------

        ::Point mapRealPoint2D( const geometry::RealPoint2D& 	rPoint,
                                const rendering::ViewState& 	rViewState,
                                const rendering::RenderState&	rRenderState )
        {
            ::basegfx::B2DPoint aPoint( ::basegfx::unotools::b2DPointFromRealPoint2D(rPoint) );

            ::basegfx::B2DHomMatrix aMatrix;
            aPoint *= ::canvas::tools::mergeViewAndRenderTransform(aMatrix,
                                                                   rViewState,
                                                                   rRenderState);

            return ::vcl::unotools::pointFromB2DPoint( aPoint );
        }

        ::PolyPolygon mapPolyPolygon( const ::basegfx::B2DPolyPolygon& 	rPoly,
                                      const rendering::ViewState& 		rViewState,
                                      const rendering::RenderState&		rRenderState )
        {
            ::basegfx::B2DHomMatrix aMatrix;
            ::canvas::tools::mergeViewAndRenderTransform(aMatrix,
                                                         rViewState,
                                                         rRenderState);

            ::basegfx::B2DPolyPolygon aTemp( rPoly );

            aTemp.transform( aMatrix );

            return ::PolyPolygon( aTemp );
        }
    }
}
