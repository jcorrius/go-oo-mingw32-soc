#ifndef SC_VBA_APPLICATION_HXX
#define SC_VBA_APPLICATION_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XWorksheetFunction.hpp>
#include <org/openoffice/vba/XApplication.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< oo::vba::XApplication > ScVbaApplication_BASE;

class ScVbaApplication : public ScVbaApplication_BASE
{
private:
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
public:
	ScVbaApplication( css::uno::Reference< css::uno::XComponentContext >& m_xContext );
	virtual ~ScVbaApplication();

    // XApplication
	virtual css::uno::Reference< oo::vba::XRange > getSelection() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XWorkbook > getActiveWorkbook() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > getActiveCell() throw ( css::uno::RuntimeException);
	virtual sal_Bool getScreenUpdating() throw (css::uno::RuntimeException);
	virtual void setScreenUpdating(sal_Bool bUpdate) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL Workbooks( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XWorksheetFunction > SAL_CALL WorksheetFunction( ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL Evaluate( const ::rtl::OUString& Name ) throw (css::uno::RuntimeException); 
	virtual css::uno::Any SAL_CALL Dialogs( const css::uno::Any& DialogIndex ) throw (css::uno::RuntimeException);

};
#endif /* SC_VBA_APPLICATION_HXX */
