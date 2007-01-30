#ifndef SC_VBA_LISTBOX_HXX
#define SC_VBA_LISTBOX_HXX
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include <org/openoffice/vba/XListBox.hpp>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/beans/PropertyAttribute.hpp>

#include "vbacontrol.hxx"
#include "vbapropvalue.hxx"
#include "vbahelper.hxx"

typedef cppu::ImplInheritanceHelper2<ScVbaControl, oo::vba::XListBox, css::script::XDefaultProperty > ListBoxImpl_BASE;
// for the moment its preferable for default value behaviour to
// be provided by both old an new mechanisms
class ScVbaListBox : public ListBoxImpl_BASE
    ,public PropListener
    ,public ::comphelper::OPropertyContainer
{		

	//css::uno::Reference< css::uno::XComponentContext > m_xContext;
	//css::uno::Reference< css::beans::XPropertySet > m_xProps;
	rtl::OUString sSourceName; 
	rtl::OUString msDftPropName;

    sal_Int32 m_nIndex;

	void setDfltPropHandler();
	
public:
	ScVbaListBox( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
    ScVbaListBox( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::beans::XPropertySet >& xProps,
                    const css::uno::Reference< css::drawing::XControlShape> xControlShape );


	// Attributes
	virtual css::uno::Any SAL_CALL getValue() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setValue( const css::uno::Any& _value ) throw (css::uno::RuntimeException);
	virtual rtl::OUString SAL_CALL getText() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setText( const ::rtl::OUString& _text ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getMultiSelect() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMultiSelect( sal_Bool _multiselect ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Selected( ::sal_Int32 index ) throw (css::uno::RuntimeException);

	// Methods
	virtual void SAL_CALL AddItem( const css::uno::Any& pvargItem, const css::uno::Any& pvargIndex ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Clear(  ) throw (css::uno::RuntimeException);

	// XPropertySet

	virtual css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (css::uno::RuntimeException);  

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()

	// XDefaultProperty
    rtl::OUString SAL_CALL getDefaultPropertyName(  ) throw (css::uno::RuntimeException) { return ::rtl::OUString::createFromAscii("Value"); }
    
    //PropListener
    virtual void setValueEvent( const css::uno::Any& value );
    virtual css::uno::Any getValueEvent();


protected:
	// OPropertySetHelper
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

	virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;

};

#endif //
