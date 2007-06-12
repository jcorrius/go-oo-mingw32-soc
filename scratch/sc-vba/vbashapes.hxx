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
#ifndef SC_VBA_SHAPES_HXX
#define SC_VBA_SHAPES_HXX

#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <org/openoffice/msforms/XShapes.hpp>

#include "vbahelperinterface.hxx"

#include "vbacollectionimpl.hxx"

typedef CollTestImplHelper< oo::msforms::XShapes > ScVbaShapes_BASE;

class ScVbaShapes : public ScVbaShapes_BASE
{
private: 
    css::uno::Reference< css::drawing::XShapes > m_xShapes;
    css::uno::Reference< css::drawing::XDrawPage > m_xDrawPage;
    sal_Int32 m_nNewShapeCount;
protected:
    virtual css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
    virtual css::uno::Reference< css::drawing::XShape > getByName( rtl::OUString sIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getEmptyShapesCollection() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapeByName( rtl::OUString sIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapeByIndex( sal_Int32  nIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapesByNames( css::uno::Sequence< rtl::OUString > sArray ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapesByIndex( css::uno::Sequence< sal_Int16 > nArray ) throw (css::uno::RuntimeException);
    css::uno::Reference< css::drawing::XShape > createShape( rtl::OUString service ) throw (css::uno::RuntimeException);
    css::uno::Any AddRectangle( sal_Int32 startX, sal_Int32 startY, sal_Int32 nLineWidth, sal_Int32 nLineHeight, css::uno::Any aRange ) throw (css::uno::RuntimeException);
    css::uno::Any AddEllipse( sal_Int32 startX, sal_Int32 startY, sal_Int32 nLineWidth, sal_Int32 nLineHeight, css::uno::Any aRange ) throw (css::uno::RuntimeException);
    rtl::OUString createName( rtl::OUString sName );
    css::awt::Point calculateTopLeftMargin( css::uno::Reference< oo::vba::XHelperInterface > xDocument );
public:
    ScVbaShapes( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess > xShapes );
    static void setDefaultShapeProperties( css::uno::Reference< css::drawing::XShape > xShape ) throw (css::uno::RuntimeException);
    static void setShape_NameProperty( css::uno::Reference< css::drawing::XShape > xShape, rtl::OUString sName );
    //XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // ScVbaCollectionBaseImpl
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    // Methods
    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& NameOrIndex ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL Count() throw (css::uno::RuntimeException);
    virtual void SAL_CALL SelectAll() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< oo::msforms::XShapeRange > SAL_CALL Range( const css::uno::Any& shapes ) throw (css::uno::RuntimeException);
    //helper::calc
    virtual css::uno::Any SAL_CALL AddLine( sal_Int32 StartX, sal_Int32 StartY, sal_Int32 endX, sal_Int32 endY ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL AddShape( sal_Int32 ShapeType, sal_Int32 StartX, sal_Int32 StartY, sal_Int32 endX, sal_Int32 endY ) throw (css::uno::RuntimeException);
};

#endif//SC_VBA_SHAPES_HXX
