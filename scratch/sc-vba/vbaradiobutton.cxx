#include "vbaradiobutton.hxx"
#include <vector>

using namespace com::sun::star;
using namespace org::openoffice;


const static rtl::OUString LABEL( RTL_CONSTASCII_USTRINGPARAM("Label") );
const static rtl::OUString STATE( RTL_CONSTASCII_USTRINGPARAM("State") );
// name is not defineable in IDL so no chance of a false detection of the
// another property/method of the same name

ScVbaRadioButton::ScVbaRadioButton( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< css::drawing::XControlShape >& xControlShape ) : RadioButtonImpl_BASE( xContext, xControlShape ), OPropertyContainer(GetBroadcastHelper())
{
	// grab the default value property name
}

// Attributes
rtl::OUString SAL_CALL 
ScVbaRadioButton::getCaption() throw (css::uno::RuntimeException)
{
    rtl::OUString Label;
    m_xProps->getPropertyValue( LABEL ) >>= Label;
    return Label;
}

void SAL_CALL 
ScVbaRadioButton::setCaption( const rtl::OUString& _caption ) throw (::com::sun::star::uno::RuntimeException)
{
    m_xProps->setPropertyValue( LABEL, uno::makeAny( _caption ) );
}

sal_Bool SAL_CALL 
ScVbaRadioButton::getValue() throw (css::uno::RuntimeException)
{
    sal_Bool bValue = sal_False;
    sal_Int16 nValue;
    m_xProps->getPropertyValue( STATE ) >>= nValue;
    if( nValue != 0 )
        bValue = sal_True;
    return bValue;
}

void SAL_CALL 
ScVbaRadioButton::setValue( sal_Bool _value ) throw (css::uno::RuntimeException)
{
    sal_Int16 nValue = 0;
    if( _value )
        nValue = 1;
    m_xProps->setPropertyValue( STATE, uno::makeAny( nValue ) );
}

// XInterface

IMPLEMENT_FORWARD_XINTERFACE2( ScVbaRadioButton, RadioButtonImpl_BASE, OPropertyContainer )

// XTypeProvider

IMPLEMENT_FORWARD_XTYPEPROVIDER2( ScVbaRadioButton, RadioButtonImpl_BASE, OPropertyContainer )

// OPropertySetHelper

::cppu::IPropertyArrayHelper& 
ScVbaRadioButton::getInfoHelper(  )
{
    static ::cppu::IPropertyArrayHelper* sProps = 0;
    if ( !sProps )
        sProps = createArrayHelper();
    return *sProps;
}


::cppu::IPropertyArrayHelper* 
ScVbaRadioButton::createArrayHelper(  ) const
{
    uno::Sequence< beans::Property > aProps;
    describeProperties( aProps );
    return new ::cppu::OPropertyArrayHelper( aProps );
}

// XPropertySet
uno::Reference< beans::XPropertySetInfo > 
ScVbaRadioButton::getPropertySetInfo(  ) throw (uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

