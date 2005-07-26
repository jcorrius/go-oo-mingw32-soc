#ifndef SC_VBA_OUTLINE_HXX
#define SC_VBA_OUTLINE_HXX

#include <com/sun/star/sheet/XSheetOutline.hpp>
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XOutline.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

using namespace ::org::openoffice;
using namespace ::com::sun::star;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XOutline > ScVbaOutline_BASE;

class ScVbaOutline :  public ScVbaOutline_BASE 
{
	uno::Reference< sheet::XSheetOutline > mxOutline;
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
public:
	ScVbaOutline( uno::Reference< uno::XComponentContext >& xContext,
		 uno::Reference<sheet::XSheetOutline> outline):m_xContext(xContext),mxOutline(outline)
	{}
	virtual ~ScVbaOutline(){}
	
	virtual void SAL_CALL ShowLevels( const uno::Any& RowLevels, const uno::Any& ColumnLevels ) throw (uno::RuntimeException) ;
};
#endif
