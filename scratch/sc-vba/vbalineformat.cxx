#include "vbalineformat.hxx"

using namespace org::openoffice;
using namespace com::sun::star;

ScVbaLineFormat::ScVbaLineFormat( const uno::Reference< oo::vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape ) : ScVbaLineFormat_BASE( xParent, xContext ), m_xShape( xShape ) 
{
}

// Attributes
sal_Int32 SAL_CALL 
ScVbaLineFormat::getBeginArrowheadStyle() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaLineFormat::setBeginArrowheadStyle( sal_Int32 _beginarrowheadstyle ) throw (uno::RuntimeException)
{
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getBeginArrowheadLength() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaLineFormat::setBeginArrowheadLength( sal_Int32 _beginarrowheadlength ) throw (uno::RuntimeException)
{
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getBeginArrowheadWidth() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaLineFormat::setBeginArrowheadWidth( sal_Int32 _beginarrowheadwidth ) throw (uno::RuntimeException)
{
}


rtl::OUString& 
ScVbaLineFormat::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaLineFormat") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaLineFormat::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.LineFormat" ) );
	}
	return aServiceNames;
}


