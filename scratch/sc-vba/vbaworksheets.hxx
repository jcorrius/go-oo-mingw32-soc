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

typedef ::cppu::WeakImplHelper2< org::openoffice::vba::XWorksheets,
         com::sun::star::container::XEnumerationAccess > ScVbaWorksheets_BASE;

class ScVbaWorksheets : public ScVbaWorksheets_BASE
{
	uno::Reference< sheet::XSpreadsheets > mxSheets;
	uno::Reference<  ::com::sun::star::uno::XComponentContext > m_xContext;

public:
	ScVbaWorksheets(uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, uno::Reference< sheet::XSpreadsheets > xSheets ) :
		mxSheets( xSheets ), m_xContext( xContext ) {}
	virtual ~ScVbaWorksheets() {}

	// XEnumerationAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType() throw (::com::sun::star::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL hasElements() throw (::com::sun::star::uno::RuntimeException);
	virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration() throw (uno::RuntimeException);

	// XCollection
    virtual ::sal_Int32 SAL_CALL getCount() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL getParent() throw (uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getCreator() throw (uno::RuntimeException);
    virtual uno::Reference< ::org::openoffice::vba::XApplication > SAL_CALL getApplication() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL Item( ::sal_Int32 Index ) throw (uno::RuntimeException);

	// XWorksheets
    virtual uno::Any SAL_CALL Add( const uno::Any& Before, const uno::Any& After, const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException);
    virtual void SAL_CALL Delete(  ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKSHEETS_HXX */
