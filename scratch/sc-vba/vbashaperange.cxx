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
#include <com/sun/star/drawing/XShapeGrouper.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>

#include "vbashaperange.hxx"
#include "vbashape.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShapeRange::ScVbaShapeRange( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, uno::Reference< drawing::XShapes > xShapes, uno::Reference< drawing::XDrawPage > xDrawPage ) : ScVbaShapeRange_BASE( xParent, xContext ), m_xShapes( xShapes ), m_xDrawPage( xDrawPage ), m_nShapeGroupCount(0)
{
}

// Methods
void SAL_CALL 
ScVbaShapeRange::Select(  ) throw (uno::RuntimeException)
{
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XSelectionSupplier > xSelectSupp( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    xSelectSupp->select( uno::makeAny( m_xShapes ) );
}

uno::Reference< msforms::XShape > SAL_CALL 
ScVbaShapeRange::Group() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShapeGrouper > xShapeGrouper( m_xDrawPage, uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShapeGroup > xShapeGroup( xShapeGrouper->group( m_xShapes ), uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShape > xShape( xShapeGroup, uno::UNO_QUERY_THROW );
    return uno::Reference< msforms::XShape >( new ScVbaShape( getParent(), mxContext, xShape, m_xShapes, office::MsoShapeType::msoGroup ) );
}
sal_Int32 SAL_CALL 
ScVbaShapeRange::Count(  ) throw (uno::RuntimeException)
{
    uno::Reference< container::XIndexAccess > xIndexAccess( m_xShapes, uno::UNO_QUERY_THROW );
    return xIndexAccess->getCount();
}

rtl::OUString& 
ScVbaShapeRange::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaShapeRange") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaShapeRange::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.ShapeRange" ) );
	}
	return aServiceNames;
}
