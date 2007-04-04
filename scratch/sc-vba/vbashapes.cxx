#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <org/openoffice/msform/XShapeRange.hpp>

#include "vbashapes.hxx"
#include "vbashape.hxx"
#include "vbashaperange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShapes::ScVbaShapes( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess > xShapes ): ScVbaShapes_BASE( xParent, xContext, xShapes )
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
        return uno::makeAny( uno::Reference< msform::XShape >( new ScVbaShape( getParent(), mxContext, xShape, m_xShapes, ScVbaShape::getType( xShape ) ) ) );
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
    return org::openoffice::msform::XShapes::static_type(0);
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

//Method
uno::Any SAL_CALL 
ScVbaShapes::Item( const css::uno::Any& NameOrIndex ) throw (css::uno::RuntimeException)
{
    return ScVbaShapes_BASE::Item( NameOrIndex );
}

sal_Int32 SAL_CALL 
ScVbaShapes::Count(  ) throw (css::uno::RuntimeException)
{
    return getCount();
}

uno::Reference< msform::XShapeRange > SAL_CALL 
ScVbaShapes::Range( const uno::Any& shapes ) throw (css::uno::RuntimeException)
{
    if( shapes.hasValue() )
    {
        uno::Reference< msform::XShapeRange > xShapeRange;
        uno::Reference< drawing::XShapes > xShapes;
        sal_Int32 nIndex;
        rtl::OUString sIndex;
        uno::Sequence< sal_Int16 > nArray;
        uno::Sequence< rtl::OUString > sArray;
        if( shapes >>= nIndex )
        {
            return uno::Reference< msform::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapeByIndex( nIndex ), m_xDrawPage ) );
        }
        else if( shapes >>= sIndex )
        {
            return uno::Reference< msform::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapeByName( sIndex ), m_xDrawPage ) );
        }
        else if( shapes >>= nArray )
        {
            return uno::Reference< msform::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapesByIndex( nArray ), m_xDrawPage ) );
        }
        else if( shapes >>= sArray )
        {
            return uno::Reference< msform::XShapeRange >( new ScVbaShapeRange( getParent(), mxContext, getShapesByNames( sArray ), m_xDrawPage ) );
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
