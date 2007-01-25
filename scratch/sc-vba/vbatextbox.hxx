#ifndef SC_VBA_TEXTBOX_HXX
#define SC_VBA_TEXTBOX_HXX
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XTextBox.hpp>
#include <comphelper/propertycontainer.hxx>

#include "vbacontrol.hxx"
#include "vbahelper.hxx"

typedef cppu::ImplInheritanceHelper1< ScVbaControl, oo::vba::XTextBox > TextBoxImpl_BASE;

class ScVbaTextBox : public TextBoxImpl_BASE
    ,public ::comphelper::OPropertyContainer
{
public:
    ScVbaTextBox( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
   // Attributes
    virtual rtl::OUString SAL_CALL getText() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setText( const rtl::OUString& _text ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getMaxLength() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMaxLength( sal_Int32 _maxlength ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getMultiline() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMultiline( sal_Bool _multiline ) throw (css::uno::RuntimeException);

    // XPropertySet
    virtual css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (css::uno::RuntimeException);

    // XInterface
    DECLARE_XINTERFACE()

    // XTypeProvider
    DECLARE_XTYPEPROVIDER()

protected:
    // OPropertySetHelper
    virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

    virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;

};
#endif //SC_VBA_TEXTBOX_HXX
