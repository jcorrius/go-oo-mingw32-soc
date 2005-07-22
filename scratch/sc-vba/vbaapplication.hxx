#ifndef SC_VBA_APPLICATION_HXX
#define SC_VBA_APPLICATION_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XApplication.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XApplication > ScVbaApplication_BASE;

class ScVbaApplication : public ScVbaApplication_BASE
{
private:
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
	uno::Reference< ::com::sun::star::frame::XDesktop > m_xDesktop;
public:
	ScVbaApplication( uno::Reference< ::com::sun::star::uno::XComponentContext >& m_xContext );
	virtual ~ScVbaApplication();

    // XApplication
    virtual uno::Reference< vba::XRange > getSelection() throw (uno::RuntimeException);
    virtual uno::Reference< vba::XWorkbook > getActiveWorkbook() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XWorkbooks > getWorkbooks() throw (uno::RuntimeException);
};

#endif /* SC_VBA_APPLICATION_HXX */
