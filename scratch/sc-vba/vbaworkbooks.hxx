#ifndef SC_VBA_WORKBOOKS_HXX
#define SC_VBA_WORKBOOKS_HXX

#include <org/openoffice/vba/XWorkbooks.hpp>
#include "vbahelper.hxx"

class ScModelObj;

class ScVbaWorkbooks : public org::openoffice::vba::XWorkbooks
{
	SC_VBA_UNO_HELPER_MEMBERS;

public:
	ScVbaWorkbooks() :
		mnRefCount( 1 ) {}
	virtual ~ScVbaWorkbooks() {}

	SC_VBA_UNO_HELPER_XINTERFACE;

	// XCollection
    virtual ::sal_Int32 SAL_CALL getCount() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL getParent() throw (uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getCreator() throw (uno::RuntimeException);
    virtual uno::Reference< ::org::openoffice::vba::XApplication > SAL_CALL getApplication() throw (uno::RuntimeException);
    virtual uno::Any SAL_CALL Item( ::sal_Int32 Index ) throw (uno::RuntimeException);

	// XWorkbooks
    virtual uno::Any SAL_CALL Add( const uno::Any& Before, const uno::Any& After, const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException);
    virtual void SAL_CALL Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException);
    virtual void SAL_CALL Move( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException);
    virtual void SAL_CALL Delete(  ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKBOOK_HXX */
