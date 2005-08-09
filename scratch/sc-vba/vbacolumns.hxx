#ifndef SC_VBA_COLUMNS_HXX
#define SC_VBA_COLUMNS_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/table/XTableColumns.hpp>
#include <org/openoffice/vba/XColumns.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
                                                                                                                             
#include "vbahelper.hxx"
class ScModelObj;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XColumns > ScVbaColumns_BASE;

class ScVbaColumns : public ScVbaColumns_BASE
{
	uno::Reference< table::XTableColumns > mxColumns;
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
	uno::Reference< table::XCellRange > mxRange;

public:
	ScVbaColumns(uno::Reference< table::XTableColumns > xColumns, uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext, uno::Reference< table::XCellRange > xRange ) : mxColumns( xColumns ), 
	m_xContext( xContext ),
	mxRange( xRange ) {}
	virtual ~ScVbaColumns() {}

    // XCollection
    virtual ::sal_Int32 SAL_CALL getCount() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL getParent() throw (uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getCreator() throw (uno::RuntimeException);
    virtual uno::Reference< ::org::openoffice::vba::XApplication > SAL_CALL getApplication() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL Item( const uno::Any& Index ) throw (uno::RuntimeException);

};

#endif /* SC_VBA_COLUMNS_HXX */
