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

#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif
#ifndef _RTL_TEXTENC_H
#include <rtl/textenc.h>
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif

#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#ifndef BOOST_BIND_HPP_INCLUDED
#include <boost/bind.hpp>
#endif

#include <algorithm>
#include <functional>
#include <numeric>

#include "cairo_redrawmanager.hxx"


#define FPS_BOUNDS Rectangle(0,0,130,90)
#define INFO_COLOR COL_RED

using namespace ::cairo;

namespace vclcanvas
{

    class SpritePainter
    {
    public:
        SpritePainter( Cairo* pCairo )
        {
	    mpCairo = pCairo;
        }

        void operator()( const ::vclcanvas::Sprite::ImplRef& rSprite )
        {
            rSprite->redraw( mpCairo );
        }

    private:
	Cairo* mpCairo;
    };

    inline double accumulatePixel( double nPrevVal, const ::vclcanvas::Sprite::ImplRef& rSprite )
    {
        const ::basegfx::B2DSize& rSize( rSprite->getSpriteSize() );

        return nPrevVal + rSize.getX() * rSize.getY();
    }

    // Initial size of change records vector
    enum{ CHANGE_RECORDS_INITIAL_SIZE=4 };
    
    RedrawManager::RedrawManager( OutputDevice&					rOutDev,
                                  const BackBufferSharedPtr&            	rBackBuffer,
				  Surface*                             pSurface) :
        maSprites(),
        maChangeRecords(),
        mrOutDev( rOutDev ),
        mpBackBuffer( rBackBuffer ), 
        maLastUpdate(),
        mbBackgroundDirty( true )
    {
	printf("redraw manager constructor\n");
        ENSURE_AND_THROW( mpBackBuffer.get(),
                          "RedrawManager::RedrawManager(): Invalid backbuffer" );
        maChangeRecords.reserve( CHANGE_RECORDS_INITIAL_SIZE );

	Size aSize = mpBackBuffer->getSize();
	mpSurface = cairo_surface_create_similar( mpBackBuffer->getSurface(), CAIRO_CONTENT_COLOR, aSize.Width(), aSize.Height() );
	mpCairo = cairo_create( mpSurface );
	mpWinSurface = pSurface;
	cairo_surface_reference( mpWinSurface );
    }

    RedrawManager::~RedrawManager()
    {
	printf("redraw manager destructor\n");
 	cairo_destroy( mpCairo );
 	cairo_surface_destroy( mpSurface );
 	cairo_surface_destroy( mpWinSurface );
    }

