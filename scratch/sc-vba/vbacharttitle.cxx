#include "vbacharttitle.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaChartTitle::ScVbaChartTitle( const uno::Reference< vba::XHelperInterface >& xParent,  const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape >& _xTitleShape ) :  ChartTitleBase( xParent, xContext, _xTitleShape )
{
}

rtl::OUString&
ScVbaChartTitle::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaChartTitle") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaChartTitle::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		uno::Sequence< rtl::OUString > BaseServiceNames = ChartTitleBase::getServiceNames();
		aServiceNames.realloc( BaseServiceNames.getLength() + 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.excel.Chart" ) );
		for ( sal_Int32 index = 1; index < (BaseServiceNames.getLength() + 1); ++index )
			aServiceNames[ index ] = BaseServiceNames[ index ];
	}
	return aServiceNames;
}


