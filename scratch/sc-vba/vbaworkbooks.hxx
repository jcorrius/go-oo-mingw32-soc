#ifndef SC_VBA_WORKBOOKS_HXX
#define SC_VBA_WORKBOOKS_HXX

#include <cppuhelper/implbase2.hxx>

#include <org/openoffice/vba/XGlobals.hpp>
#include <org/openoffice/vba/XWorkbooks.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper2< org::openoffice::vba::XWorkbooks,
        com::sun::star::container::XEnumerationAccess > ScVbaWorkbooks_BASE;

class ScVbaWorkbooks : public ScVbaWorkbooks_BASE
{
private:
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
	uno::Reference< org::openoffice::vba::XGlobals > getGlobals() throw (uno::RuntimeException);
	
public:
	ScVbaWorkbooks( uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext ): m_xContext( xContext ) {}
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
    virtual uno::Any SAL_CALL Item( const uno::Any& aIndex ) throw (uno::RuntimeException);

	// XWorkbooks
    virtual uno::Any SAL_CALL Add() throw (uno::RuntimeException);
    virtual void SAL_CALL Close(  ) throw (uno::RuntimeException);
	virtual uno::Any SAL_CALL Open(const uno::Any &aFileName) throw (uno::RuntimeException);
private:
        uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (uno::RuntimeException);

        uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKBOOKS_HXX */
