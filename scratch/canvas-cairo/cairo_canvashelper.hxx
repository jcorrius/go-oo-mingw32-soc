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

#ifndef _VCLCANVAS_CANVASHELPER_HXX_
#define _VCLCANVAS_CANVASHELPER_HXX_

#ifndef _COM_SUN_STAR_RENDERING_XCANVAS_HPP_
#include <com/sun/star/rendering/XCanvas.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XINTEGERBITMAP_HPP_
#include <com/sun/star/rendering/XIntegerBitmap.hpp>
#endif

#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif

#include <canvas/vclwrapper.hxx>

#include "cairo_cairo.hxx"
#include "cairo_outdevprovider.hxx"
#include "cairo_cachedbitmap.hxx"
#include "cairo_windowgraphicdevice.hxx"


namespace vclcanvas
{
    /** Helper class for basic canvas functionality. Also offers
        optional backbuffer painting, when providing it with a second
        OutputDevice to render into.
     */
    class CanvasHelper
    {
    public:
        CanvasHelper();

        /// Release all references
        void disposing();

        void setGraphicDevice( const WindowGraphicDevice::ImplRef& rDevice );

        /** Set primary output device

        	@param bProtect
            When true, all output operations preserve outdev
            state. When false, outdev state might change at any time.
         */
        void setCairo( ::cairo::Cairo* pCairo );

        /** Set secondary output device

        	Used for sprites, to generate mask bitmap.
         */
        void setBackgroundOutDev( const OutDevProviderSharedPtr& rOutDev );

        // CanvasHelper functionality
        // ==========================