    void RedrawManager::updateScreen( bool bUpdateAll )
    {
        tools::OutDevStateKeeper aStateKeeper( mrOutDev );

        // TODO(F3): The SpriteCanvas currently doesn't act on window
        // resizes. Somehow, the VCL window must broadcast resizes
        // _very early_ (before the application repaints the content)
        // to the canvas, such that the backbuffer can be adapted.
//         const Size aOutDevSize( mpBackBuffer->getOutDev().GetOutputSizePixel() );
        const Point aEmptyPoint(0,0);

        // TODO(P1): Might be worthwile to track areas of background
        // changes, too.
        if( !bUpdateAll && !mbBackgroundDirty )
        {
            // background has not changed, so we're free to optimize
            // repaint to areas where a sprite has changed

            // TODO(F2): Unchanged sprites, which happen to be in the 
            // area-to-be-updated are currently not repainted.

            // calculate rectangular areas of mutually intersecting
            // sprite updates. We must handle them together, to
            // achieve proper compositing effects
            SpriteConnectedRanges aUpdateAreas;

#if defined(VERBOSE) && defined(DBG_UTIL)
            // also repaint background below frame counter (fake that
            // as a sprite vanishing in this area)
            aUpdateAreas.addRange( ::basegfx::B2DRectangle( 0.0, 0.0,
                                                            FPS_BOUNDS.Right(),
                                                            FPS_BOUNDS.Bottom() ),
                                   Sprite::ImplRef() );
#endif

            // extract all referenced sprites from the maChangeRecords
            // (copy sprites, make the list unique, regarding the
            // sprite pointer). This assumes that, until this scope
            // ends, nobody changes the maChangeRecords vector!
            typedef ::std::vector< Sprite::ImplRef > UpdateSprites;
            UpdateSprites aUpdateSprites;
            ::std::transform( maChangeRecords.begin(),
                              maChangeRecords.end(),
                              ::std::back_insert_iterator< UpdateSprites >(aUpdateSprites),
                              ::std::mem_fun_ref( &SpriteChangeRecord::getSprite ) );
            ::std::sort( aUpdateSprites.begin(),
                         aUpdateSprites.end() );

            UpdateSprites::iterator aBegin( aUpdateSprites.begin() );
            UpdateSprites::iterator aEnd  ( aUpdateSprites.end() );
            aEnd = ::std::unique( aBegin, aEnd );

            // for each unique sprite, check the change event vector,
            // calculate the update operation from that, and add the
            // result to the aUpdateArea.
            ::std::for_each( aBegin, aEnd,
                             ::canvas::internal::SpriteUpdater< Sprite::ImplRef, 
                             									ChangeRecords, 
                             									SpriteConnectedRanges >(aUpdateAreas,
                                                                                        maChangeRecords) );

            // prepare and flush all aggregated components to screen
            aUpdateAreas.forEachAggregate( ::boost::bind( &RedrawManager::updateArea,
                                                          this,
                                                          _1 ) );
        }
        else
        {
            // background has changed, so we currently have no choice
            // but repaint everything (or caller requested that)
            bUpdateAll = true;
        }

        // check again for fullscreen update, maybe update area
        // calculation above yielded decision to perform a fullscreen
        // update, anyway.
        if( bUpdateAll )
        {
	    printf ("redraw manager: update all\n");

	    Surface* pBackingStoreSurface;

	    if( maSprites.size() > 0 ) {
		cairo_save( mpCairo );
		cairo_set_operator( mpCairo, CAIRO_OPERATOR_SOURCE );
		cairo_set_source_surface( mpCairo, mpBackBuffer->getSurface(), 0, 0 );
		cairo_paint( mpCairo );
		cairo_restore( mpCairo );

		// repaint all active sprites
		::std::for_each( maSprites.begin(), 
				 maSprites.end(), 
				 SpritePainter( mpCairo ) );

		pBackingStoreSurface = mpSurface;
	    } else
		pBackingStoreSurface = mpBackBuffer->getSurface();

            // flush to screen
	    Cairo* pCairo = cairo_create( mpWinSurface );
	    cairo_set_operator( pCairo, CAIRO_OPERATOR_SOURCE );
	    cairo_set_source_surface( pCairo, pBackingStoreSurface, 0, 0 );
	    Size aSize = mpBackBuffer->getSize();
	    cairo_rectangle( pCairo, 0, 0, aSize.Width(), aSize.Height() );
	    cairo_clip( pCairo );
	    cairo_paint( pCairo );
	    cairo_destroy( pCairo );
        }

        // change record vector must be cleared, for the next turn of
        // rendering and sprite changing
        maChangeRecords.clear();

        mbBackgroundDirty = false;

#if defined(VERBOSE) && defined(DBG_UTIL)
        //renderFrameCounter();
        //renderSpriteCount();
        //renderMemUsage();
#endif
    }

    void RedrawManager::renderInfoText( const ::rtl::OUString& rStr,
                                        const Point&		   rPos )
    {
        Font aVCLFont;
        aVCLFont.SetHeight( 20 );
        aVCLFont.SetColor( Color( INFO_COLOR ) );

        mrOutDev.SetTextColor( Color( INFO_COLOR ) );
        mrOutDev.SetFont( aVCLFont );

        mrOutDev.DrawText( rPos, rStr );
    }

    void RedrawManager::renderFrameCounter()
    {
        const double denominator( maLastUpdate.getElapsedTime() );
        maLastUpdate.reset();
                        
        ::rtl::OUString text( ::rtl::math::doubleToUString( denominator == 0.0 ? 100.0 : 1.0/denominator,
                                                            rtl_math_StringFormat_F,
                                                            2,'.',NULL,' ') );

        // pad with leading space
        while( text.getLength() < 6 )
            text = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM (" ")) + text;

