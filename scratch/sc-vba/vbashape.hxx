#ifndef SC_VBA_SHAPE_HXX
#define SC_VBA_SHAPE_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <org/openoffice/msform/XShape.hpp>

#include "vbahelperinterface.hxx"

typedef InheritedHelperInterfaceImpl1< oo::msform::XShape > ScVbaShape_BASE;

class ScVbaShape : public ScVbaShape_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
    rtl::OUString m_sName;
protected:
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
public:
    ScVbaShape( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape ) throw ( css::lang::IllegalArgumentException );
    ScVbaShape( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape ) throw ( css::lang::IllegalArgumentException );

    virtual rtl::OUString SAL_CALL getName() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setName( const rtl::OUString& _name ) throw (css::uno::RuntimeException);
};
#endif//SC_VBA_SHAPE_HXX
