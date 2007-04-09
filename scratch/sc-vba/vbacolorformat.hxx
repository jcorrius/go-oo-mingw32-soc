#ifndef SC_VBA_XCOLORFORMAT_HXX
#define SC_VBA_XCOLORFORMAT_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <org/openoffice/msforms/XColorFormat.hpp>
#include "vbahelperinterface.hxx"

typedef InheritedHelperInterfaceImpl1< oo::msforms::XColorFormat > ScVbaColorFormat_BASE;

class ColorFormatType
{
public:
    const static sal_Int16 LINEFORMAT_FORECOLOR = 1;
    const static sal_Int16 LINEFORMAT_BACKCOLOR = 2;
    const static sal_Int16 FILLFORMAT_FORECOLOR = 3;
    const static sal_Int16 FILLFORMAT_BACKCOLOR = 4;
    const static sal_Int16 THREEDFORMAT_EXTRUSIONCOLOR = 5;
};

class ScVbaColorFormat : public ScVbaColorFormat_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
    css::uno::Reference< css::beans::XPropertySet > m_xPropertySet;
    css::uno::Reference< oo::vba::XHelperInterface > m_xInternalParent;
    sal_Int16 m_nColorFormatType;
    sal_Int32 m_nFillFormatBackColor;
protected:
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
    void setColorFormat( sal_Int16 nType );
public:
    ScVbaColorFormat( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< oo::vba::XHelperInterface > xInternalParent, const css::uno::Reference< css::drawing::XShape > xShape, const sal_Int16 nColorFormatType );

    // Attributes
    virtual sal_Int32 SAL_CALL getRGB() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setRGB( sal_Int32 _rgb ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getSchemeColor() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSchemeColor( sal_Int32 _schemecolor ) throw (css::uno::RuntimeException);

};

#endif//SC_VBA_XCOLORFORMAT_HXX
