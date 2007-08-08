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
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <org/openoffice/msforms/XShapeRange.hpp>
#include <org/openoffice/office/MsoAutoShapeType.hpp>

#include "vbashapes.hxx"
#include "vbashape.hxx"
#include "vbashaperange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShapes::ScVbaShapes( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess > xShapes ): ScVbaShapes_BASE( xParent, xContext, xShapes ), m_nNewShapeCount(0)
{
    m_xShapes.set( xShapes, uno::UNO_QUERY_THROW );
    m_xDrawPage.set( xShapes, uno::UNO_QUERY_THROW ); 
}

uno::Reference< container::XEnumeration >
ScVbaShapes::createEnumeration() throw (uno::RuntimeException)
{
    uno::Reference< container::XEnumerationAccess > xEnumAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
    return xEnumAccess->createEnumeration();
}

uno::Any
ScVbaShapes::createCollectionObject( const css::uno::Any& aSource )
{
    if( aSource.hasValue() )
    {
        uno::Reference< drawing::XShape > xShape( aSource, uno::UNO_QUERY_THROW );
        return uno::makeAny( uno::Reference< msforms::XShape >( new ScVbaShape( getParent(), mxContext, xShape, m_xShapes, ScVbaShape::getType( xShape ) ) ) );
    }
    return uno::Any();
}

uno::Any
ScVbaShapes::getItemByStringIndex( const rtl::OUString& sIndex ) throw (uno::RuntimeException)
{
    try
    {
        return ScVbaShapes_BASE::getItemByStringIndex( sIndex );
    }
    catch( uno::RuntimeException )
    {
        try
        {
            uno::Reference< drawing::XShape > xShape( getByName( sIndex ), uno::UNO_QUERY_THROW );
            return createCollectionObject( uno::makeAny( xShape ) );
        }
        catch( uno::RuntimeException )
        {
            throw uno::RuntimeException( rtl::OUString::createFromAscii("Unsatisified object name!"), uno::Reference< uno::XInterface >() );
        }
    }
}

uno::Type
ScVbaShapes::getElementType() throw (uno::RuntimeException)
{
    return org::openoffice::msforms::XShape::static_type(0);
}
rtl::OUString& 
ScVbaShapes::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaShapes") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaShapes::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.Shapes" ) );
	}
	return aServiceNames;
}


uno::Reference< msforms::XShapeRange > SAL_CALL 
ScVbaShapes::Range( const uno::Any& shapes ) throw (css::uno::RuntimeException)
{
    if( shapes.hasValue() )
    {
        uno::Reference< msforms::XShapeRange > xShapeRange;
        uno::Reference< drawing::XShapes > xShapes;
        sal_Int32 nIndex;
        rtl::OUString sIndex;
        uno::Sequence< sal_Int16 > nArray;
        uno::Sequence< rtl::OUString > sArray;
        if( shapes >>= nIndex )
        {
            return uno::Reference< msforms::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapeByIndex( nIndex ), m_xDrawPage ) );
        }
        else if( shapes >>= sIndex )
        {
            return uno::Reference< msforms::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapeByName( sIndex ), m_xDrawPage ) );
        }
        else if( shapes >>= nArray )
        {
            return uno::Reference< msforms::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapesByIndex( nArray ), m_xDrawPage ) );
        }
        else if( shapes >>= sArray )
        {
            return uno::Reference< msforms::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapesByNames( sArray ), m_xDrawPage ) );
        }
        else 
        {
            throw uno::RuntimeException( rtl::OUString::createFromAscii("Invalid Parameter!"), uno::Reference< uno::XInterface >() );
        }
    }
    else
    {
        throw uno::RuntimeException( rtl::OUString::createFromAscii("Parameter should not empty!"), uno::Reference< uno::XInterface >() );
    }
}

uno::Reference< drawing::XShapes>
ScVbaShapes::getEmptyShapesCollection() throw (uno::RuntimeException)
{
    uno::Reference< lang::XMultiServiceFactory > xMSF( mxContext->getServiceManager(), uno::UNO_QUERY_THROW );
    uno::Reference< uno::XInterface > xInterface = xMSF->createInstance( rtl::OUString::createFromAscii( "com.sun.star.drawing.ShapeCollection" ) );
    uno::Reference< drawing::XShapes > xShapes( xInterface, uno::UNO_QUERY_THROW );
    return xShapes;
}

