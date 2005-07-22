#ifndef SC_VBA_WORKSHEET_HXX
#define SC_VBA_WORKSHEET_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <org/openoffice/vba/XWorksheet.hpp>
#include <org/openoffice/vba/XRange.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include "vbahelper.hxx"

class ScVbaWorksheet : public ::cppu::WeakImplHelper1<vba::XWorksheet> 
{
	uno::Reference< sheet::XSpreadsheet > mxSheet;
	uno::Reference< frame::XModel > mxModel;
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;

public:
	ScVbaWorksheet( 
		uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext,
		uno::Reference< sheet::XSpreadsheet > xSheet, 
		uno::Reference< frame::XModel >xModel ) : 
			m_xContext(xContext), mxSheet( xSheet ), mxModel(xModel)
	{}

	virtual ~ScVbaWorksheet() {}


    // Attributes
	virtual ::rtl::OUString SAL_CALL getName() throw (uno::RuntimeException);
	virtual void SAL_CALL setName( const ::rtl::OUString &rName ) throw (uno::RuntimeException);
	virtual sal_Bool getVisible() throw (uno::RuntimeException);
	virtual void setVisible( sal_Bool bVisible ) throw (uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getStandardWidth() throw (uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getStandardHeight() throw (uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL getProtectionMode() throw (uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL getProtectContents() throw (uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL getProtectDrawingObjects() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL getUsedRange() throw (uno::RuntimeException) ;

    // Methods
	virtual void SAL_CALL Activate() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL Range( const ::uno::Any &rRange ) throw (uno::RuntimeException);
	virtual void SAL_CALL Move( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException) ;
 	virtual void SAL_CALL Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException);
	virtual void SAL_CALL Paste( const uno::Any& Destination, const uno::Any& Link ) throw (uno::RuntimeException);
	virtual void SAL_CALL Delete(  ) throw (uno::RuntimeException);
	virtual void SAL_CALL Protect( const uno::Any& Password, const uno::Any& DrawingObjects, const uno::Any& Contents, const uno::Any& Scenarios, const uno::Any& UserInterfaceOnly ) throw (uno::RuntimeException);
	virtual void SAL_CALL Unprotect( const uno::Any& Password ) throw (uno::RuntimeException);

	virtual void SAL_CALL Calculate(  ) throw (uno::RuntimeException);
	virtual void SAL_CALL CheckSpelling( const uno::Any& CustomDictionary,const uno::Any& IgnoreUppercase,const uno::Any& AlwaysSuggest, const uno::Any& SpellingLang ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKSHEET_HXX */

