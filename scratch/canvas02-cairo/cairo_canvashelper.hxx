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

#ifndef _CAIROCANVAS_CANVASHELPER_HXX_
#define _CAIROCANVAS_CANVASHELPER_HXX_

#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XIntegerBitmap.hpp>

#include <basegfx/vector/b2isize.hxx>
#include <basegfx/vector/b2dsize.hxx>

#include <boost/utility.hpp>

#include "cairo_cairo.hxx"

namespace basegfx {
    class B2DPolyPolygon;
}

namespace cairocanvas
{
    class SpriteCanvas;

	enum Operation {
	    Stroke,
	    Fill,
	    Clip
	};

    /** Helper class for basic canvas functionality. Also offers
        optional backbuffer painting, when providing it with a second
        HDC to render into.
     */
    class CanvasHelper : private ::boost::noncopyable
    {
    public:
        CanvasHelper();

        /// Release all references
        void disposing();

		/** Initialize canvas helper

        	This method late-initializes the canvas helper, providing
        	it with the necessary device and size. Note that the
        	CanvasHelper does <em>not</em> take ownership of the
        	passed rDevice reference, nor does it perform any
        	reference counting. Thus, to prevent the reference counted
        	SpriteCanvas object from deletion, the user of this class
        	is responsible for holding ref-counted references itself!

            @param rSizePixel
            Size of the output surface in pixel.

        	@param rDevice
            Reference device this canvas is associated with

         */
        void init( const ::basegfx::B2ISize& rSizePixel,
                   SpriteCanvas&             rDevice,
		   ::cairo::Cairo*           pCairo,
                   bool                      bHasAlpha );


        // CanvasHelper functionality
        // ==========================