        text += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM (" fps"));

        renderInfoText( text, 
                        Point(0, 0) );
    }

    void RedrawManager::renderSpriteCount()
    {
        ::rtl::OUString text( 
            ::rtl::OUString::valueOf( 
                // disambiguate overload...
                static_cast<sal_Int64>(maSprites.size()) ) );

        // pad with leading space
        while( text.getLength() < 3 )
            text = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM (" ")) + text;

        text = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM ("Sprites: ")) + text;

        renderInfoText( text, 
                        Point(0, 30) );
    }

    void RedrawManager::renderMemUsage()
    {
	// rodo TODO
//         const double nPixel( ::std::accumulate( maSprites.begin(),
//                                                 maSprites.end(),
//                                                 0.0,
//                                                 ::std::ptr_fun( &accumulatePixel ) ) );

//         static const int NUM_VIRDEV(2);
//         static const int BYTES_PER_PIXEL(3);

//         const Size& rVDevSize( maVDev->GetOutputSizePixel() );
//         // const Size& rBackBufferSize( mpBackBuffer->getOutDev().GetOutputSizePixel() );

//         const double nMemUsage( nPixel * NUM_VIRDEV * BYTES_PER_PIXEL +
//                                 rVDevSize.Width()*rVDevSize.Height() * BYTES_PER_PIXEL +
//                                 rBackBufferSize.Width()*rBackBufferSize.Height() * BYTES_PER_PIXEL );

//         ::rtl::OUString text( ::rtl::math::doubleToUString( nMemUsage / 1048576.0,
//                                                             rtl_math_StringFormat_F,
//                                                             2,'.',NULL,' ') );

//         // pad with leading space
//         while( text.getLength() < 4 )
//             text = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM (" ")) + text;

//         text = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM ("Mem: ")) + 
//             text + 
//             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM ("MB"));

//         renderInfoText( text, 
//                         Point(0, 60) );
    }

    void RedrawManager::backgroundDirty()
    {
        mbBackgroundDirty = true;
    }

    void RedrawManager::showSprite( const Sprite::ImplRef& sprite )
    {
        maSprites.push_back( sprite );
    }

    void RedrawManager::hideSprite( const Sprite::ImplRef& sprite )
    {
        maSprites.remove( sprite );
    }

    void RedrawManager::moveSprite( const Sprite::ImplRef&		sprite,
                                    const Point& 			rOldPos,
                                    const Point& 			rNewPos,
                                    const Size&			 	rSpriteSize )
    {
        // TODO(Q1): Use AW's converters once resynced
        maChangeRecords.push_back( SpriteChangeRecord( sprite,
                                                       ::basegfx::B2DPoint( rOldPos.X(),
                                                                            rOldPos.Y() ),
                                                       ::basegfx::B2DPoint( rNewPos.X(),
                                                                            rNewPos.Y() ),
                                                       ::basegfx::B2DSize( rSpriteSize.Width(),
                                                                           rSpriteSize.Height() ) ) );
    }

    void RedrawManager::updateSprite( const Sprite::ImplRef& 	sprite, 
                                      const Point& 				rPos,
                                      const Rectangle& 			rUpdateArea )
    {
        // TODO(Q1): Use AW's converters once resynced
        maChangeRecords.push_back( SpriteChangeRecord( sprite,
                                                       ::basegfx::B2DPoint( rPos.X(),
                                                                            rPos.Y() ),
                                                       ::basegfx::B2DRectangle( rUpdateArea.Left(),
                                                                                rUpdateArea.Top(),
                                                                                rUpdateArea.Right(),
                                                                                rUpdateArea.Bottom() ) ) );
    }

    namespace
    {
        bool isAreaUpdateNotOpaque( const Rectangle& 											rUpdateArea, 
                                    const RedrawManager::SpriteConnectedRanges::ComponentType&	rComponent )
        {
            const Sprite::ImplRef& pAffectedSprite( rComponent.second );

            if( !pAffectedSprite.is() )
                return true; // no sprite, no opaque update!

            return !pAffectedSprite->isAreaUpdateOpaque( rUpdateArea );
        }

        void spriteRedrawStub( const Point& 												rOffset, 
                               Cairo* pCairo,
                               const RedrawManager::SpriteConnectedRanges::ComponentType&	rComponent )
        {
            const Sprite::ImplRef& pAffectedSprite( rComponent.second );

	    printf ("spriteRedrawStub\n");

            if( !pAffectedSprite.is() )
                return; // no sprite, no update!

            // calc relative sprite position in rUpdateArea (which need
            // not be the whole screen!)
            const ::basegfx::B2DPoint	aSpriteScreenPos( pAffectedSprite->getSpritePos() );

            // paint affected sprite
            pAffectedSprite->redraw( pCairo, aSpriteScreenPos );
        }
    }


    void RedrawManager::updateArea( const SpriteConnectedRanges::ConnectedComponents& rComponents )
    {
        const ::basegfx::B2DRectangle& rRequestedArea( rComponents.maTotalBounds );

        // limit size of update VDev to target outdev's size
        const Size& rTargetSizePixel( mrOutDev.GetOutputSizePixel() );

        // round output position towards zero. Don't want to truncate
        // a fraction of a sprite pixel...  Clip position at origin,
        // otherwise, truncation of size below might leave visible
        // areas uncovered by VDev.
        const Point aOutputPosition( 
            ::std::max( 0L,
                        static_cast< long >(rRequestedArea.getMinX()) ),
            ::std::max( 0L,
                        static_cast< long >(rRequestedArea.getMinY()) ) );
        // round output size towards +infty. Don't want to truncate a
        // fraction of a sprite pixel... Limit size of VDev to output
        // device's area.
        const Size  aOutputSize( 
            ::std::min( rTargetSizePixel.Width(),
                        static_cast< long >(
                            ceil( rRequestedArea.getMaxX() - aOutputPosition.X()) ) ),
            ::std::min( rTargetSizePixel.Height(),
                        static_cast< long >(
                            ceil( rRequestedArea.getMaxY() - aOutputPosition.Y()) ) ) );

        const Rectangle aUpdateArea( aOutputPosition.X(),
                                     aOutputPosition.Y(),
                                     aOutputPosition.X() + aOutputSize.Width(),
                                     aOutputPosition.Y() + aOutputSize.Height() );
        
	printf("update area size: %d x %d\n", aOutputSize.Width(), aOutputSize.Height() );

	if( aOutputSize.Width() > 0 && aOutputSize.Height() > 0 ) {


	    cairo_save( mpCairo );

	    cairo_rectangle( mpCairo, aOutputPosition.X(), aOutputPosition.Y(), aOutputSize.Width(), aOutputSize.Height() );
	    cairo_clip( mpCairo );

	    cairo_save( mpCairo );
	    cairo_set_operator( mpCairo, CAIRO_OPERATOR_SOURCE );
	    cairo_set_source_surface( mpCairo, mpBackBuffer->getSurface(), 0, 0 );
	    cairo_paint( mpCairo );
	    cairo_restore( mpCairo );
        
	    // paint all affected sprites to update area
	    ::std::for_each( rComponents.maComponentList.begin(),
			     rComponents.maComponentList.end(),
			     ::boost::bind( &spriteRedrawStub,
					    ::boost::cref(aUpdateArea.TopLeft()),
					    mpCairo,
					    _1 ) );

            // flush to screen
	    Cairo* pCairo = cairo_create( mpWinSurface );

	    cairo_save( mpCairo );
	    cairo_rectangle( pCairo, aOutputPosition.X(), aOutputPosition.Y(), aOutputSize.Width(), aOutputSize.Height() );
	    cairo_clip( pCairo );
	    cairo_set_operator( pCairo, CAIRO_OPERATOR_SOURCE );
	    cairo_set_source_surface( pCairo, mpSurface, 0, 0 );
	    cairo_paint( pCairo );
	    cairo_restore( mpCairo );

	    cairo_restore( mpCairo );

	    cairo_destroy( pCairo );
	}
    }
}
