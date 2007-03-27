#include<com/sun/star/container/XNamed.hpp>

#include "vbashape.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShape::ScVbaShape( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape ) throw( lang::IllegalArgumentException ) : ScVbaShape_BASE( xParent, xContext ), m_xShape( xShape )
{
}

ScVbaShape::ScVbaShape( const uno::Reference< uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape ) throw( lang::IllegalArgumentException ) : ScVbaShape_BASE( uno::Reference< vba::XHelperInterface >(), xContext ), m_xShape( xShape )
{
}

rtl::OUString SAL_CALL 
ScVbaShape::getName() throw (uno::RuntimeException)
{
    if( !m_sName.getLength() )
    {
        uno::Reference< container::XNamed > xNamed( m_xShape, uno::UNO_QUERY_THROW );
        m_sName = xNamed->getName();
    }
    return m_sName;
}

void SAL_CALL 
ScVbaShape::setName( const rtl::OUString& _name ) throw (uno::RuntimeException)
{
    uno::Reference< container::XNamed > xNamed( m_xShape, uno::UNO_QUERY_THROW );
    xNamed->setName( _name );
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
