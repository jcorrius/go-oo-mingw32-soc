#ifndef SC_VBA_ROWS_HXX
#define SC_VBA_ROWS_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/table/XTableRows.hpp>
#include <org/openoffice/vba/XRows.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
 
#include "vbahelper.hxx"
class ScModelObj;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XRows > ScVbaRows_BASE;

class ScVbaRows : public ScVbaRows_BASE
{
	uno::Reference< table::XTableRows > mxRows;
	uno::Reference< table::XCellRange > mxRange;
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
public:
	ScVbaRows( uno::Reference< table::XTableRows > xRows,uno::Reference< ::com::sun::star::uno::XComponentContext>& xContext,uno::Reference< table::XCellRange > xRange ): mxRows( xRows ),
    m_xContext( xContext ),
	mxRange( xRange ) {}
	virtual ~ScVbaRows() {}
	
	// XCollection
	virtual ::sal_Int32 SAL_CALL getCount() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL getParent() throw (uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getCreator() throw (uno::RuntimeException);
    virtual uno::Reference< ::org::openoffice::vba::XApplication > SAL_CALL getApplication() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL Item( const uno::Any& Index ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_ROWS_HXX */
