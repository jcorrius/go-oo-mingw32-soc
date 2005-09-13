#ifndef SC_VBA_WORKBOOK_HXX
#define SC_VBA_WORKBOOK_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/frame/XModel.hpp>
#include <org/openoffice/vba/XWorkbook.hpp>
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XWorkbook > ScVbaWorkbook_BASE;

class ScVbaWorkbook : public ScVbaWorkbook_BASE
{
	uno::Reference< ::uno::XComponentContext > m_xContext;
	uno::Reference< frame::XModel > mxModel;

public:
	ScVbaWorkbook( 	uno::Reference< ::uno::XComponentContext >& xContext,
			uno::Reference< frame::XModel > xModel ) :
 			mxModel( xModel ), m_xContext( xContext ){}
	virtual ~ScVbaWorkbook() {}

    // Attributes
	virtual ::rtl::OUString SAL_CALL getName() throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getPath() throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFullName() throw (uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL getProtectStructure() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XWorksheet > SAL_CALL getActiveSheet() throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getSaved() throw (uno::RuntimeException);
	virtual void SAL_CALL setSaved( sal_Bool bSave ) throw (uno::RuntimeException);

	// Methods
	virtual uno::Any Worksheets( const uno::Any& aIndex ) throw (uno::RuntimeException);
	virtual void SAL_CALL Close( const uno::Any &bSaveChanges,
								 const uno::Any &aFileName,
								 const uno::Any &bRouteWorkbook ) throw (uno::RuntimeException);
	virtual void SAL_CALL Protect( const uno::Any & aPassword ) throw (uno::RuntimeException);
	virtual void SAL_CALL Unprotect( const uno::Any &aPassword ) throw (uno::RuntimeException);
	virtual void SAL_CALL Save() throw (uno::RuntimeException);
	virtual void SAL_CALL Activate() throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKBOOK_HXX */
