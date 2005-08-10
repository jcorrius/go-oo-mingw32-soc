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

#ifndef  _USE_MATH_DEFINES
#define  _USE_MATH_DEFINES  // needed by Visual C++ for math constants
#endif
#include <math.h>           // M_PI definition 

#include <canvas/debug.hxx>
#include <canvas/canvastools.hxx>

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif 

#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif
#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_RANGE_B2DRECTANGLE_HXX
#include <basegfx/range/b2drectangle.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif
#ifndef _BGFX_TOOLS_TOOLS_HXX
#include <basegfx/tools/tools.hxx>
#endif

#include <limits>

#include "cairo_impltools.hxx"
#include "cairo_parametricpolypolygon.hxx"

using namespace ::com::sun::star;
using namespace ::cairo;

double calcAspectRatio( const geometry::RealRectangle2D& rBoundRect )
{
    const double nWidth( rBoundRect.X2 - rBoundRect.X1 );
    const double nHeight( rBoundRect.Y2 - rBoundRect.Y1 );

    return ::basegfx::fTools::equalZero( nHeight ) ? 1.0 : fabs( nWidth / nHeight );
}

namespace cairocanvas
{
    ParametricPolyPolygon* ParametricPolyPolygon::createLinearHorizontalGradient( const uno::Sequence< double >& 						leftColor, 
                                                                                  const uno::Sequence< double >& 						rightColor,
                                                                                  const uno::Reference< rendering::XGraphicDevice >& 	xDevice )
    {
        return new ParametricPolyPolygon( GRADIENT_LINEAR, leftColor, rightColor );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createAxialHorizontalGradient( const uno::Sequence< double >& 					middleColor, 
                                                                                 const uno::Sequence< double >& 					endColor,
                                                                                 const uno::Reference< rendering::XGraphicDevice >& xDevice )
    {
        return new ParametricPolyPolygon( GRADIENT_AXIAL, endColor, middleColor );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createEllipticalGradient( const uno::Sequence< double >& 						centerColor, 
                                                                            const uno::Sequence< double >& 						endColor,
                                                                            const geometry::RealRectangle2D&					boundRect,
                                                                            const uno::Reference< rendering::XGraphicDevice >& 	xDevice )
    {
	// NYI
	OSL_TRACE( "TODO: ParametricPolyPolygon::createEllipticalGradient\n" );
        return new ParametricPolyPolygon( GRADIENT_ELLIPTICAL, centerColor, endColor, calcAspectRatio (boundRect) );
    }

    ParametricPolyPolygon* ParametricPolyPolygon::createRectangularGradient( const uno::Sequence< double >& 					centerColor, 
                                                                             const uno::Sequence< double >& 					endColor,
                                                                             const geometry::RealRectangle2D&					boundRect,
                                                                             const uno::Reference< rendering::XGraphicDevice >& xDevice )
    {
	// NYI
	OSL_TRACE( "TODO: ParametricPolyPolygon::createRectangularGradient\n" );
        return new ParametricPolyPolygon( GRADIENT_RECTANGULAR, endColor, centerColor, calcAspectRatio( boundRect ) );
    }

    void SAL_CALL ParametricPolyPolygon::disposing()
    {
    }

    uno::Reference< rendering::XPolyPolygon2D > SAL_CALL ParametricPolyPolygon::getOutline( double t ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): outline NYI
        return uno::Reference< rendering::XPolyPolygon2D >();
    }

    uno::Sequence< double > SAL_CALL ParametricPolyPolygon::getColor( double t ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): color NYI
        return uno::Sequence< double >();
    }

    uno::Sequence< double > SAL_CALL ParametricPolyPolygon::getPointColor( const geometry::RealPoint2D& point ) throw (lang::IllegalArgumentException, uno::RuntimeException)
    {
        // TODO(F1): point color NYI
        return uno::Sequence< double >();
    }

#define SERVICE_NAME "com.sun.star.rendering.ParametricPolyPolygon"

    ::rtl::OUString SAL_CALL ParametricPolyPolygon::getImplementationName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( PARAMETRICPOLYPOLYGON_IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL ParametricPolyPolygon::supportsService( const ::rtl::OUString& ServiceName ) throw (uno::RuntimeException)
    {
        return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ) );
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL ParametricPolyPolygon::getSupportedServiceNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );

        return aRet;
    }


    ParametricPolyPolygon::~ParametricPolyPolygon()
    {
    }

    ParametricPolyPolygon::ParametricPolyPolygon( GradientType eType, const uno::Sequence< double >& rColor1, const uno::Sequence< double >& rColor2, double nAspectRatio ) :
        ParametricPolyPolygon_Base( m_aMutex ),        
        maGradientPoly(),
        mnAspectRatio( nAspectRatio ),
        maColor1( rColor1 ),
        maColor2( rColor2 ),
        meType( eType )
    {
    }

    namespace {
	void addColorStop( Pattern* pPattern, double nOffset, const uno::Sequence< double >& rColor )
	{
	    if( rColor.getLength() == 3 )
		cairo_pattern_add_color_stop_rgb( pPattern, nOffset, rColor[0], rColor[1], rColor[2] );
	    else if( rColor.getLength() == 4 )
		cairo_pattern_add_color_stop_rgba( pPattern, nOffset, rColor[0], rColor[1], rColor[2], rColor[3] );
	}
    }

    Pattern* ParametricPolyPolygon::getPattern( Matrix& rMatrix )
    {
	Pattern* pPattern = NULL;

	switch( meType) {
	case GRADIENT_LINEAR: {
	    double x0 = 0;
	    double y0 = 0;
	    double x1 = 1;
	    double y1 = 0;
	    cairo_matrix_transform_point( &rMatrix, &x0, &y0 );
	    cairo_matrix_transform_point( &rMatrix, &x1, &y1 );
	    pPattern = cairo_pattern_create_linear( x0, y0, x1, y1 );
	    addColorStop( pPattern, 0, maColor1 );
	    addColorStop( pPattern, 1, maColor2 );
	}
	break;
	// FIXME: NYI
	case GRADIENT_RECTANGULAR:
	case GRADIENT_AXIAL: {
	    double x0 = 0;
	    double y0 = 0;
	    double x1 = 1;
	    double y1 = 0;
	    cairo_matrix_transform_point( &rMatrix, &x0, &y0 );
	    cairo_matrix_transform_point( &rMatrix, &x1, &y1 );
	    pPattern = cairo_pattern_create_linear( x0, y0, x1, y1 );
	    addColorStop( pPattern, 0, maColor1 );
	    addColorStop( pPattern, 0.5, maColor2 );
	    addColorStop( pPattern, 1, maColor1 );
	}
	break;
	case GRADIENT_ELLIPTICAL: {
	    double cx = 0.5;
	    double cy = 0.5;
	    double r0 = 0;
	    double r1 = 0.5;
	    Matrix* pMatrix = &rMatrix;
	    Matrix aScaledMatrix, aScaleMatrix;

	    cairo_matrix_transform_point( &rMatrix, &cx, &cy );
	    cairo_matrix_transform_distance( &rMatrix, &r0, &r1 );
	    pPattern = cairo_pattern_create_radial( cx, cy, r0, cx, cy, r1 );
	    addColorStop( pPattern, 0, maColor1 );
	    addColorStop( pPattern, 1, maColor2 );

	    if( ! ::rtl::math::approxEqual( mnAspectRatio, 1 ) ) {
		cairo_matrix_init_scale( &aScaleMatrix, 1, mnAspectRatio );
		cairo_pattern_set_matrix( pPattern, &aScaleMatrix );
	    }
	}

	break;
	}

	return pPattern;
    }
}