        // XCanvas (only providing, not implementing the
        // interface. Also note subtle method parameter differences)
        void drawPoint( const ::com::sun::star::rendering::XCanvas& 		rCanvas, 
                        const ::com::sun::star::geometry::RealPoint2D& 		aPoint, 
                        const ::com::sun::star::rendering::ViewState& 		viewState, 
                        const ::com::sun::star::rendering::RenderState& 	renderState );
        void drawLine( const ::com::sun::star::rendering::XCanvas& 		rCanvas, 
                       const ::com::sun::star::geometry::RealPoint2D& 	aStartPoint, 
                       const ::com::sun::star::geometry::RealPoint2D& 	aEndPoint, 
                       const ::com::sun::star::rendering::ViewState& 	viewState, 
                       const ::com::sun::star::rendering::RenderState& 	renderState	);
        void drawBezier( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                         const ::com::sun::star::geometry::RealBezierSegment2D&	aBezierSegment, 
                         const ::com::sun::star::geometry::RealPoint2D& 		aEndPoint,
                         const ::com::sun::star::rendering::ViewState& 			viewState, 
                         const ::com::sun::star::rendering::RenderState& 		renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	drawPolyPolygon( const ::com::sun::star::rendering::XCanvas& 		rCanvas, 
                             const ::com::sun::star::uno::Reference< 
                             	::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                             const ::com::sun::star::rendering::ViewState& 		viewState, 
                             const ::com::sun::star::rendering::RenderState& 	renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	strokePolyPolygon( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                               const ::com::sun::star::uno::Reference< 
                               		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                               const ::com::sun::star::rendering::ViewState& 		viewState, 
                               const ::com::sun::star::rendering::RenderState& 		renderState, 
                               const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	strokeTexturedPolyPolygon( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                                       const ::com::sun::star::uno::Reference< 
                                       		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                                       const ::com::sun::star::rendering::ViewState& 		viewState, 
                                       const ::com::sun::star::rendering::RenderState& 		renderState, 
                                       const ::com::sun::star::uno::Sequence< 
                                       		::com::sun::star::rendering::Texture >& 		textures, 
                                       const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	strokeTextureMappedPolyPolygon( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
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
        	queryStrokeShapes( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                               const ::com::sun::star::uno::Reference< 
                               		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                               const ::com::sun::star::rendering::ViewState& 		viewState, 
                               const ::com::sun::star::rendering::RenderState& 		renderState, 
                               const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	fillPolyPolygon( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                             const ::com::sun::star::uno::Reference< 
                             		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                             const ::com::sun::star::rendering::ViewState& 			viewState, 
                             const ::com::sun::star::rendering::RenderState& 		renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	fillTexturedPolyPolygon( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                                     const ::com::sun::star::uno::Reference< 
                                     		::com::sun::star::rendering::XPolyPolygon2D >&	xPolyPolygon, 
                                     const ::com::sun::star::rendering::ViewState& 			viewState, 
                                     const ::com::sun::star::rendering::RenderState& 		renderState, 
                                     const ::com::sun::star::uno::Sequence< 
                                     		::com::sun::star::rendering::Texture >& 		textures );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	fillTextureMappedPolyPolygon( const ::com::sun::star::rendering::XCanvas& 			rCanvas, 
                                          const ::com::sun::star::uno::Reference< 
                                          		::com::sun::star::rendering::XPolyPolygon2D >& 	xPolyPolygon, 
                                          const ::com::sun::star::rendering::ViewState& 		viewState, 
                                          const ::com::sun::star::rendering::RenderState& 		renderState, 
                                          const ::com::sun::star::uno::Sequence< 
                                          		::com::sun::star::rendering::Texture >& 		textures, 
                                          const ::com::sun::star::uno::Reference< 
                                          		::com::sun::star::geometry::XMapping2D >& 		xMapping );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCanvasFont > 
	        createFont( const ::com::sun::star::rendering::XCanvas& 		rCanvas,
                        const ::com::sun::star::rendering::FontRequest& 	fontRequest, 
                        const ::com::sun::star::uno::Sequence< 
                        	::com::sun::star::beans::PropertyValue >& 		extraFontProperties, 
                        const ::com::sun::star::geometry::Matrix2D& 		fontMatrix );

        ::com::sun::star::uno::Sequence< ::com::sun::star::rendering::FontInfo > 
        	queryAvailableFonts( const ::com::sun::star::rendering::XCanvas& 		rCanvas,
                                 const ::com::sun::star::rendering::FontInfo& 		aFilter, 
                                 const ::com::sun::star::uno::Sequence< 
                                 		::com::sun::star::beans::PropertyValue >& 	aFontProperties );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 
        	drawText( const ::com::sun::star::rendering::XCanvas& 		rCanvas,
                      const ::com::sun::star::rendering::StringContext& text, 
                      const ::com::sun::star::uno::Reference< 
                      		::com::sun::star::rendering::XCanvasFont >& xFont, 
                      const ::com::sun::star::rendering::ViewState& 	viewState, 
                      const ::com::sun::star::rendering::RenderState& 	renderState, 
                      sal_Int8 											textDirection );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 
        	drawTextLayout( const ::com::sun::star::rendering::XCanvas& 		rCanvas,
                            const ::com::sun::star::uno::Reference< 
                            		::com::sun::star::rendering::XTextLayout >& layoutetText, 
                            const ::com::sun::star::rendering::ViewState& 		viewState, 
                            const ::com::sun::star::rendering::RenderState& 	renderState );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	drawBitmap( const ::com::sun::star::rendering::XCanvas& 	rCanvas,
                        const ::com::sun::star::uno::Reference< 
                        		::com::sun::star::rendering::XBitmap >& xBitmap, 
                        const ::com::sun::star::rendering::ViewState& 	viewState, 
                        const ::com::sun::star::rendering::RenderState& renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	drawBitmapModulated( const ::com::sun::star::rendering::XCanvas& 		rCanvas,
                                 const ::com::sun::star::uno::Reference< 
                                 		::com::sun::star::rendering::XBitmap >&		xBitmap, 
                                 const ::com::sun::star::rendering::ViewState& 		viewState, 
                                 const ::com::sun::star::rendering::RenderState&	renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XGraphicDevice > 
        	getDevice();

        // BitmapCanvasHelper functionality
        // ================================

        void copyRect( const ::com::sun::star::rendering::XCanvas& 			rCanvas,
                       const ::com::sun::star::uno::Reference< 
                       		::com::sun::star::rendering::XBitmapCanvas >& 	sourceCanvas, 
                       const ::com::sun::star::geometry::RealRectangle2D& 	sourceRect, 
                       const ::com::sun::star::rendering::ViewState& 		sourceViewState, 
                       const ::com::sun::star::rendering::RenderState& 		sourceRenderState, 
                       const ::com::sun::star::geometry::RealRectangle2D& 	destRect, 
                       const ::com::sun::star::rendering::ViewState& 		destViewState, 
                       const ::com::sun::star::rendering::RenderState& 		destRenderState );
            
        ::com::sun::star::geometry::IntegerSize2D getSize();

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmapCanvas > queryBitmapCanvas();

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmap > 
        	getScaledBitmap( const ::com::sun::star::geometry::RealSize2D& 	newSize, 
                             sal_Bool 												beFast );

        ::com::sun::star::uno::Sequence< sal_Int8 > 
	        getData( const ::com::sun::star::geometry::IntegerRectangle2D& rect );

        void setData( const ::com::sun::star::uno::Sequence< sal_Int8 >& 				data, 
                               const ::com::sun::star::geometry::IntegerRectangle2D& 	rect );

        void setPixel( const ::com::sun::star::uno::Sequence< sal_Int8 >& 			color, 
                                const ::com::sun::star::geometry::IntegerPoint2D& 	pos );

        ::com::sun::star::uno::Sequence< sal_Int8 > 
	        getPixel( const ::com::sun::star::geometry::IntegerPoint2D& pos );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmapPalette > getPalette();

        ::com::sun::star::rendering::IntegerBitmapLayout getMemoryLayout();

        /// Repaint a cached bitmap
        bool repaint( const GraphicObjectSharedPtr&	rGrf,
                      const ::Point& 				rPt, 
                      const ::Size& 				rSz,
                      const GraphicAttr&			rAttr ) const;

        enum ColorType
        {
            LINE_COLOR, FILL_COLOR, TEXT_COLOR, IGNORE_COLOR
        };
        
    protected:
        /// Phyical output device
        WindowGraphicDevice::ImplRef	mxDevice;

        OutDevProviderSharedPtr			mp2ndOutDev;

	::cairo::Cairo* mpCairo;
        
    private:
        // default: disabled copy/assignment
        CanvasHelper(const CanvasHelper&);
        CanvasHelper& operator=( const CanvasHelper& );
        
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive > 	
        	implDrawBitmap( const ::com::sun::star::rendering::XCanvas& 	rCanvas,
                            const ::com::sun::star::uno::Reference< 
                  		          ::com::sun::star::rendering::XBitmap >& 	xBitmap, 
                            const ::com::sun::star::rendering::ViewState& 	viewState, 
                            const ::com::sun::star::rendering::RenderState& renderState,
                            bool 											bModulateColors );

        bool setupTextOutput( ::Point&																						o_rOutPos,
                              const ::com::sun::star::rendering::ViewState& 										viewState, 
                              const ::com::sun::star::rendering::RenderState& 										renderState, 
                              const ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCanvasFont >&	xFont );

	void useStates( const ::com::sun::star::rendering::ViewState& viewState,
			const ::com::sun::star::rendering::RenderState& renderState,
			bool setColor );

	enum Operation {
	    Stroke,
	    Fill,
	    Clip
	};

	void doOperation( Operation aOperation );

	void drawPolyPolygonPath( const ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D >& xPolyPolygon,
				  Operation aOperation );
    };
}

#endif /* _VCLCANVAS_CANVASHELPER_HXX_ */
