#ifndef SC_VBA_WORKBOOKS_HXX
#define SC_VBA_WORKBOOKS_HXX

#include <cppuhelper/implbase2.hxx>

#include <org/openoffice/vba/XWorkbooks.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper2< org::openoffice::vba::XWorkbooks,
        com::sun::star::container::XEnumerationAccess > ScVbaWorkbooks_BASE;

class ScVbaWorkbooks : public ScVbaWorkbooks_BASE
{

public:
	ScVbaWorkbooks() {}
	virtual ~ScVbaWorkbooks() {}

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

	// XWorkbooks
    virtual uno::Any SAL_CALL Add( const uno::Any& Before, const uno::Any& After, const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException);
    virtual void SAL_CALL Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException);
    virtual void SAL_CALL Move( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException);
    virtual void SAL_CALL Delete(  ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKBOOK_HXX */
