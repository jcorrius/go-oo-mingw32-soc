#ifndef SC_VBA_COLUMNS_HXX
#define SC_VBA_COLUMNS_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/table/XTableColumns.hpp>
#include <org/openoffice/vba/XColumns.hpp>
 
#include "vbahelper.hxx"


typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XColumns > ScVbaColumns_BASE;

class ScVbaColumns : public ScVbaColumns_BASE
{
	uno::Reference< table::XTableColumns > mxColumns;

public:
	ScVbaColumns(uno::Reference< table::XTableColumns > xColumns ) : mxColumns( xColumns ) {}
	virtual ~ScVbaColumns() {}

	// XColumns
	virtual double SAL_CALL getCount() throw (uno::RuntimeException);

};

#endif /* SC_VBA_COLUMNS_HXX */
