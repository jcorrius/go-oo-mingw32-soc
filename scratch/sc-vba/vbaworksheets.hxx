#ifndef SC_VBA_WORKSHEETS_HXX
#define SC_VBA_WORKSHEETS_HXX

#include <cppuhelper/implbase2.hxx>

#include <org/openoffice/vba/XWorksheets.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <org/openoffice/vba/XGlobals.hpp>
 
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper2< oo::vba::XWorksheets,
         css::container::XEnumerationAccess > ScVbaWorksheets_BASE;

class ScVbaWorksheets : public ScVbaWorksheets_BASE
{
	css::uno::Reference< css::sheet::XSpreadsheets > mxSheets;
	css::uno::Reference< css::frame::XModel > mxModel;
	css::uno::Reference<  css::uno::XComponentContext > m_xContext;

public:
	ScVbaWorksheets(css::uno::Reference< css::uno::XComponentContext > & xContext, css::uno::Reference< css::frame::XModel > xModel );
	virtual ~ScVbaWorksheets() {}

	// XEnumerationAccess
	virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL hasElements() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

	// XCollection
    virtual ::sal_Int32 SAL_CALL getCount() throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getParent() throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getCreator() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< oo::vba::XApplication > SAL_CALL getApplication() throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& Index ) throw (css::uno::RuntimeException);

	// XWorksheets
    virtual css::uno::Any SAL_CALL Add( const css::uno::Any& Before, const css::uno::Any& After, const css::uno::Any& Count, const css::uno::Any& Type ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL Delete(  ) throw (css::uno::RuntimeException);
private:
	css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);

	css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException);
};

#endif /* SC_VBA_WORKSHEETS_HXX */
