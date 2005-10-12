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
#include <canvas/verbosetrace.hxx>

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif
#ifndef _SV_ALPHA_HXX
#include <vcl/alpha.hxx>
#endif
#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif
#ifndef _SV_SYSDATA_HXX
#include <vcl/sysdata.hxx>
#endif
#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
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
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#include <canvas/canvastools.hxx>

#include "cairo_canvascustomsprite.hxx"


using namespace ::com::sun::star;
using namespace ::cairo;

namespace cairocanvas
{

    CanvasCustomSprite::CanvasCustomSprite( const geometry::RealSize2D& 		rSpriteSize,
                                            const WindowGraphicDevice::ImplRef&	rDevice,
					    const SystemEnvData* pSysData,
                                            const SpriteCanvas::ImplRef&		rSpriteCanvas ) :
        mpBackBuffer(),
        mpBackBufferMask(),
        mpSpriteCanvas( rSpriteCanvas ),
        maCurrClipBounds(),
        maPosition(0.0, 0.0),
        maSize( static_cast< sal_Int32 >( 
                    ::std::max( 1.0,
                                ceil( rSpriteSize.Width ) ) ), // round up to nearest int, 
                											   // enforce sprite to have at
                											   // least (1,1) pixel size
                static_cast< sal_Int32 >( 
                    ::std::max( 1.0,
                                ceil( rSpriteSize.Height ) ) ) ),
        maTransform(),
        mxClipPoly(),
        mfAlpha(0.0),
        mbActive(false),
        mbIsCurrClipRectangle(true),
        mbIsContentFullyOpaque( false ),
        mbTransformDirty( true ),
	mxDevice( rDevice )
    {
        ENSURE_AND_THROW( rDevice.get() && rSpriteCanvas.get(),
                          "CanvasBitmap::CanvasBitmap(): Invalid device or sprite canvas" );

	OSL_TRACE("going to create canvas custom sprite\n");

        // setup graphic device
        maCanvasHelper.setGraphicDevice( mxDevice );
	setContent( CAIRO_CONTENT_COLOR_ALPHA );

        // create mask backbuffer, with one bit color depth
        mpBackBufferMask.reset( new BackBuffer( mxDevice ) );
	mpBackBufferMask->setSize( maSize );
    }

    CanvasCustomSprite::~CanvasCustomSprite()
    {
    }
    
    void SAL_CALL CanvasCustomSprite::disposing()
    {
        tools::LocalGuard aGuard;

        mpSpriteCanvas.clear();
        mxDevice.reset();

        // forward to parent
        CanvasCustomSprite_Base::disposing();
    }

    void CanvasCustomSprite::setContent( Content aContent )
    {
	OSL_TRACE("set Content of %p to %x\n", this, aContent);
	
	mpBackBuffer.reset( new BackBuffer( mxDevice, aContent ) );
	mpBackBuffer->setSize( maSize );
	maCanvasHelper.setCairo( mpBackBuffer->getCairo() );
	maContent = aContent;
    }

