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
#include "cairo_canvasbitmap.hxx"

#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif
#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::cairo;

namespace vclcanvas
{
    CanvasBitmap::CanvasBitmap( const ::Size& 						rSize,
                                bool								bAlphaBitmap,
                                const WindowGraphicDevice::ImplRef&	rDevice )
    { 
	WindowGraphicDevice::ImplRef xDevice = rDevice;

	mpCairo = cairo_create( xDevice->getSimilarSurface( rSize, bAlphaBitmap ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24 ) );
	maCanvasHelper.setCairo( mpCairo );
	maCanvasHelper.setDevice( rDevice );
    }

    CanvasBitmap::CanvasBitmap( const geometry::RealSize2D& rSize, Surface* pSrcSurface,
                                const WindowGraphicDevice::ImplRef& rDevice )
    {
	WindowGraphicDevice::ImplRef xDevice = rDevice;

	mpCairo = cairo_create( xDevice->getSimilarSurface( ::vcl::unotools::sizeFromRealSize2D(rSize), CAIRO_FORMAT_RGB24 ) );
	maCanvasHelper.setCairo( mpCairo );
	maCanvasHelper.setDevice( rDevice );

	geometry::RealSize2D aSrcSize = xDevice->getSize();

	cairo_save( mpCairo );
	cairo_set_source_surface( mpCairo, pSrcSurface, 0, 0 );
	cairo__scale( mpCairo, rSize.Width/aSrcSize.Width, rSize.Height/aSrcSize.Height );
	cairo_set_operator( mpCairo, CAIRO_OPERATOR_SOURCE );
	cairo_paint( mpCairo );
	cairo_restore( mpCairo );
    }

    CanvasBitmap::~CanvasBitmap()
    {
	Surface *pSurface = cairo_get_target( mpCairo );

	cairo_destroy( mpCairo );
	cairo_surface_destroy( pSurface );
    }

    void SAL_CALL CanvasBitmap::disposing()
    {
        tools::LocalGuard aGuard;

        // forward to parent
        CanvasBitmap_Base::disposing();
    }

#define SERVICE_NAME "com.sun.star.rendering.CanvasBitmap"

    ::rtl::OUString SAL_CALL CanvasBitmap::getImplementationName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CANVASBITMAP_IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL CanvasBitmap::supportsService( const ::rtl::OUString& ServiceName ) throw (uno::RuntimeException)
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL CanvasBitmap::getSupportedServiceNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );
        
        return aRet;
    }

    bool CanvasBitmap::repaint( const GraphicObjectSharedPtr&	rGrf,
                                const ::Point& 					rPt, 
                                const ::Size& 					rSz,
                                const GraphicAttr&				rAttr ) const
    {
        tools::LocalGuard aGuard;

        mbSurfaceDirty = true;

        return maCanvasHelper.repaint( rGrf, rPt, rSz, rAttr );
    }

    Cairo* CanvasBitmap::getCairo()
    {
	return mpCairo;
    }
}
