#ifndef SC_VBA_CHARTOBJECTS_HXX
#define SC_VBA_CHARTOBJECTS_HXX
#include <org/openoffice/vba/XChartObjects.hpp>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/table/XTableCharts.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>

#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper2<oo::vba::XChartObjects,
css::container::XEnumerationAccess > ChartObjectsImpl_BASE;

class ScVbaChartObjects : public ChartObjectsImpl_BASE
{		

	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	css::uno::Reference< css::table::XTableCharts > m_xTableCharts;
	css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);

	css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException);
public:
	ScVbaChartObjects( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::table::XTableCharts >& xTableCharts ) : m_xContext(xContext), m_xTableCharts( xTableCharts ) {}
	
	//XCollection
	virtual ::sal_Int32 SAL_CALL getCount() throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getParent() throw (css::uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getCreator() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XApplication > SAL_CALL getApplication() throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL Item( const css::uno::Any& Index ) throw (css::uno::RuntimeException);

	// XEnumerationAccess
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);	
	// XElementAccess
	virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL hasElements() throw (css::uno::RuntimeException);
};

#endif //SC_VBA_WINDOW_HXX