    void SAL_CALL CanvasCustomSprite::setAlpha( double alpha ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        if( !mpSpriteCanvas.get() )
            return; // we're disposed

        ::canvas::tools::checkRange(alpha, 0.0, 1.0);

        if( alpha != mfAlpha )
        {
            mfAlpha = alpha;        

            if( mbActive )
                mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                              ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                              getSpriteRect() );
        }
    }

    void SAL_CALL CanvasCustomSprite::move( const geometry::RealPoint2D&  aNewPos, 
                                            const rendering::ViewState&   viewState, 
                                            const rendering::RenderState& renderState ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

	OSL_TRACE ("custom sprite move to new pos %f, %f\n", aNewPos.X, aNewPos.Y);

        if( !mpSpriteCanvas.get() )
            return; // we're disposed

        ::basegfx::B2DHomMatrix aTransform;
        ::canvas::tools::mergeViewAndRenderTransform(aTransform,
                                                     viewState, 
                                                     renderState);

        ::basegfx::B2DPoint aPoint( ::basegfx::unotools::b2DPointFromRealPoint2D(aNewPos) );
        aPoint *= aTransform;

        if( aPoint != maPosition )
        {
            const ::Rectangle& 			rBounds( getFullSpriteRect() );
            const ::basegfx::B2DPoint& 	rOutPos( 
                ::vcl::unotools::b2DPointFromPoint( rBounds.TopLeft() ) );

            if( mbActive )
                mpSpriteCanvas->moveSprite( Sprite::ImplRef( this ), 
                                            rBounds.TopLeft(),
                                            ::vcl::unotools::pointFromB2DPoint( 
                                                rOutPos - maPosition + aPoint ),
                                            rBounds.GetSize() );

            maPosition = aPoint;
        }
    }

    void SAL_CALL CanvasCustomSprite::transform( const geometry::AffineMatrix2D& aTransformation ) throw (lang::IllegalArgumentException, 
                                                                                                          uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        ::basegfx::B2DHomMatrix aMatrix;
		::basegfx::unotools::homMatrixFromAffineMatrix(aMatrix,
                                                       aTransformation);

        if( maTransform != aMatrix )
        {
            // retrieve bounds before and after transformation change,
            // and calc union of them, as the resulting update area.
            Rectangle aPrevBounds( 
                getSpriteRect() );

            maTransform = aMatrix;

            aPrevBounds.Union( getSpriteRect() );

            if( mbActive )
                mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                              ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                              aPrevBounds );

            mbTransformDirty = true;
        }
    }

	namespace
	{
        /** This method computes the up to four rectangles that make
            up the set difference between two rectangles.

            @param a
            First rect

            @param b
            Second rect

            @param out
            Pointer to an array of at least four elements, herein, the
            resulting parts of the set difference are returned.

            @return
            The number of rectangles written to the out array.
         */
		unsigned int b2dRectComputeSetDifference( const ::basegfx::B2DRectangle&	a,
                                                  const ::basegfx::B2DRectangle&	b,
                                                  ::basegfx::B2DRectangle* 			out )
		{
            OSL_ENSURE( out != NULL,
                        "b2dRectComputeSetDifference(): Invalid output array" );

            // special-casing the empty rect case (this will fail most
            // of the times below, because of the DBL_MIN/MAX special
            // values denoting emptyness in the rectangle.
            if( a.isEmpty() )
            {
                *out = b;
                return 1;
            }
            if( b.isEmpty() )
            {
                *out = a;
                return 1;
            }

			unsigned int num_rectangles = 0;

			double ax(a.getMinX());
			double ay(a.getMinY());
			double aw(a.getWidth());
			double ah(a.getHeight());
			double bx(b.getMinX());
			double by(b.getMinY());
			double bw(b.getWidth());
			double bh(b.getHeight());

			double h0 = (by > ay) ? by - ay : 0.0;
			double h3 = (by + bh < ay + ah) ? ay + ah - by - bh : 0.0;
			double w1 = (bx > ax) ? bx - ax : 0.0;
			double w2 = (ax + aw > bx + bw) ? ax + aw - bx - bw : 0.0;
			double h12 = (h0 + h3 < ah) ? ah - h0 - h3 : 0.0;

			if (h0 > 0)
				out[num_rectangles++] = ::basegfx::B2DRectangle(ax,ay,ax+aw,ay+h0);

			if (w1 > 0 && h12 > 0)
				out[num_rectangles++] = ::basegfx::B2DRectangle(ax,ay+h0,ax+w1,ay+h0+h12);

			if (w2 > 0 && h12 > 0)
				out[num_rectangles++] = ::basegfx::B2DRectangle(bx+bw,ay+h0,bx+bw+w2,ay+h0+h12);

			if (h3 > 0)
				out[num_rectangles++] = ::basegfx::B2DRectangle(ax,ay+h0+h12,ax+aw,ay+h0+h12+h3);

			return num_rectangles;
		}
	}

    void SAL_CALL CanvasCustomSprite::clip( const uno::Reference< rendering::XPolyPolygon2D >& xClip ) throw (uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        mxClipPoly = xClip;

        if( mbActive )
		{
            const sal_Int32 nNumClipPolygons( mxClipPoly->getNumberOfPolygons() );

            if( !mxClipPoly.is() || 
                nNumClipPolygons == 0 )
            {
                // empty clip polygon -> everything is visible now
                maCurrClipBounds.reset();
                mbIsCurrClipRectangle = true;
            }
            else 
            {
                // new clip is not empty - determine actual update
                // area
                const ::basegfx::B2DPolyPolygon& rClipPath(
                    tools::polyPolygonFromXPolyPolygon2D( mxClipPoly ) );
                
                // clip which is about to be set, expressed as a
                // b2drectangle
                const ::basegfx::B2DRectangle& rClipBounds(
                    ::basegfx::tools::getRange( rClipPath ) );

                const ::basegfx::B2DRectangle aBounds( 0.0, 0.0,
                                                       maSize.Width(),
                                                       maSize.Height() );
                
                // rectangular area which is actually covered by the sprite.
                // coordinates are relative to the sprite origin.
                ::basegfx::B2DRectangle aSpriteRectPixel;
                ::canvas::tools::calcTransformedRectBounds( aSpriteRectPixel,
                                                            aBounds,
                                                            maTransform );
                
                // aClipBoundsA = new clip [set intersection] sprite
                ::basegfx::B2DRectangle aClipBoundsA(rClipBounds);
                aClipBoundsA.intersect( aSpriteRectPixel );

                if( nNumClipPolygons != 1 )
                {
                    // new clip cannot be a single rectangle -> cannot
                    // optimize update
                    mbIsCurrClipRectangle = false;
                    maCurrClipBounds = aClipBoundsA;
                }
                else
                {
                    // new clip could be a single rectangle
                    const bool bNewClipIsRect( 
                        ::basegfx::tools::isRectangle( rClipPath.getB2DPolygon(0) ) );
                
                    // both new and old clip are truly rectangles
                    // - can now take the optimized path
                    const bool bUseOptimizedUpdate( bNewClipIsRect &&
                                                    mbIsCurrClipRectangle );

                    const ::basegfx::B2DRectangle aOldBounds( maCurrClipBounds );

                    // store new current clip type
                    maCurrClipBounds = aClipBoundsA;
                    mbIsCurrClipRectangle = bNewClipIsRect;

                    if( bUseOptimizedUpdate  )
                    {
                        // aClipBoundsB = maCurrClipBounds, i.e. last clip [set intersection] sprite
                        ::basegfx::B2DRectangle aClipDifference[4];
                        const unsigned int num_rectangles(
                            b2dRectComputeSetDifference(aClipBoundsA,
                                                        aOldBounds,
                                                        aClipDifference) );
                    
                        for(unsigned int i=0; i<num_rectangles; ++i)
                        {
                            // aClipDifference[i] now contains the final
                            // update area, coordinates are still relative
                            // to the sprite origin. before submitting
                            // this area to 'updateSprite()' we need to
                            // translate this area to the final position,
                            // coordinates need to be relative to the
                            // spritecanvas.
                            mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                                          ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                                          Rectangle( static_cast< sal_Int32 >( maPosition.getX() + aClipDifference[i].getMinX() ),
                                                                     static_cast< sal_Int32 >( maPosition.getY() + aClipDifference[i].getMinY() ),
                                                                     static_cast< sal_Int32 >( ceil( maPosition.getX() + aClipDifference[i].getMaxX() ) ),
                                                                     static_cast< sal_Int32 >( ceil( maPosition.getY() + aClipDifference[i].getMaxY() ) ) ) );
                        }

                        // early exit, needed to process the four
                        // difference rects independently.
                        return;
                    }
                }
            }
                    
            mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                          ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                          getSpriteRect() );
        }
    }

    void SAL_CALL CanvasCustomSprite::setPriority( double nPriority ) throw (uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        // TODO(F2): Implement sprite priority
    }

    void SAL_CALL CanvasCustomSprite::show() throw (uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        if( !mpSpriteCanvas.get() )
            return; // we're disposed

        if( !mbActive )
        {
            mpSpriteCanvas->showSprite( Sprite::ImplRef( this ) );
            mbActive = true;
            
            if( mfAlpha != 0.0 )
            {
                mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                              ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                              getSpriteRect() );
            }
        }
    }

    void SAL_CALL CanvasCustomSprite::hide() throw (uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        if( !mpSpriteCanvas.get() )
            return; // we're disposed

        if( mbActive )
        {
            mpSpriteCanvas->hideSprite( Sprite::ImplRef( this ) );
            mbActive = false;
            
            if( mfAlpha != 0.0 )
            {
                mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                              ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                              getSpriteRect() );
            }
        }
    }

    uno::Reference< rendering::XCanvas > SAL_CALL CanvasCustomSprite::getContentCanvas() throw (uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

        if( !mpSpriteCanvas.get() || !mpBackBuffer.get() )
            uno::Reference< rendering::XCanvas >(); // we're disposed

        if( mbActive )
        {
            // the client is about to render into the sprite. Thus,
            // potentially the whole sprite area has changed.
            mpSpriteCanvas->updateSprite( Sprite::ImplRef( this ), 
                                          ::vcl::unotools::pointFromB2DPoint( maPosition ),
                                          getSpriteRect() );
        }

        // clear surface
// 	Cairo* pCairo = mpBackBuffer->getCairo();
// 	cairo_save( pCairo );
// 	cairo_set_source_rgb( pCairo, 1, 1, 1 );
// 	cairo_rectangle( pCairo, 0, 0, maSize.Width(), maSize.Height() );
// 	cairo_fill( pCairo );
// 	cairo_restore( pCairo );

	// rodo TODO
//         OutputDevice& rMaskOutDev( mpBackBufferMask->getOutDev() );
//         rMaskOutDev.SetDrawMode( DRAWMODE_DEFAULT );
//         rMaskOutDev.EnableMapMode( FALSE );
//         rMaskOutDev.SetFillColor( Color( COL_WHITE ) );
//         rMaskOutDev.SetLineColor();
//         rMaskOutDev.DrawRect( Rectangle(Point(), maSize) );
//         rMaskOutDev.SetDrawMode( DRAWMODE_BLACKLINE | DRAWMODE_BLACKFILL | DRAWMODE_BLACKTEXT |
//                                  DRAWMODE_BLACKGRADIENT | DRAWMODE_BLACKBITMAP );

        // surface content has changed (we cleared it, at least)
        mbSurfaceDirty = true;

        // just cleared content to fully transparent
        mbIsContentFullyOpaque = false;

        return this;
    }

