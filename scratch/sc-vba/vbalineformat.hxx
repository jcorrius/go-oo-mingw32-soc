#ifndef SC_VBA_XLINEFORMAT_HXX
#define SC_VBA_XLINEFORMAT_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <org/openoffice/msforms/XLineFormat.hpp>
#include "vbahelperinterface.hxx"

typedef InheritedHelperInterfaceImpl1< oo::msforms::XLineFormat > ScVbaLineFormat_BASE;

class ScVbaLineFormat : public ScVbaLineFormat_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
protected:
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
public:
    ScVbaLineFormat( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape );

    // Attributes
    virtual sal_Int32 SAL_CALL getBeginArrowheadStyle() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setBeginArrowheadStyle( sal_Int32 _beginarrowheadstyle ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getBeginArrowheadLength() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setBeginArrowheadLength( sal_Int32 _beginarrowheadlength ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getBeginArrowheadWidth() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setBeginArrowheadWidth( sal_Int32 _beginarrowheadwidth ) throw (css::uno::RuntimeException);

};

#endif//SC_VBA_XLINEFORMAT_HXX
