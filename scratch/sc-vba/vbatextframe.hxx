#ifndef SC_VBA_TEXTFRAME_HXX
#define SC_VBA_TEXTFRAME_HXX
#include <com/sun/star/drawing/XShape.hpp>
#include <org/openoffice/excel/XCharacters.hpp>
#include <org/openoffice/excel/XTextFrame.hpp>

#include "vbahelperinterface.hxx"
#include "vbapalette.hxx"

typedef InheritedHelperInterfaceImpl1< oo::excel::XTextFrame > ScVbaTextFrame_BASE;

class ScVbaTextFrame : public ScVbaTextFrame_BASE
{
private:
    css::uno::Reference< oo::excel::XCharacters > m_xCharacters;
    css::uno::Reference< css::drawing::XShape > m_xShape;
public:
    ScVbaTextFrame( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext , css::uno::Reference< css::drawing::XShape > xShape);
    virtual ~ScVbaTextFrame() {}
    // Attributes
    virtual sal_Bool SAL_CALL getAutoSize() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoSize( sal_Bool _autosize ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getMarginBottom() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMarginBottom( float _marginbottom ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getMarginTop() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMarginTop( float _margintop ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getMarginLeft() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMarginLeft( float _marginleft ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getMarginRight() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMarginRight( float _marginright ) throw (css::uno::RuntimeException);

    // Methods
    virtual css::uno::Reference< oo::excel::XCharacters > SAL_CALL Characters(  ) throw (css::uno::RuntimeException);

};

#endif//SC_VBA_TEXTFRAME_HXX