#define SERVICE_NAME "com.sun.star.rendering.CanvasCustomSprite"

    ::rtl::OUString SAL_CALL CanvasCustomSprite::getImplementationName() throw( uno::RuntimeException )
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CANVASCUSTOMSPRITE_IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL CanvasCustomSprite::supportsService( const ::rtl::OUString& ServiceName ) throw( uno::RuntimeException )
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL CanvasCustomSprite::getSupportedServiceNames()  throw( uno::RuntimeException )
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );
        
        return aRet;
    }

    // Sprite
    void CanvasCustomSprite::redraw( Cairo* pCairo ) const
    {
        tools::LocalGuard aGuard;

        redraw( pCairo, maPosition );
    }

    bool CanvasCustomSprite::doesBitmapCoverWholeSprite( const uno::Reference< rendering::XBitmap >& xBitmap, 
							 const rendering::ViewState& viewState, 
							 const rendering::RenderState& renderState )
    {
	// TODO(Q2): Factor out to canvastools or similar

	::basegfx::B2DHomMatrix aTransform;
	::canvas::tools::mergeViewAndRenderTransform(aTransform,
						     viewState, 
						     renderState);
            
	const geometry::IntegerSize2D& rSize( xBitmap->getSize() );

	::basegfx::B2DPolygon aPoly( 
				    ::basegfx::tools::createPolygonFromRect(
									    ::basegfx::B2DRectangle( 0.0,0.0,
												     rSize.Width+1,
												     rSize.Height+1 ) ) );
	aPoly.transform( aTransform );

	if( ::basegfx::tools::isInside( 
				       aPoly,
				       ::basegfx::tools::createPolygonFromRect(
									       ::basegfx::B2DRectangle( 0.0,0.0,
													maSize.Width(),
													maSize.Height() ) ),
				       true ) )
            {
                // bitmap will fully cover the sprite, set flag
                // appropriately
                mbIsContentFullyOpaque = true;
		OSL_TRACE ("sprite fully opaque");
            }
	return mbIsContentFullyOpaque;
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasCustomSprite::drawBitmap( const uno::Reference< rendering::XBitmap >&	xBitmap, 
                                                                                           const rendering::ViewState& viewState, 
                                                                                           const rendering::RenderState& renderState )
	throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        tools::LocalGuard aGuard;

	uno::Reference< rendering::XCachedPrimitive > rv;
	bool bHasAlpha;
	unsigned char* data;
	Surface* pSurface = tools::surfaceFromXBitmap( xBitmap, mxDevice, data, bHasAlpha );
	Content aRequiredContent;

	if( pSurface ) {
  	    aRequiredContent = bHasAlpha ? CAIRO_CONTENT_COLOR_ALPHA : CAIRO_CONTENT_COLOR;
   	    if( aRequiredContent != maContent && aRequiredContent == CAIRO_CONTENT_COLOR && doesBitmapCoverWholeSprite( xBitmap, viewState, renderState ) )
   		setContent( aRequiredContent );

	    rv = maCanvasHelper.implDrawBitmapSurface( pSurface, viewState, renderState, false, bHasAlpha );

	    cairo_surface_destroy( pSurface );

	    if( data )
		free( data );
	} else
	    rv = uno::Reference< rendering::XCachedPrimitive >(NULL);
           
	return rv;
    }

    void CanvasCustomSprite::redraw( Cairo* pCairo,
                                     const ::basegfx::B2DPoint& rOutputPos ) const
    {
        tools::LocalGuard aGuard;

	#ifdef CAIRO_CANVAS_PERF_TRACE
	struct timespec aTimer;
	mxDevice->startPerfTrace( &aTimer );
        #endif

	if( mbActive && !::basegfx::fTools::equalZero( mfAlpha ) ) {
	    OSL_TRACE ("CanvasCustomSprite::redraw called\n");
	    if( pCairo ) {

		Size aSize = mpBackBuffer->getSize();
		OSL_TRACE ("CanvasCustomSprite::redraw painting surface %p on %p cairo with surface %p on %f,%f size %d x %d\n",
			mpBackBuffer->getSurface(), pCairo, cairo_get_target( pCairo ), rOutputPos.getX(), rOutputPos.getY(),
			aSize.Width(), aSize.Height() );
		cairo_save( pCairo );

		double fX, fY;

		fX = rOutputPos.getX();
		fY = rOutputPos.getY();

 		if( !maTransform.isIdentity() ) {
 		    cairo_matrix_t aMatrix, aInverseMatrix;
 		    cairo_matrix_init( &aMatrix,
 				       maTransform.get( 0, 0 ), maTransform.get( 1, 0 ), maTransform.get( 0, 1 ),
 				       maTransform.get( 1, 1 ), maTransform.get( 0, 2 ), maTransform.get( 1, 2 ) );

		    aMatrix.x0 = round( aMatrix.x0 );
		    aMatrix.y0 = round( aMatrix.y0 );

		    cairo_matrix_init( &aInverseMatrix, aMatrix.xx, aMatrix.yx, aMatrix.xy, aMatrix.yy, aMatrix.x0, aMatrix.y0 );
		    cairo_matrix_invert( &aInverseMatrix );
		    cairo_matrix_transform_distance( &aInverseMatrix, &fX, &fY );


 		    cairo_set_matrix( pCairo, &aMatrix );
 		}

// 		fX = floor( fX );
// 		fY = floor( fY );

		cairo_matrix_t aOrigMatrix;
		cairo_get_matrix( pCairo, &aOrigMatrix );
		cairo_translate( pCairo, round(fX), round(fY) );
		if( mbIsCurrClipRectangle ) {
		    cairo_reset_clip( pCairo );
 		    if( ! maCurrClipBounds.isEmpty() ) {
  			cairo_rectangle( pCairo,
					 floor( maCurrClipBounds.getMinX() ),
					 floor( maCurrClipBounds.getMinY() ),
  					 ceil( maCurrClipBounds.getMaxX() - maCurrClipBounds.getMinX() ),
					 ceil( maCurrClipBounds.getMaxY() - maCurrClipBounds.getMinY() ) );
			cairo_clip( pCairo );
		    }
		    //else
		    //cairo_rectangle( pCairo, 0, 0, maSize.Width(), maSize.Height() );
		}
		if( mxClipPoly.is() ) {
		    cairo_reset_clip( pCairo );
		    maCanvasHelper.drawPolyPolygonPath( mxClipPoly, CanvasHelper::Clip, pCairo );
		}
		OSL_TRACE ("maSize %d x %d\n", maSize.Width(), maSize.Height() );
		cairo_rectangle( pCairo, 0, 0, ceil( maSize.Width() ), ceil( maSize.Height() ) );
		cairo_clip( pCairo );
		cairo_set_matrix( pCairo, &aOrigMatrix );

		if( mbIsContentFullyOpaque )
		    cairo_set_operator( pCairo, CAIRO_OPERATOR_SOURCE );
		cairo_set_source_surface( pCairo, mpBackBuffer->getSurface(), fX, fY );
                if( ::rtl::math::approxEqual(mfAlpha, 1.0) )
		    cairo_paint( pCairo );
		else
		    cairo_paint_with_alpha( pCairo, mfAlpha );

		cairo_restore( pCairo );
	    }
	}

	#ifdef CAIRO_CANVAS_PERF_TRACE
	mxDevice->stopPerfTrace( &aTimer, "sprite redraw" );
        #endif
    }

    bool CanvasCustomSprite::isAreaUpdateOpaque( const Rectangle& rUpdateArea ) const
    {
        if( !mbIsCurrClipRectangle || 
            !mbIsContentFullyOpaque ||
            !::rtl::math::approxEqual(mfAlpha, 1.0) )
        {
            // sprite either transparent, or clip rect does not
            // represent exact bounds -> update might not be fully
            // opaque
            return false;
        }
        else
        {
            const Rectangle& rSpriteRect( getSpriteRect() );

            // make sure sprite rect covers update area fully -
            // although the update area originates from the sprite,
            // it's by no means guaranteed that it's limited to this
            // sprite's update area - after all, other sprites might
            // have been merged, or this sprite is moving.

            // Note: as Rectangle::IsInside() checks for _strict_
            // insidedness (i.e. all rect edges must be strictly
            // inside, not equal to one of the spriteRect's edges),
            // need the check for equality here.
            return rSpriteRect == rUpdateArea || rSpriteRect.IsInside( rUpdateArea );
        }
    }

    ::basegfx::B2DPoint CanvasCustomSprite::getSpritePos() const
    {
        tools::LocalGuard aGuard;

        return maPosition;
    }

    ::basegfx::B2DSize CanvasCustomSprite::getSpriteSize() const
    {
        tools::LocalGuard aGuard;

        // TODO(Q1): Use AW's wrappers once resynced
        return ::basegfx::B2DSize( maSize.Width(), 
                                   maSize.Height() );
    }

    bool CanvasCustomSprite::repaint( const GraphicObjectSharedPtr&	rGrf,
                                      const ::Point& 				rPt, 
                                      const ::Size& 				rSz,
                                      const GraphicAttr&			rAttr ) const
    {
        tools::LocalGuard aGuard;

        mbSurfaceDirty = true;

        return maCanvasHelper.repaint( rGrf, rPt, rSz, rAttr );
    }

    Rectangle CanvasCustomSprite::getSpriteRect( const ::basegfx::B2DRectangle& rBounds ) const
    {
        // Internal! Only call with locked object mutex!
        ::basegfx::B2DHomMatrix aTransform( maTransform );
        aTransform.translate( maPosition.getX(),
                              maPosition.getY() );

        // transform bounds at origin, as the sprite transformation is
        // formulated that way
        ::basegfx::B2DRectangle aTransformedBounds;
        ::canvas::tools::calcTransformedRectBounds( aTransformedBounds,
                                                    rBounds,
                                                    aTransform );

        // return integer rect, rounded away from the center
        return Rectangle( static_cast< sal_Int32 >( aTransformedBounds.getMinX() ),
                          static_cast< sal_Int32 >( aTransformedBounds.getMinY() ),
                          static_cast< sal_Int32 >( aTransformedBounds.getMaxX() )+1,
                          static_cast< sal_Int32 >( aTransformedBounds.getMaxY() )+1 );
    }

    Rectangle CanvasCustomSprite::getSpriteRect() const
    {
        // Internal! Only call with locked object mutex!

        // return effective sprite rect, i.e. take active clip into
        // account
        if( maCurrClipBounds.isEmpty() )
            return getSpriteRect( ::basegfx::B2DRectangle( 0.0, 0.0,
                                                           maSize.Width(),
                                                           maSize.Height() ) );
        else
            // return integer rect, rounded away from the center
            return Rectangle( static_cast< sal_Int32 >( maPosition.getX() + maCurrClipBounds.getMinX() ),
                              static_cast< sal_Int32 >( maPosition.getY() + maCurrClipBounds.getMinY() ),
                              static_cast< sal_Int32 >( ceil( maPosition.getX() + maCurrClipBounds.getMaxX() ) ),
                              static_cast< sal_Int32 >( ceil( maPosition.getY() + maCurrClipBounds.getMaxY() ) ) );
    }

    Rectangle CanvasCustomSprite::getFullSpriteRect() const
    {
        // Internal! Only call with locked object mutex!
        return getSpriteRect( ::basegfx::B2DRectangle( 0.0, 0.0,
                                                       maSize.Width(),
                                                       maSize.Height() ) );
    }
}
