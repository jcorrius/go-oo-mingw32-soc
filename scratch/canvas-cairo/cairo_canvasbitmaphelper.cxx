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

#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
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
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#include <canvas/canvastools.hxx>

#include "cairo_canvasbitmap.hxx"


using namespace ::com::sun::star;

namespace cairocanvas
{
    CanvasBitmapHelper::CanvasBitmapHelper() :
        mpBackBuffer()
    { 
    }

    void CanvasBitmapHelper::setDevice( const WindowGraphicDevice::ImplRef&	rDevice )
    {
        ENSURE_AND_THROW( rDevice.is(),
                          "CanvasBitmapHelper::setBitmap(): Invalid reference device" );

        mpBackBuffer.reset( new BackBuffer( rDevice ) );

        // forward new settings to base class
        setGraphicDevice( rDevice );
    }

    void CanvasBitmapHelper::disposing()
    {
        mpBackBuffer.reset();

        // forward to base class
        CanvasHelper::disposing();
    }
    
    geometry::IntegerSize2D SAL_CALL CanvasBitmapHelper::getSize()
    {
        if( !mpBackBuffer.get() )
            return geometry::IntegerSize2D();

        return ::vcl::unotools::integerSize2DFromSize( mpBackBuffer->getSize() );
    }

    uno::Reference< rendering::XBitmap > SAL_CALL CanvasBitmapHelper::getScaledBitmap( const geometry::RealSize2D& 	newSize, 
                                                                                       sal_Bool 					beFast )
    {
	OSL_TRACE ("CanvasBitmapHelper::getScaledBitmap TODO\n");

        RTL_LOGFILE_CONTEXT( aLog, "::cairocanvas::CanvasBitmapHelper::getScaledBitmap()" );

        if( !mpBackBuffer.get() )
            return uno::Reference< rendering::XBitmap >(); // we're disposed

	return uno::Reference< rendering::XBitmap >(); // we're disposed
    }

    uno::Sequence< sal_Int8 > SAL_CALL CanvasBitmapHelper::getData( const geometry::IntegerRectangle2D& rect )
    {
	OSL_TRACE ("CanvasBitmapHelper::getData TODO\n");

        RTL_LOGFILE_CONTEXT( aLog, "::cairocanvas::CanvasBitmapHelper::getData()" );

        if( !mpBackBuffer.get() )
            return uno::Sequence< sal_Int8 >(); // we're disposed

	return uno::Sequence< sal_Int8 >(); // we're disposed
    }

    void SAL_CALL CanvasBitmapHelper::setData( const uno::Sequence< sal_Int8 >& 	data, 
                                               const geometry::IntegerRectangle2D&	rect )
    {
	OSL_TRACE ("CanvasBitmapHelper::setData TODO\n");

        RTL_LOGFILE_CONTEXT( aLog, "::cairocanvas::CanvasBitmapHelper::setData()" );

        if( !mpBackBuffer.get() )
            return; // we're disposed

        return; 
    }

    void SAL_CALL CanvasBitmapHelper::setPixel( const uno::Sequence< sal_Int8 >& 	color, 
                                                const geometry::IntegerPoint2D& 	pos )
    {
	OSL_TRACE ("CanvasBitmapHelper::setPixel TODO\n");

        RTL_LOGFILE_CONTEXT( aLog, "::cairocanvas::CanvasBitmapHelper::setPixel()" );

        if( !mpBackBuffer.get() )
            return; // we're disposed

	return;
    }

    uno::Sequence< sal_Int8 > SAL_CALL CanvasBitmapHelper::getPixel( const geometry::IntegerPoint2D& pos )
    {
	OSL_TRACE ("CanvasBitmapHelper::getPixel TODO\n");

        RTL_LOGFILE_CONTEXT( aLog, "::cairocanvas::CanvasBitmapHelper::getPixel()" );

        if( !mpBackBuffer.get() )
            return uno::Sequence< sal_Int8 >(); // we're disposed

	return uno::Sequence< sal_Int8 >(); // we're disposed
    }

    uno::Reference< rendering::XBitmapPalette > SAL_CALL CanvasBitmapHelper::getPalette()
    {
        // TODO(F1): Provide palette support
        return uno::Reference< rendering::XBitmapPalette >();
    }

    rendering::IntegerBitmapLayout SAL_CALL CanvasBitmapHelper::getMemoryLayout()
    {
        // TODO(F1): finish that one
        rendering::IntegerBitmapLayout aLayout;

        if( !mpBackBuffer.get() )
            return aLayout; // we're disposed

        const Size aBmpSize( mpBackBuffer->getSize() );

        aLayout.ScanLines = aBmpSize.Height();
        aLayout.ScanLineBytes = aBmpSize.Width()*4;
        aLayout.ScanLineStride = aLayout.ScanLineBytes;
        aLayout.Format = 0;
        aLayout.NumComponents = 4;
        aLayout.ComponentMasks = uno::Sequence<sal_Int64>();
        aLayout.Endianness = 0;
        aLayout.IsPseudoColor = false;

        return aLayout;
    }
}
