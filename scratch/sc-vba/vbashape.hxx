#ifndef SC_VBA_SHAPE_HXX
#define SC_VBA_SHAPE_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <org/openoffice/msform/XShape.hpp>

#include "vbahelperinterface.hxx"

typedef InheritedHelperInterfaceImpl1< oo::msform::XShape > ScVbaShape_BASE;

class ScVbaShape : public ScVbaShape_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
    css::uno::Reference< css::drawing::XShapes > m_xShapes;
    css::uno::Reference< css::beans::XPropertySet > m_xPropertySet;
protected:
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
public:
    ScVbaShape( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape, const css::uno::Reference< css::drawing::XShapes > xShapes ) throw ( css::lang::IllegalArgumentException );
    ScVbaShape( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape ) throw ( css::lang::IllegalArgumentException );

    // Attributes
    virtual rtl::OUString SAL_CALL getName() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setName( const rtl::OUString& _name ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getHeight() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setHeight( double _height ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getWidth() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setWidth( double _width ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getLeft() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setLeft( double _left ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getTop() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTop( double _top ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getVisible() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setVisible( sal_Bool _visible ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getZOrderPosition() throw (css::uno::RuntimeException);

    // Methods
    virtual css::uno::Reference< oo::excel::XTextFrame > SAL_CALL TextFrame(  ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL Delete() throw (css::uno::RuntimeException);
    virtual void SAL_CALL ZOrder( sal_Int32 ZOrderCmd ) throw (css::uno::RuntimeException);
};
#endif//SC_VBA_SHAPE_HXX
