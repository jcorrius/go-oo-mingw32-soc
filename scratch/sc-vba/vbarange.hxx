#ifndef SC_VBA_RANGE_HXX
#define SC_VBA_RANGE_HXX

#include <org/openoffice/vba/XRange.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include "vbahelper.hxx"

class ScTableSheetsObj;

class ScVbaRange : public org::openoffice::vba::XRange
{
	SC_VBA_UNO_HELPER_MEMBERS;
	uno::Reference< table::XCellRange > mxRange;

public:
	ScVbaRange( uno::Reference< table::XCellRange > xRange ) :
 			mxRange( xRange ), mnRefCount( 1 ) {}
	virtual ~ScVbaRange() {}

	SC_VBA_UNO_HELPER_XINTERFACE;

    // Attributes
	virtual double SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
	virtual void   SAL_CALL setValue( double value ) throw (::com::sun::star::uno::RuntimeException);

	virtual void SAL_CALL Clear() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearComments() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearContents() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearFormats() throw (uno::RuntimeException);
};

#endif /* SC_VBA_RANGE_HXX */

