#ifndef SC_VBA_WORKSHEETS_HXX
#define SC_VBA_WORKSHEETS_HXX

#include <org/openoffice/vba/XWorksheets.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include "vbahelper.hxx"

class ScModelObj;

class ScVbaWorksheets : public org::openoffice::vba::XWorksheets,
					   public com::sun::star::container::XEnumerationAccess
{
	SC_VBA_UNO_HELPER_MEMBERS;
	uno::Reference< sheet::XSpreadsheets > mxSheets;

public:
	ScVbaWorksheets(uno::Reference< sheet::XSpreadsheets > xSheets ) :
		mxSheets( xSheets ), mnRefCount( 1 ) {}
	virtual ~ScVbaWorksheets() {}

	SC_VBA_UNO_HELPER_XINTERFACE;

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
