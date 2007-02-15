#include "vbaseriescollection.hxx"
#include <org/openoffice/excel/XSeries.hpp>

#include "vbaglobals.hxx"

using namespace ::com::sun::star;
using namespace ::org::openoffice;

uno::Reference< oo::excel::XApplication > 
ScVbaSeriesCollection::getApplication() throw (uno::RuntimeException)
{
	return ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
}
::sal_Int32
ScVbaSeriesCollection::getCount() throw (uno::RuntimeException)
{
	return 0;
}

uno::Any
ScVbaSeriesCollection::Item( const uno::Any& /*aIndex*/ ) throw (uno::RuntimeException)
{
	// #TODO #FIXME some implementation would be nice !!
	return uno::Any();
}

// XEnumerationAccess

uno::Reference< container::XEnumeration >
ScVbaSeriesCollection::createEnumeration() throw (uno::RuntimeException)
{
	uno::Reference< container::XEnumeration > xEnum;
	return xEnum;
}

// XElementAccess

uno::Type 
ScVbaSeriesCollection::getElementType() throw (uno::RuntimeException)
{
	return excel::XSeries::static_type(0);
}
::sal_Bool
ScVbaSeriesCollection::hasElements() throw (uno::RuntimeException)
{
	// #TODO #TOFIX Really?, how can we say that!
	// needs to delegate to xIndex
	return sal_True;
}

uno::Any SAL_CALL 
ScVbaSeriesCollection::getParent() throw (uno::RuntimeException)
{
	uno::Reference< excel::XApplication > xApplication =
		getApplication();
	uno::Reference< excel::XWorkbook > xWorkbook;
	if ( xApplication.is() )
	{
		xWorkbook = xApplication->getActiveWorkbook();
	}
	return uno::Any( xWorkbook );
}

sal_Int32 SAL_CALL 
ScVbaSeriesCollection::getCreator() throw (uno::RuntimeException)
{
	// #TODO# #FIXME# implementation?
	return 0;
}





