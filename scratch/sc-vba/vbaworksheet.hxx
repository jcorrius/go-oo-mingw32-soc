#ifndef SC_VBA_WORKSHEET_HXX
#define SC_VBA_WORKSHEET_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <org/openoffice/vba/XWorksheet.hpp>
#include <org/openoffice/vba/XRange.hpp>
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XWorksheet > ScVbaWorksheet_BASE;

class ScVbaWorksheet : public ScVbaWorksheet_BASE
{
	uno::Reference< sheet::XSpreadsheet > mxSheet;

public:
	ScVbaWorksheet( uno::Reference< sheet::XSpreadsheet > xSheet ) :
 			mxSheet( xSheet ){}
	virtual ~ScVbaWorksheet() {}

    // Attributes
	virtual ::rtl::OUString SAL_CALL getName() throw (uno::RuntimeException);
	virtual void SAL_CALL setName( const ::rtl::OUString &rName ) throw (uno::RuntimeException);
	virtual sal_Bool getVisible() throw (uno::RuntimeException);
	virtual void setVisible( sal_Bool bVisible ) throw (uno::RuntimeException);

    // Methods
	virtual void SAL_CALL Activate() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL Range( const ::uno::Any &rRange ) throw (uno::RuntimeException);
	virtual void SAL_CALL Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException);
	virtual void SAL_CALL Move( const ::uno::Any& Before, const ::uno::Any& After ) throw (uno::RuntimeException);

};

#endif /* SC_VBA_WORKSHEET_HXX */

