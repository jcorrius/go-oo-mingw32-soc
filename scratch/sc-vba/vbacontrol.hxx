#ifndef SC_VBA_CONTROL_HXX
#define SC_VBA_CONTROL_HXX

#include <cppuhelper/implbase1.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <org/openoffice/msforms/XControl.hpp>

#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< oo::msforms::XControl > ControlImpl_BASE;

class ScVbaControl : public ControlImpl_BASE
    ,public ::comphelper::OMutexAndBroadcastHelper
{
/* this will cause error when the sheet is invisible.
private:
    css::uno::Reference< css::awt::XWindowPeer > m_xWindowPeer;
*/
protected:
    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::beans::XPropertySet > m_xProps;
    css::uno::Reference< css::drawing::XControlShape > m_xControlShape;
protected:
    ScVbaControl(){ m_xContext = 0; m_xProps = 0; m_xControlShape = 0; }
    void SetControl( const css::uno::Reference< css::uno::XComponentContext > xContext, 
                const css::uno::Reference< css::drawing::XControlShape > xControlShape );
    void SetControl( const css::uno::Reference< css::uno::XComponentContext > xContext, 
                const css::uno::Reference< css::beans::XPropertySet > xProps,
                const css::uno::Reference< css::drawing::XControlShape > xControlShape );
public:
    ScVbaControl( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
    ScVbaControl( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::beans::XPropertySet >& xProps,
                    const css::uno::Reference< css::drawing::XControlShape > xControlShape );
    // XControl
    virtual sal_Bool SAL_CALL getEnabled() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setEnabled( sal_Bool _enabled ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getVisible() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setVisible( sal_Bool _visible ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getSize() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSize( const css::uno::Any& _size ) throw (css::uno::RuntimeException);
    //virtual ~ScVbaControl();
};


class ScVbaControlFactory
{
public:
    ScVbaControlFactory( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
    ScVbaControlFactory( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::beans::XPropertySet >& xProps,
                    const css::uno::Reference< css::drawing::XControlShape > xControlShape );
    ScVbaControl* createControl( const sal_Int16 nClassID )  throw ( css::uno::RuntimeException );
    ScVbaControl* createControl()  throw ( css::uno::RuntimeException );
private:
    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::beans::XPropertySet > m_xProps;
    css::uno::Reference< css::drawing::XControlShape > m_xControlShape;
};

#endif//SC_VBA_CONTROL_HXX