uno::Reference< drawing::XShape >
ScVbaShapes::getByName( rtl::OUString sIndex ) throw (uno::RuntimeException)
{
    if( m_xNameAccess.is() )
        return  uno::Reference< drawing::XShape >( m_xNameAccess->getByName( sIndex ), uno::UNO_QUERY_THROW );
    else
    {
        uno::Reference< container::XIndexAccess > xIndexAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
        sal_uInt16 nCount = xIndexAccess->getCount();
        for( int index = 0; index < nCount; index++ )
        {
            uno::Any aUnoObj =  xIndexAccess->getByIndex( index );
            uno::Reference< drawing::XShape > xShape( aUnoObj, uno::UNO_QUERY_THROW );
            uno::Reference< container::XNamed > xNamed( xShape, uno::UNO_QUERY_THROW );
            if( sIndex.equals( xNamed->getName() ))
            {
                return xShape;
            }
        }
    }
    return uno::Reference< drawing::XShape >();
}

uno::Reference< drawing::XShapes > 
ScVbaShapes::getShapeByName( rtl::OUString sIndex ) throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShapes > xShapes( getEmptyShapesCollection(), uno::UNO_QUERY_THROW );
    try
    {
        uno::Reference< drawing::XShape > xShape( getByName( sIndex ), uno::UNO_QUERY_THROW );
        xShapes->add( xShape );
        return xShapes;
    }
    catch( uno::RuntimeException )
    {
        throw uno::RuntimeException( rtl::OUString::createFromAscii("Unsatisified object name!"), uno::Reference< uno::XInterface >() );
    }
}

uno::Reference< drawing::XShapes > 
ScVbaShapes::getShapeByIndex( sal_Int32 nIndex ) throw (uno::RuntimeException)
{
    if( !m_xIndexAccess.is() )
    {
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ScVbaCollectionBase numeric index access not supported by this object") ), uno::Reference< css::uno::XInterface >() );
    }
	if ( nIndex <= 0 )
	{
		throw  lang::IndexOutOfBoundsException( 
			rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			"index is 0 or negative" ) ), 
			uno::Reference< uno::XInterface >() );
	}
    uno::Reference< drawing::XShape > xShape( m_xIndexAccess->getByIndex( nIndex - 1 ), uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShapes > xShapes( getEmptyShapesCollection(), uno::UNO_QUERY_THROW );
    xShapes->add( xShape );
    return xShapes;
}

uno::Reference< drawing::XShapes > 
ScVbaShapes::getShapesByNames( uno::Sequence< rtl::OUString > sArray ) throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShapes > xShapes( getEmptyShapesCollection(), uno::UNO_QUERY_THROW );
    rtl::OUString sName;
    sal_Int32 nLength = sArray.getLength();
    if( nLength == 0 )
        throw uno::RuntimeException( rtl::OUString::createFromAscii("array of name is empty!"), uno::Reference< uno::XInterface >() );
    for( sal_Int32 i = 0; i < nLength; i++ )
    {
        sName = sArray[i];
        uno::Reference< drawing::XShapes > xTmpShapes( getShapeByName( sName ), uno::UNO_QUERY_THROW );
        uno::Reference< drawing::XShape > xShape( xTmpShapes->getByIndex(0), uno::UNO_QUERY_THROW );
        xShapes->add( xShape );
    }
    return xShapes; 
}

uno::Reference< drawing::XShapes > 
ScVbaShapes::getShapesByIndex( uno::Sequence< sal_Int16 > nArray ) throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShapes > xShapes( getEmptyShapesCollection(), uno::UNO_QUERY_THROW );
    sal_Int16 nIndex;
    sal_Int32 nLength = nArray.getLength();
    if( nLength == 0 )
        throw uno::RuntimeException( rtl::OUString::createFromAscii("array of index is empty!"), uno::Reference< uno::XInterface >() );
    for( sal_Int32 i = 0; i < nLength; i++ )
    {
        nIndex = nArray[i];
        uno::Reference< drawing::XShapes > xTmpShapes( getShapeByIndex( nIndex ), uno::UNO_QUERY_THROW );
        uno::Reference< drawing::XShape > xShape( xTmpShapes->getByIndex(0), uno::UNO_QUERY_THROW );
        xShapes->add( xShape );
    }
    return xShapes; 
}