        // XCanvas (only providing, not implementing the
        // interface. Also note subtle method parameter differences)
        void drawPoint( const ::com::sun::star::rendering::XCanvas* 	pCanvas, 
                        const ::com::sun::star::geometry::RealPoint2D& 	aPoint, 
                        const ::com::sun::star::rendering::ViewState& 	viewState, 
                        const ::com::sun::star::rendering::RenderState& renderState );
        void drawLine( const ::com::sun::star::rendering::XCanvas* 		pCanvas, 
                       const ::com::sun::star::geometry::RealPoint2D& 	aStartPoint, 
                       const ::com::sun::star::geometry::RealPoint2D& 	aEndPoint, 
                       const ::com::sun::star::rendering::ViewState& 	viewState, 
                       const ::com::sun::star::rendering::RenderState& 	renderState	);
        void drawBezier( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                         const ::com::sun::star::geometry::RealBezierSegment2D&	aBezierSegment, 
                         const ::com::sun::star::geometry::RealPoint2D& 		aEndPoint,
                         const ::com::sun::star::rendering::ViewState& 			viewState, 
                         const ::com::sun::star::rendering::RenderState& 		renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	drawPolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                             const ::com::sun::star::uno::Reference< 
                             		::com::sun::star::rendering::XPolyPolygon2D >&	xPolyPolygon, 
                             const ::com::sun::star::rendering::ViewState& 			viewState, 
                             const ::com::sun::star::rendering::RenderState& 		renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	strokePolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                               const ::com::sun::star::uno::Reference< 
                               		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                               const ::com::sun::star::rendering::ViewState& 		viewState, 
                               const ::com::sun::star::rendering::RenderState& 		renderState, 
                               const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	strokeTexturedPolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                                       const ::com::sun::star::uno::Reference< 
                                       		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                                       const ::com::sun::star::rendering::ViewState& 		viewState, 
                                       const ::com::sun::star::rendering::RenderState& 		renderState, 
                                       const ::com::sun::star::uno::Sequence< 
                                       		::com::sun::star::rendering::Texture >& 		textures, 
                                       const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	strokeTextureMappedPolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                                            const ::com::sun::star::uno::Reference< 
                                            		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                                            const ::com::sun::star::rendering::ViewState& 			viewState, 
                                            const ::com::sun::star::rendering::RenderState& 		renderState, 
                                            const ::com::sun::star::uno::Sequence< 
                                            		::com::sun::star::rendering::Texture >& 		textures, 
                                            const ::com::sun::star::uno::Reference< 
                                            		::com::sun::star::geometry::XMapping2D >& 		xMapping, 
                                            const ::com::sun::star::rendering::StrokeAttributes& 	strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D >   	
        	queryStrokeShapes( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                               const ::com::sun::star::uno::Reference< 
                               		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                               const ::com::sun::star::rendering::ViewState& 		viewState, 
                               const ::com::sun::star::rendering::RenderState& 		renderState, 
                               const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	fillPolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                             const ::com::sun::star::uno::Reference< 
                             		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                             const ::com::sun::star::rendering::ViewState& 			viewState, 
                             const ::com::sun::star::rendering::RenderState& 		renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	fillTexturedPolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                                     const ::com::sun::star::uno::Reference< 
                                     		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                                     const ::com::sun::star::rendering::ViewState& 			viewState, 
                                     const ::com::sun::star::rendering::RenderState& 		renderState, 
                                     const ::com::sun::star::uno::Sequence< 
                                     		::com::sun::star::rendering::Texture >& 		textures );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	fillTextureMappedPolyPolygon( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                                          const ::com::sun::star::uno::Reference< 
                                          		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                                          const ::com::sun::star::rendering::ViewState& 		viewState, 
                                          const ::com::sun::star::rendering::RenderState& 		renderState, 
                                          const ::com::sun::star::uno::Sequence< 
                                          		::com::sun::star::rendering::Texture >& 		textures, 
                                          const ::com::sun::star::uno::Reference< 
                                          		::com::sun::star::geometry::XMapping2D >& 		xMapping );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCanvasFont > SAL_CALL
        	createFont( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                        const ::com::sun::star::rendering::FontRequest& 		fontRequest, 
                        const ::com::sun::star::uno::Sequence< 
                        		::com::sun::star::beans::PropertyValue >& 		extraFontProperties, 
                        const ::com::sun::star::geometry::Matrix2D& 			fontMatrix );

        ::com::sun::star::uno::Sequence< ::com::sun::star::rendering::FontInfo > 
        	queryAvailableFonts( const ::com::sun::star::rendering::XCanvas* 		pCanvas, 
                                 const ::com::sun::star::rendering::FontInfo& 		aFilter, 
                                 const ::com::sun::star::uno::Sequence< 
                                 		::com::sun::star::beans::PropertyValue >& 	aFontProperties );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 
        	drawText( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                      const ::com::sun::star::rendering::StringContext& 	text, 
                      const ::com::sun::star::uno::Reference< 
                      		::com::sun::star::rendering::XCanvasFont >& 	xFont, 
                      const ::com::sun::star::rendering::ViewState& 		viewState, 
                      const ::com::sun::star::rendering::RenderState& 		renderState, 
                      sal_Int8 												textDirection );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 
        	drawTextLayout( const ::com::sun::star::rendering::XCanvas* 	pCanvas, 
                            const ::com::sun::star::uno::Reference< 
                            	::com::sun::star::rendering::XTextLayout >& layoutetText, 
                            const ::com::sun::star::rendering::ViewState& 	viewState, 
                            const ::com::sun::star::rendering::RenderState& renderState );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	drawBitmap( const ::com::sun::star::rendering::XCanvas* 	pCanvas, 
                        const ::com::sun::star::uno::Reference< 
                        		::com::sun::star::rendering::XBitmap >& xBitmap, 
                        const ::com::sun::star::rendering::ViewState& 	viewState, 
                        const ::com::sun::star::rendering::RenderState& renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	drawBitmapModulated( const ::com::sun::star::rendering::XCanvas* 		pCanvas, 
                                 const ::com::sun::star::uno::Reference< 
                                 		::com::sun::star::rendering::XBitmap >&		xBitmap, 
                                 const ::com::sun::star::rendering::ViewState& 		viewState, 
                                 const ::com::sun::star::rendering::RenderState& 	renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XGraphicDevice > 
        	getDevice();

        // BitmapCanvasHelper functionality
        // ================================

        void copyRect( const ::com::sun::star::rendering::XCanvas* 			pCanvas, 
                       const ::com::sun::star::uno::Reference< 
                       		::com::sun::star::rendering::XBitmapCanvas >& 	sourceCanvas, 
                       const ::com::sun::star::geometry::RealRectangle2D& 	sourceRect, 
                       const ::com::sun::star::rendering::ViewState& 		sourceViewState, 
                       const ::com::sun::star::rendering::RenderState& 		sourceRenderState, 
                       const ::com::sun::star::geometry::RealRectangle2D& 	destRect, 
                       const ::com::sun::star::rendering::ViewState& 		destViewState, 
                       const ::com::sun::star::rendering::RenderState& 		destRenderState );
            
        ::com::sun::star::geometry::IntegerSize2D getSize();

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmap > 
        	getScaledBitmap( const ::com::sun::star::geometry::RealSize2D& 	newSize, 
                             sal_Bool 										beFast );

        ::com::sun::star::uno::Sequence< sal_Int8 > 
	        getData( ::com::sun::star::rendering::IntegerBitmapLayout& 		bitmapLayout,
                     const ::com::sun::star::geometry::IntegerRectangle2D&  rect );

        void setData( const ::com::sun::star::uno::Sequence< sal_Int8 >&         data, 
                      const ::com::sun::star::rendering::IntegerBitmapLayout&    bitmapLayout, 
                      const ::com::sun::star::geometry::IntegerRectangle2D&      rect );

        void setPixel( const ::com::sun::star::uno::Sequence< sal_Int8 >&        color, 
                       const ::com::sun::star::rendering::IntegerBitmapLayout&   bitmapLayout, 
                       const ::com::sun::star::geometry::IntegerPoint2D&         pos );

        ::com::sun::star::uno::Sequence< sal_Int8 > 
	        getPixel( ::com::sun::star::rendering::IntegerBitmapLayout& bitmapLayout, 
                      const ::com::sun::star::geometry::IntegerPoint2D& pos );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmapPalette > getPalette();

        ::com::sun::star::rendering::IntegerBitmapLayout getMemoryLayout();
        
        // Flush drawing queue to screen
        void					flush() const;

        /** Called from XCanvas base classes, to notify that content
            is _about_ to change
        */
        void modifying() {}

        bool hasAlpha() const;

        enum ColorType
        {
            LINE_COLOR, FILL_COLOR, TEXT_COLOR, IGNORE_COLOR
        };
        
	void drawPolyPolygonPath( const ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D >& xPolyPolygon,
				  Operation aOperation,
				  const ::com::sun::star::uno::Sequence< ::com::sun::star::rendering::Texture >* pTextures=NULL,
				  ::cairo::Cairo* pCairo=NULL ) const;

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
	implDrawBitmapSurface( ::cairo::Surface* pSurface, 
			       const ::com::sun::star::rendering::ViewState& 	viewState, 
			       const ::com::sun::star::rendering::RenderState& renderState,
			       bool bModulateColors,
			       bool bHasAlpha );

    protected:
        /** Phyical output device

            Deliberately not a refcounted reference, because of
            potential circular references for spritecanvas.
         */
        SpriteCanvas*                   mpDevice;

    private:
	void useStates( const ::com::sun::star::rendering::ViewState& viewState,
			const ::com::sun::star::rendering::RenderState& renderState,
			bool setColor );

        /// When true, content is able to represent alpha
        bool                            mbHaveAlpha;

	::cairo::Cairo* mpCairo;
    };

    void drawPolyPolygonImplementation( ::basegfx::B2DPolyPolygon aPolyPolygon,
					Operation aOperation,
					::cairo::Cairo* pCairo,
					const ::com::sun::star::uno::Sequence< ::com::sun::star::rendering::Texture >* pTextures=NULL,
					SpriteCanvas* pDevice=NULL );
    void doOperation( Operation aOperation,
		      ::cairo::Cairo* pCairo,
		      sal_uInt32 nPolygonIndex=0,
		      const ::com::sun::star::uno::Sequence< ::com::sun::star::rendering::Texture >* pTextures=NULL,
		      SpriteCanvas* pDevice=NULL);
}

#endif /* _CAIROCANVAS_CANVASHELPER_HXX_ */
