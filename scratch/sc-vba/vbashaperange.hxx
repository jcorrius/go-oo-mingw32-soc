#ifndef SC_VBA_SHAPERANGE_HXX
#define SC_VBA_SHAPERANGE_HXX

#include <com/sun/star/drawing/XShapes.hpp>
#include <org/openoffice/msform/XShapeRange.hpp>

#include "vbahelperinterface.hxx"
#include "vbahelper.hxx"

typedef InheritedHelperInterfaceImpl1< oo::msform::XShapeRange > ScVbaShapeRange_BASE;

class ScVbaShapeRange : public ScVbaShapeRange_BASE
{
private:
    css::uno::Reference< css::drawing::XShapes > m_xShapes;
    css::uno::Reference< css::drawing::XDrawPage > m_xDrawPage;
    sal_Int32 m_nShapeGroupCount;
protected:
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
public:
    ScVbaShapeRange( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, css::uno::Reference< css::drawing::XShapes > xShapes, css::uno::Reference< css::drawing::XDrawPage> xDrawShape );

    // Methods
    virtual void SAL_CALL Select(  ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< ::org::openoffice::msform::XShape > SAL_CALL Group() throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL Count(  ) throw (css::uno::RuntimeException);
};

#endif//SC_VBA_SHAPERANGE_HXX