void SAL_CALL 
ScVbaShapes::SelectAll() throw (uno::RuntimeException)
{
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XSelectionSupplier > xSelectSupp( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    xSelectSupp->select( uno::makeAny( m_xShapes ) );
}

uno::Reference< drawing::XShape > 
ScVbaShapes::createShape( rtl::OUString service ) throw (css::uno::RuntimeException)
{
    uno::Reference< lang::XMultiServiceFactory > xMSF( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShape > xShape( xMSF->createInstance( service ), uno::UNO_QUERY_THROW );
    return xShape;
}

uno::Any 
ScVbaShapes::AddRectangle( sal_Int32 startX, sal_Int32 startY, sal_Int32 nLineWidth, sal_Int32 nLineHeight, uno::Any aRange ) throw (css::uno::RuntimeException)
{
    rtl::OUString sCreateShapeName( rtl::OUString::createFromAscii( "com.sun.star.drawing.RectangleShape" ) );
    sal_Int32 nXPos = Millimeter::getInHundredthsOfOneMillimeter( startX );
    sal_Int32 nYPos = Millimeter::getInHundredthsOfOneMillimeter( startY );
    sal_Int32 nWidth = Millimeter::getInHundredthsOfOneMillimeter( nLineWidth );
    sal_Int32 nHeight = Millimeter::getInHundredthsOfOneMillimeter( nLineHeight );

    uno::Reference< drawing::XShape > xShape( createShape( sCreateShapeName ), uno::UNO_QUERY_THROW );
    m_xShapes->add( xShape );

    rtl::OUString sName = createName( rtl::OUString::createFromAscii( "Rectangle" ) );
    setDefaultShapeProperties( xShape );
    setShape_NameProperty( xShape, sName );

    awt::Point aMovePositionIfRange(0, 0);
    awt::Point position;
    position.X = nXPos - aMovePositionIfRange.X;
    position.Y = nYPos - aMovePositionIfRange.Y;
    xShape->setPosition( position );

    awt::Size size;
    size.Height = nHeight;
    size.Width = nWidth;
    xShape->setSize( size );

    ScVbaShape *pScVbaShape = new ScVbaShape( getParent(), mxContext, xShape, m_xShapes, ScVbaShape::getType( xShape ) );
    pScVbaShape->setRange( aRange ); 
    return uno::makeAny( uno::Reference< msforms::XShape > ( pScVbaShape ) );
}

uno::Any 
ScVbaShapes::AddEllipse( sal_Int32 startX, sal_Int32 startY, sal_Int32 nLineWidth, sal_Int32 nLineHeight, uno::Any aRange ) throw (css::uno::RuntimeException)
{
    rtl::OUString sCreateShapeName( rtl::OUString::createFromAscii( "com.sun.star.drawing.EllipseShape" ) );
    sal_Int32 nXPos = Millimeter::getInHundredthsOfOneMillimeter( startX );
    sal_Int32 nYPos = Millimeter::getInHundredthsOfOneMillimeter( startY );
    sal_Int32 nWidth = Millimeter::getInHundredthsOfOneMillimeter( nLineWidth );
    sal_Int32 nHeight = Millimeter::getInHundredthsOfOneMillimeter( nLineHeight );

    uno::Reference< drawing::XShape > xShape( createShape( sCreateShapeName ), uno::UNO_QUERY_THROW );
    m_xShapes->add( xShape );

    awt::Point aMovePositionIfRange( 0, 0 );
    //TODO helperapi using a writer document
    /*
    XDocument xDocument = (XDocument)getParent();
    if (AnyConverter.isVoid(_aRange))
    {
        _aRange = xDocument.Range(new Integer(0), new Integer(1));
        // Top&Left in Word is Top&Left of the paper and not the writeable area.
        aMovePositionIfRange = calculateTopLeftMargin((HelperInterfaceAdaptor)xDocument);
    }

    setShape_AnchorTypeAndRangeProperty(xShape, _aRange);
    */
    rtl::OUString name = createName( rtl::OUString::createFromAscii( "Oval" ));
    setDefaultShapeProperties(xShape);
    setShape_NameProperty(xShape, name);

    awt::Point position;
    position.X = nXPos - aMovePositionIfRange.X;
    position.Y = nYPos - aMovePositionIfRange.Y;
    xShape->setPosition(position);

    awt::Size size;
    size.Height = nHeight;
    size.Width = nWidth;
    xShape->setSize(size);

    ScVbaShape *pScVbaShape = new ScVbaShape( getParent(), mxContext, xShape, m_xShapes, ScVbaShape::getType( xShape ) );
    pScVbaShape->setRange( aRange ); 
    return uno::makeAny( uno::Reference< msforms::XShape > ( pScVbaShape ) );
}

//helpeapi calc
uno::Any SAL_CALL
ScVbaShapes::AddLine( sal_Int32 StartX, sal_Int32 StartY, sal_Int32 endX, sal_Int32 endY ) throw (uno::RuntimeException)
{
    sal_Int32 nLineWidth = endX - StartX;
    sal_Int32 nLineHeight = endY - StartY;

    sal_Int32 nHeight = Millimeter::getInHundredthsOfOneMillimeter( nLineHeight );
    sal_Int32 nWidth = Millimeter::getInHundredthsOfOneMillimeter( nLineWidth );
    sal_Int32 nXPos = Millimeter::getInHundredthsOfOneMillimeter( StartX );
    sal_Int32 nYPos = Millimeter::getInHundredthsOfOneMillimeter( StartY );
    
    uno::Reference< drawing::XShape > xShape( createShape( rtl::OUString::createFromAscii("com.sun.star.drawing.LineShape") ), uno::UNO_QUERY_THROW );
    m_xShapes->add( xShape );

    awt::Point aMovePositionIfRange( 0, 0 );
    
    rtl::OUString name = createName( rtl::OUString::createFromAscii( "Line" ) );
    setDefaultShapeProperties(xShape);
    setShape_NameProperty(xShape, name);

    awt::Point position;
    position.X = nXPos - aMovePositionIfRange.X;
    position.Y = nYPos - aMovePositionIfRange.Y;
    xShape->setPosition(position);

    awt::Size size;
    size.Height = nHeight;
    size.Width = nWidth;
    xShape->setSize(size);

    ScVbaShape *pScVbaShape = new ScVbaShape( getParent(), mxContext, xShape, m_xShapes, ScVbaShape::getType( xShape ) );
    return uno::makeAny( uno::Reference< msforms::XShape > ( pScVbaShape ) );
}

uno::Any SAL_CALL
ScVbaShapes::AddShape( sal_Int32 _nType, sal_Int32 _nLeft, sal_Int32 _nTop, sal_Int32 _nWidth, sal_Int32 _nHeight ) throw (uno::RuntimeException)
{
    uno::Any _aAnchor;
    if (_nType == office::MsoAutoShapeType::msoShapeRectangle)
    {
        return AddRectangle(_nLeft, _nTop, _nWidth, _nHeight, _aAnchor);
    }
    else if (_nType == office::MsoAutoShapeType::msoShapeOval)
    {
        return AddEllipse(_nLeft, _nTop, _nWidth, _nHeight, _aAnchor);
    }
    return uno::Any();
}

void
ScVbaShapes::setDefaultShapeProperties( uno::Reference< drawing::XShape > xShape ) throw (uno::RuntimeException)
{
    uno::Reference< beans::XPropertySet > xPropertySet( xShape, uno::UNO_QUERY_THROW );
    xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "FillStyle" ), uno::makeAny( rtl::OUString::createFromAscii( "SOLID" ) ) );
    xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "FillColor"), uno::makeAny( sal_Int32(0xFFFFFF) )  );
    xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "TextWordWrap"), uno::makeAny( text::WrapTextMode_THROUGHT )  );
    //not find in OOo2.3
    //xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "Opaque"), uno::makeAny( sal_True )  );
}

void
ScVbaShapes::setShape_NameProperty( uno::Reference< css::drawing::XShape > xShape, rtl::OUString sName )
{
    uno::Reference< beans::XPropertySet > xPropertySet( xShape, uno::UNO_QUERY_THROW );
    try
    {
        xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "Name" ), uno::makeAny( sName ) );
    }
    catch( script::BasicErrorException e )
    {
    }
}

rtl::OUString
ScVbaShapes::createName( rtl::OUString sName )
{
    sal_Int32 nActNumber = 1 + m_nNewShapeCount;
    m_nNewShapeCount++; 
    sName += rtl::OUString::valueOf( nActNumber );
    return sName;
}

awt::Point
calculateTopLeftMargin( uno::Reference< vba::XHelperInterface > xDocument )
{
    awt::Point aPoint( 0, 0 );
    uno::Reference< frame::XModel > xModel( xDocument, uno::UNO_QUERY_THROW );
    return awt::Point();
}
