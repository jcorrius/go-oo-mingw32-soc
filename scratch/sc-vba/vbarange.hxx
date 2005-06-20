#ifndef SC_VBA_RANGE_HXX
#define SC_VBA_RANGE_HXX

#include <org/openoffice/vba/XRange.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <org/openoffice/vba/XFont.hpp>
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
	virtual ::rtl::OUString SAL_CALL getFormula() throw (uno::RuntimeException);
	virtual void   SAL_CALL setFormula( const ::rtl::OUString &rFormula ) throw (uno::RuntimeException);
	virtual double SAL_CALL getCount() throw (uno::RuntimeException);
	virtual long   SAL_CALL getRow() throw (uno::RuntimeException);
	virtual long   SAL_CALL getColumn() throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getText() throw (uno::RuntimeException);
	virtual void   SAL_CALL setText( const ::rtl::OUString &rString ) throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFormulaArray() throw (uno::RuntimeException);
	virtual void   SAL_CALL setFormulaArray(const ::rtl::OUString &rFormula) throw (uno::RuntimeException);

	// Methods
	virtual void SAL_CALL Clear() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearComments() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearContents() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearFormats() throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL HasFormula() throw (uno::RuntimeException);
	virtual void SAL_CALL FillLeft() throw (uno::RuntimeException);
	virtual void SAL_CALL FillRight() throw (uno::RuntimeException);
	virtual void SAL_CALL FillUp() throw (uno::RuntimeException);
	virtual void SAL_CALL FillDown() throw (uno::RuntimeException);
	virtual	uno::Reference< vba::XRange > SAL_CALL Offset( const ::uno::Any &nRowOffset, const ::uno::Any &nColOffset )
														   throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL CurrentRegion() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL CurrentArray() throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL Characters( long nIndex, long nCount ) throw (uno::RuntimeException);
	virtual uno::Reference< vba::XFont > SAL_CALL Font() throw (uno::RuntimeException);
};

#endif /* SC_VBA_RANGE_HXX */

