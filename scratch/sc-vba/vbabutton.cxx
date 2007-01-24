#include "vbabutton.hxx"
#include <vector>

using namespace com::sun::star;
using namespace org::openoffice;


const static rtl::OUString LABEL( RTL_CONSTASCII_USTRINGPARAM("Label") );
// name is not defineable in IDL so no chance of a false detection of the
// another property/method of the same name

ScVbaButton::ScVbaButton( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< css::drawing::XControlShape >& xControlShape ) : ButtonImpl_BASE( xContext, xControlShape ), OPropertyContainer(GetBroadcastHelper())
{
	// grab the default value property name
}

// Attributes
rtl::OUString SAL_CALL 
ScVbaButton::getCaption() throw (css::uno::RuntimeException)
{
    rtl::OUString Label;
    m_xProps->getPropertyValue( LABEL ) >>= Label;
    return Label;
}

void SAL_CALL 
ScVbaButton::setCaption( const rtl::OUString& _caption ) throw (::com::sun::star::uno::RuntimeException)
{
    m_xProps->setPropertyValue( LABEL, uno::makeAny( _caption ) );
}

// XInterface

IMPLEMENT_FORWARD_XINTERFACE2( ScVbaButton, ButtonImpl_BASE, OPropertyContainer )

// XTypeProvider

IMPLEMENT_FORWARD_XTYPEPROVIDER2( ScVbaButton, ButtonImpl_BASE, OPropertyContainer )

// OPropertySetHelper

::cppu::IPropertyArrayHelper& 
ScVbaButton::getInfoHelper(  )
{
    static ::cppu::IPropertyArrayHelper* sProps = 0;
    if ( !sProps )
        sProps = createArrayHelper();
    return *sProps;
}


::cppu::IPropertyArrayHelper* 
ScVbaButton::createArrayHelper(  ) const
{
    uno::Sequence< beans::Property > aProps;
    describeProperties( aProps );
    return new ::cppu::OPropertyArrayHelper( aProps );
}

// XPropertySet
uno::Reference< beans::XPropertySetInfo > 
ScVbaButton::getPropertySetInfo(  ) throw (uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

