#ifndef SC_VBA_ROWS_HXX
#define SC_VBA_ROWS_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/table/XTableRows.hpp>
#include <org/openoffice/vba/XRows.hpp>
 
#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XRows > ScVbaRows_BASE;

class ScVbaRows : public ScVbaRows_BASE
{
	uno::Reference< table::XTableRows > mxRows;

public:
	ScVbaRows(uno::Reference< table::XTableRows > xRows ) : mxRows( xRows ) {}
	virtual ~ScVbaRows() {}

	// XRows
	virtual double SAL_CALL getCount() throw (uno::RuntimeException);

};

#endif /* SC_VBA_ROWS_HXX */
