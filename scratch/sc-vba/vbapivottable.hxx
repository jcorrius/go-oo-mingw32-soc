#ifndef SC_VBA_PIVOTTABLE_HXX
#define SC_VBA_PIVOTTABLE_HXX
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/sheet/XDataPilotTable.hpp>
#include <org/openoffice/excel/XPivotTable.hpp>
#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1<oo::excel::XPivotTable > PivotTableImpl_BASE;

class ScVbaPivotTable : public PivotTableImpl_BASE
{		
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	css::uno::Reference< css::sheet::XDataPilotTable > m_xTable;
public:
	ScVbaPivotTable( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::sheet::XDataPilotTable >& xTable );
	virtual css::uno::Reference< oo::excel::XPivotCache > SAL_CALL PivotCache(  ) throw (css::uno::RuntimeException);
};

#endif //SC_VBA_PIVOTTABLE_HXX
