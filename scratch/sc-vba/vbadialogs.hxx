#ifndef SC_VBA_DIALOGS_HXX
#define SC_VBA_DIALOGS_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <org/openoffice/vba/XDialogs.hpp>
#include <org/openoffice/vba/XCollection.hpp>
 
class ScModelObj;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XDialogs > ScVbaDialogs_BASE;

class ScVbaDialogs : public ScVbaDialogs_BASE
{
	uno::Reference< ::com::sun::star::uno::XComponentContext > & m_xContext;
public:
	ScVbaDialogs( uno::Reference< com::sun::star::uno::XComponentContext > &xContext ):
	m_xContext( xContext ) {} 
	virtual ~ScVbaDialogs() {}

	// XCollection
	virtual ::sal_Int32 SAL_CALL getCount() throw (uno::RuntimeException);
	virtual uno::Any SAL_CALL getParent() throw (uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getCreator() throw (uno::RuntimeException);
	virtual uno::Reference< ::org::openoffice::vba::XApplication > SAL_CALL getApplication() 
									throw (uno::RuntimeException);
	virtual uno::Any SAL_CALL Item( const uno::Any& Index ) throw (uno::RuntimeException);

	// XDialogs
	virtual void SAL_CALL Dummy() throw (uno::RuntimeException);
};

#endif /* SC_VBA_DIALOGS_HXX */
