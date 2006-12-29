#include <com/sun/star/form/FormComponentType.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/view/XControlAccess.hpp>

#include"vbacontrol.hxx"
#include"vbacombobox.hxx"

using namespace com::sun::star;
using namespace org::openoffice;

//ScVbaControl

ScVbaControl::ScVbaControl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< ::drawing::XControlShape >& xControlShape ) : m_xContext( xContext ), m_xControlShape( xControlShape )
{
    //init m_xProps
    uno::Reference< awt::XControlModel > xControlModel( xControlShape->getControl(), css::uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xControlModel, css::uno::UNO_QUERY_THROW );
    m_xProps = xProps;
    //init m_xWindowPeer
    css::uno::Reference< css::frame::XModel > xModel = getCurrentDocument();
    css::uno::Reference< css::view::XControlAccess > xControlAccess( xModel->getCurrentController(), css::uno::UNO_QUERY_THROW );
    m_xWindowPeer = xControlAccess->getControl( xControlModel )->getPeer();
}

ScVbaControl::ScVbaControl( const uno::Reference< uno::XComponentContext >& xContext, 
        const uno::Reference< beans::XPropertySet >& xProps, 
        const css::uno::Reference< css::drawing::XControlShape > xControlShape ) : m_xContext( xContext ), 
        m_xProps( xProps ), m_xControlShape( xControlShape )
{
    // grab the default value property name
    //m_xProps->getPropertyValue( CONTROLSOURCEPROP ) >>= sSourceName;
    //setDfltPropHandler();
    css::uno::Reference< css::awt::XControlModel > xControlModel( m_xProps, css::uno::UNO_QUERY_THROW );
    css::uno::Reference< css::frame::XModel > xModel = getCurrentDocument();
    css::uno::Reference< css::view::XControlAccess > xControlAccess( xModel->getCurrentController(), css::uno::UNO_QUERY_THROW );
    m_xWindowPeer = xControlAccess->getControl( xControlModel )->getPeer();

    // XShapeDescriptor
        
}

/*
void
ScVbaControl::Init( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::beans::XPropertySet >& xProps )
{
    m_xContext = xContext;
    m_xProps = xProps;
}
*/
sal_Bool SAL_CALL ScVbaControl::getEnabled() throw (css::uno::RuntimeException)
{
    uno::Any aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enabled" ) ) );
    sal_Bool bRet = false;
    aValue >>= bRet;
    return bRet;
}

void SAL_CALL ScVbaControl::setEnabled( sal_Bool bVisible ) throw (css::uno::RuntimeException)
{
    uno::Any aValue( bVisible );
    m_xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enabled" ) ), aValue);

}

sal_Bool SAL_CALL ScVbaControl::getVisible() throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::awt::XWindow2 > xWindow2( m_xWindowPeer, css::uno::UNO_QUERY_THROW );
    return xWindow2->isVisible();
}

void SAL_CALL ScVbaControl::setVisible( sal_Bool bVisible ) throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::awt::XWindow2 > xWindow2( m_xWindowPeer, css::uno::UNO_QUERY_THROW );
    xWindow2->setVisible( bVisible );
}
uno::Any SAL_CALL ScVbaControl::getSize() throw (css::uno::RuntimeException)
{
    uno::Reference< drawing::XShape > xShape( m_xControlShape, css::uno::UNO_QUERY_THROW );
    return uno::makeAny(  xShape->getSize() );
}
void SAL_CALL ScVbaControl::setSize( const uno::Any& _size ) throw (css::uno::RuntimeException)
{
    awt::Size aSize;
    _size >>= aSize;
    uno::Reference< drawing::XShape > xShape( m_xControlShape, css::uno::UNO_QUERY_THROW );
    xShape->setSize( aSize );
}


//ScVbaControlFactory

ScVbaControlFactory::ScVbaControlFactory( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XControlShape >& xControlShape ): m_xContext( xContext ), m_xControlShape( xControlShape )
{
    uno::Reference< awt::XControlModel > xControlModel( xControlShape->getControl(), css::uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xControlModel, css::uno::UNO_QUERY_THROW );
    m_xProps = xProps;
}

ScVbaControlFactory::ScVbaControlFactory( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< beans::XPropertySet >& xProps, const css::uno::Reference< css::drawing::XControlShape > xControlShape ): m_xContext( xContext ), m_xProps( xProps ), m_xControlShape( xControlShape )
{
}

ScVbaControl* ScVbaControlFactory::createControl( const sal_Int16 nClassId )  throw (uno::RuntimeException)
{
    switch( nClassId )
    {
        case form::FormComponentType::COMBOBOX:
            return new ScVbaComboBox( m_xContext, m_xProps, m_xControlShape );
        default:
            throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Donot surpport this Control Type." ), uno::Reference< uno::XInterface >() );
    }
}

ScVbaControl* ScVbaControlFactory::createControl()  throw (uno::RuntimeException)
{
    sal_Int32 nClassId = -1;
    const static rtl::OUString sClassId( RTL_CONSTASCII_USTRINGPARAM("ClassId") );
    m_xProps->getPropertyValue( sClassId ) >>= nClassId;
    switch( nClassId )
    {
        case form::FormComponentType::COMBOBOX:
            return new ScVbaComboBox( m_xContext, m_xControlShape );
        default:
            throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Donot surpport this Control Type." ), uno::Reference< uno::XInterface >() );
    }
}
