#ifndef SC_VBA_APPLICATION_HXX
#define SC_VBA_APPLICATION_HXX

#include <org/openoffice/vba/XApplication.hpp>
#include "vbahelper.hxx"

class ScVbaApplication : public org::openoffice::vba::XApplication
{
	SC_VBA_UNO_HELPER_MEMBERS;

public:
	ScVbaApplication();
	virtual ~ScVbaApplication();

	// XInterface
	SC_VBA_UNO_HELPER_XINTERFACE;

    // XApplication
    virtual uno::Reference< vba::XRange > getSelection() throw (uno::RuntimeException);
    virtual uno::Reference< vba::XWorkbook > getActiveWorkbook() throw (uno::RuntimeException);
};

#endif /* SC_VBA_APPLICATION_HXX */
