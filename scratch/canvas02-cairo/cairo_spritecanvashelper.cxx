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

#include <comphelper/scopeguard.hxx> 

#include <basegfx/range/b2drectangle.hxx>
#include <basegfx/tools/canvastools.hxx>

#include <boost/cast.hpp>

#include "cairo_spritecanvashelper.hxx"
#include "cairo_canvascustomsprite.hxx"

using namespace ::cairo;
using namespace ::com::sun::star;

namespace cairocanvas
{
    namespace
    {
        /** Sprite redraw at original position

            Used to repaint the whole canvas (background and all
            sprites)
         */
        void spriteRedraw( Cairo* pCairo,
                           const ::canvas::Sprite::Reference& rSprite )
        {            
            // downcast to derived vclcanvas::Sprite interface, which
            // provides the actual redraw methods.
            ::boost::polymorphic_downcast< Sprite* >(rSprite.get())->redraw( pCairo, true);
        }
    }

    SpriteCanvasHelper::SpriteCanvasHelper() :
        mpRedrawManager( NULL )
    {
    }

    void SpriteCanvasHelper::init( ::canvas::SpriteRedrawManager& rManager,
                                   SpriteCanvas&                  rDevice,
				   Cairo*                         pCairo,
                                   const ::basegfx::B2ISize&      rSize,
                                   bool                           bHasAlpha )
    {
        mpRedrawManager = &rManager;

        CanvasHelper::init( rSize, rDevice, pCairo, bHasAlpha );
    }
    
    void SpriteCanvasHelper::disposing()
    {
        mpRedrawManager = NULL;

        // forward to base
        CanvasHelper::disposing();        
    }

    uno::Reference< rendering::XAnimatedSprite > SpriteCanvasHelper::createSpriteFromAnimation( 
        const uno::Reference< rendering::XAnimation >& animation )
    {
        return uno::Reference< rendering::XAnimatedSprite >();
    }

    uno::Reference< rendering::XAnimatedSprite > SpriteCanvasHelper::createSpriteFromBitmaps( 
        const uno::Sequence< uno::Reference< rendering::XBitmap > >& animationBitmaps, 
        sal_Int8                                                     interpolationMode )
    {
        return uno::Reference< rendering::XAnimatedSprite >();
    }

    uno::Reference< rendering::XCustomSprite > SpriteCanvasHelper::createCustomSprite( const geometry::RealSize2D& spriteSize )
    {
        if( !mpRedrawManager )
            return uno::Reference< rendering::XCustomSprite >(); // we're disposed

        return uno::Reference< rendering::XCustomSprite >( 
            new CanvasCustomSprite( spriteSize, 
                                    mpDevice ) );
    }

    uno::Reference< rendering::XSprite > SpriteCanvasHelper::createClonedSprite( const uno::Reference< rendering::XSprite >& original )
    {
        return uno::Reference< rendering::XSprite >();
    }

    sal_Bool SpriteCanvasHelper::updateScreen( const ::basegfx::B2IRange& rCurrArea,
                                               sal_Bool                   bUpdateAll,
                                               bool&                      io_bSurfaceDirty )
    {
        if( !mpRedrawManager || 
            !mpDevice ||
            !mpDevice->getWindowSurface() ||
            !mpDevice->getBufferSurface() )
        {
            return sal_False; // disposed, or otherwise dysfunctional
        }

	Cairo* pBufferCairo = cairo_create( mpDevice->getBufferSurface() );
	Cairo* pWindowCairo = cairo_create( mpDevice->getWindowSurface() );

	geometry::IntegerSize2D aSize = mpDevice->getSize();

        // TODO(P1): Might be worthwile to track areas of background
        // changes, too.
        if( !bUpdateAll && !io_bSurfaceDirty )
        {
            // background has not changed, so we're free to optimize
            // repaint to areas where a sprite has changed

            // process each independent area of overlapping sprites
            // separately.
            mpRedrawManager->forEachSpriteArea( *this );
        }
        else
        {
            // background has changed, so we currently have no choice
            // but repaint everything (or caller requested that)

	    cairo_rectangle( pBufferCairo, 0, 0, aSize.Width, aSize.Height );
	    cairo_clip( pBufferCairo );
	    cairo_set_source_surface( pBufferCairo, mpDevice->getBackgroundSurface(), 0, 0 );
	    cairo_set_operator( pBufferCairo, CAIRO_OPERATOR_SOURCE );
	    cairo_paint( pBufferCairo );

            // repaint all active sprites on top of background into
            // VDev.
            mpRedrawManager->forEachSprite( 
                ::boost::bind(
                    &spriteRedraw,
                    pBufferCairo,
                    _1 ) );

            // flush to screen
	    cairo_rectangle( pWindowCairo, 0, 0, aSize.Width, aSize.Height );
	    cairo_clip( pWindowCairo );
	    cairo_set_source_surface( pWindowCairo, mpDevice->getBufferSurface(), 0, 0 );
	    cairo_set_operator( pWindowCairo, CAIRO_OPERATOR_SOURCE );
	    cairo_paint( pWindowCairo );
// 	    cairo_rectangle( pWindowCairo, 0, 0, aSize.Width, aSize.Height );
// 	    cairo_set_source_rgb( pWindowCairo, 0, 0, 1 );
// 	    cairo_stroke( pWindowCairo );
        }

	cairo_destroy( pBufferCairo );
	cairo_destroy( pWindowCairo );

        // change record vector must be cleared, for the next turn of
        // rendering and sprite changing
        mpRedrawManager->clearChangeRecords();

        io_bSurfaceDirty = false;

        // commit to screen
        mpDevice->flush();

        return sal_True;
    }

    void SpriteCanvasHelper::backgroundPaint( const ::basegfx::B2DRange& rUpdateRect )
    {
	if( mpDevice ) {
	    Cairo* pBufferCairo = cairo_create( mpDevice->getBufferSurface() );

	    cairo_rectangle( pBufferCairo, rUpdateRect.getMinX(), rUpdateRect.getMinY(), rUpdateRect.getWidth(), rUpdateRect.getHeight() );
	    cairo_clip( pBufferCairo );
	    cairo_set_source_surface( pBufferCairo, mpDevice->getBackgroundSurface(), 0, 0 );
	    cairo_set_operator( pBufferCairo, CAIRO_OPERATOR_SOURCE );
	    cairo_paint( pBufferCairo );
	}
    }

    void SpriteCanvasHelper::scrollUpdate( const ::basegfx::B2DRange& 						rMoveStart, 
                                           const ::basegfx::B2DRange& 						rMoveEnd,
                                           const ::canvas::SpriteRedrawManager::UpdateArea& rUpdateArea )
    {
        // TODO
    }
    
    void SpriteCanvasHelper::opaqueUpdate( const ::basegfx::B2DRange&                          rTotalArea,
                                           const ::std::vector< ::canvas::Sprite::Reference >& rSortedUpdateSprites )
    {
        // TODO
    }
    
    void SpriteCanvasHelper::genericUpdate( const ::basegfx::B2DRange&                          rRequestedArea,
                                            const ::std::vector< ::canvas::Sprite::Reference >& rSortedUpdateSprites )
    {
        // TODO
    }
}
