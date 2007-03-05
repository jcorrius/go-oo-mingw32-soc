
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/view/XControlAccess.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "vbaoleobject.hxx"

using namespace com::sun::star;
using namespace org::openoffice;

uno::Reference< css::awt::XWindowPeer >
ScVbaOLEObject::getWindowPeer( const uno::Reference< ::drawing::XControlShape >& xControlShape ) throw (uno::RuntimeException)
{
    uno::Reference< awt::XControlModel > xControlModel( xControlShape->getControl(), uno::UNO_QUERY_THROW );
    //init m_xWindowPeer
    uno::Reference< container::XChild > xChild( xControlModel, uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XControlAccess > xControlAccess( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    uno::Reference< awt::XControl > xControl;
    try
    {
        xControl.set( xControlAccess->getControl( xControlModel ), uno::UNO_QUERY );
    }
    catch( uno::Exception )
    {
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "The Control does not exsit" ),
                uno::Reference< uno::XInterface >() );
    }
    return xControl->getPeer();
}

ScVbaOLEObject::ScVbaOLEObject( const uno::Reference< uno::XComponentContext >& xContext,
            css::uno::Reference< css::drawing::XControlShape > xControlShape )
: m_xContext( xContext ), m_xControlShape( xControlShape )
{
}

uno::Reference< uno::XInterface > SAL_CALL 
ScVbaOLEObject::getObject() throw (uno::RuntimeException)
{
    return uno::Reference< uno::XInterface >( m_xControlShape, uno::UNO_QUERY_THROW );
}

sal_Bool SAL_CALL 
ScVbaOLEObject::getEnabled() throw (uno::RuntimeException)
{
    uno::Reference< beans::XPropertySet > xProps( m_xControlShape, uno::UNO_QUERY_THROW );
    uno::Any aValue = xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enabled" ) ) );
    sal_Bool bRet = false;
    aValue >>= bRet;
    return bRet;
}

void SAL_CALL 
ScVbaOLEObject::setEnabled( sal_Bool _enabled ) throw (uno::RuntimeException)
{
    uno::Reference< beans::XPropertySet > xProps( m_xControlShape, uno::UNO_QUERY_THROW );
    uno::Any aValue( _enabled );
    xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enabled" ) ), aValue);
}

sal_Bool SAL_CALL 
ScVbaOLEObject::getVisible() throw (uno::RuntimeException)
{
    uno::Reference< css::awt::XWindow2 > xWindow2( getWindowPeer( m_xControlShape ), css::uno::UNO_QUERY_THROW );
    return xWindow2->isVisible();
}

void SAL_CALL 
ScVbaOLEObject::setVisible( sal_Bool _visible ) throw (uno::RuntimeException)
{
    uno::Reference< css::awt::XWindow2 > xWindow2( getWindowPeer( m_xControlShape ), css::uno::UNO_QUERY_THROW );
    xWindow2->setVisible( _visible );
}

double SAL_CALL 
ScVbaOLEObject::getLeft() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    return mm2pt( xShape->getPosition().X / 100 );
}

void SAL_CALL 
ScVbaOLEObject::setLeft( double _left ) throw (uno::RuntimeException)
{
    awt::Point oldPosition;
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    oldPosition = xShape->getPosition();
    oldPosition.X = pt2mm( _left ) * 100;
    xShape->setPosition( oldPosition );

}

double SAL_CALL 
ScVbaOLEObject::getTop() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    return mm2pt( xShape->getPosition().Y / 100 );
}

void SAL_CALL 
ScVbaOLEObject::setTop( double _top ) throw (uno::RuntimeException)
{
    awt::Point oldPosition;
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    oldPosition = xShape->getPosition();
    oldPosition.Y = pt2mm( _top ) * 100;;
    xShape->setPosition( oldPosition );
}

double SAL_CALL 
ScVbaOLEObject::getHeight() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    return mm2pt( xShape->getSize().Height / 100 );//1pt = 1/72in
}

void SAL_CALL 
ScVbaOLEObject::setHeight( double _height ) throw (uno::RuntimeException)
{
    awt::Size oldSize;
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    oldSize = xShape->getSize();
    oldSize.Height = pt2mm( _height ) * 100;
    xShape->setSize( oldSize );
}

double SAL_CALL 
ScVbaOLEObject::getWidth() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    return mm2pt ( xShape->getSize().Width / 100 );
}

void SAL_CALL 
ScVbaOLEObject::setWidth( double _width ) throw (uno::RuntimeException)
{
    awt::Size oldSize;
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    oldSize = xShape->getSize();
    oldSize.Width = pt2mm( _width ) * 100;
    xShape->setSize( oldSize );
}

