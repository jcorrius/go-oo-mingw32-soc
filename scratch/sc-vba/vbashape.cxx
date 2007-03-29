#include<org/openoffice/excel/XlMsoZOrderCmd.hpp>
#include<com/sun/star/container/XNamed.hpp>

#include "vbashape.hxx"
#include "vbatextframe.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShape::ScVbaShape( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape, const uno::Reference< drawing::XShapes > xShapes ) throw( lang::IllegalArgumentException ) : ScVbaShape_BASE( xParent, xContext ), m_xShape( xShape ), m_xShapes( xShapes )
{
    m_xPropertySet.set( m_xShape, uno::UNO_QUERY_THROW );
}

ScVbaShape::ScVbaShape( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape ) throw( lang::IllegalArgumentException ) : ScVbaShape_BASE( uno::Reference< vba::XHelperInterface >(), xContext ), m_xShape( xShape )
{
}

// Attributes
rtl::OUString SAL_CALL 
ScVbaShape::getName() throw (uno::RuntimeException)
{
    rtl::OUString sName;
    uno::Reference< container::XNamed > xNamed( m_xShape, uno::UNO_QUERY_THROW );
    sName = xNamed->getName();
    return sName;
}

void SAL_CALL 
ScVbaShape::setName( const rtl::OUString& _name ) throw (uno::RuntimeException)
{
    uno::Reference< container::XNamed > xNamed( m_xShape, uno::UNO_QUERY_THROW );
    xNamed->setName( _name );
}

double SAL_CALL 
ScVbaShape::getHeight() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getSize().Height );
}

void SAL_CALL 
ScVbaShape::setHeight( double _height ) throw (uno::RuntimeException)
{
    awt::Size aSize( m_xShape->getSize() );
    aSize.Height = pointsToHundredthsMillimeter( _height );
    m_xShape->setSize( aSize );
}

double SAL_CALL 
ScVbaShape::getWidth() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getSize().Width );
}

void SAL_CALL 
ScVbaShape::setWidth( double _width ) throw (uno::RuntimeException)
{
    awt::Size aSize( m_xShape->getSize() );
    aSize.Width = pointsToHundredthsMillimeter( _width );
    m_xShape->setSize( aSize );
}

double SAL_CALL 
ScVbaShape::getLeft() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getPosition().X );
}

void SAL_CALL 
ScVbaShape::setLeft( double _left ) throw (uno::RuntimeException)
{
    awt::Point oldPosition;
    oldPosition = m_xShape->getPosition();
    oldPosition.X = pointsToHundredthsMillimeter( _left );
    m_xShape->setPosition( oldPosition );
}

double SAL_CALL 
ScVbaShape::getTop() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getPosition().Y );
}

void SAL_CALL 
ScVbaShape::setTop( double _top ) throw (uno::RuntimeException)
{
    awt::Point oldPosition;
    oldPosition = m_xShape->getPosition();
    oldPosition.Y = pointsToHundredthsMillimeter( _top );
    m_xShape->setPosition( oldPosition );
}

sal_Bool SAL_CALL 
ScVbaShape::getVisible() throw (uno::RuntimeException)
{
    //UNO Shapes are always visible
    return sal_True;
}

void SAL_CALL 
ScVbaShape::setVisible( sal_Bool _visible ) throw (uno::RuntimeException)
{
    //UNO Shapes are always visible
}

sal_Int32 SAL_CALL 
ScVbaShape::getZOrderPosition() throw (uno::RuntimeException)
{
    sal_Int32 nZOrderPosition;
    uno::Any aZOrderPosition =  m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ) );
    aZOrderPosition >>= nZOrderPosition;
    return nZOrderPosition + 1;
}

// Methods
uno::Reference< excel::XTextFrame > SAL_CALL 
ScVbaShape::TextFrame() throw (uno::RuntimeException)
{
    return uno::Reference< excel::XTextFrame >(new ScVbaTextFrame( getParent(), mxContext, m_xShape ) );
}

void SAL_CALL 
ScVbaShape::Delete() throw (uno::RuntimeException)
{
    m_xShapes->remove( m_xShape );
}

void SAL_CALL 
ScVbaShape::ZOrder( sal_Int32 ZOrderCmd ) throw (uno::RuntimeException)
{
    sal_Int32 nOrderPositon;
    uno::Any aOrderPostion = m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ) );
    aOrderPostion >>= nOrderPositon;
    switch( ZOrderCmd )
    {
    case excel::XlMsoZOrderCmd::msoBringToFront:
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( SAL_MAX_INT32 ) );
        break;
    case excel::XlMsoZOrderCmd::msoSendToBack:
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( (sal_Int32)0 ) );
        break;
    case excel::XlMsoZOrderCmd::msoBringForward:
        nOrderPositon += 1;
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( nOrderPositon ) );
        break;
    case excel::XlMsoZOrderCmd::msoSendBackward:
        if( nOrderPositon > 0 )
        {
            nOrderPositon -= 1;
            m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( nOrderPositon ) );
        }
        break;
    // below two commands use with Writer for text and image object.
    case excel::XlMsoZOrderCmd::msoBringInFrontOfText:
    case excel::XlMsoZOrderCmd::msoSendBehindText:
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "This ZOrderCmd is not implemented, it is use with writer." ), uno::Reference< uno::XInterface >() );
        break;
    default:
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "Invalid Parameter." ), uno::Reference< uno::XInterface >() );
    }
}


rtl::OUString& 
ScVbaShape::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaShape") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaShape::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.Shape" ) );
	}
	return aServiceNames;
}
