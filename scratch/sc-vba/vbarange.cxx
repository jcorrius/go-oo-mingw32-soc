#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/sheet/XSheetOperation.hpp>
#include <com/sun/star/sheet/CellFlags.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/sheet/XCellAddressable.hpp>
#include <com/sun/star/table/CellContentType.hpp>

#include "vbarange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XInterface implementation
uno::Any ScVbaRange::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XRange * >( this ) ),
			 static_cast< vba::XRange * >( this ) );
}

double
ScVbaRange::getValue() throw (::com::sun::star::uno::RuntimeException)
{
	uno::Reference< table::XCell > xCell = mxRange->getCellByPosition( 0, 0 );
	return xCell->getValue();
}

void
ScVbaRange::setValue( double value ) throw (::com::sun::star::uno::RuntimeException)
{
	uno::Reference< table::XCell > xCell = mxRange->getCellByPosition( 0, 0 );
	xCell->setValue( value );
}

void
ScVbaRange::Clear() throw (uno::RuntimeException)
{
	//FIXME: should get cell range by name ? and not by position 
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	//FIXME: add all flags here?
	xSheetOperation->clearContents(sheet::CellFlags::VALUE );
}

void
ScVbaRange::ClearComments() throw (uno::RuntimeException)
{
	//FIXME: should get cell range by name ? and not by position 
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	//FIXME: STRING is not the correct type; May require special handling; clearNotes?
	xSheetOperation->clearContents(sheet::CellFlags::STRING);
}

void
ScVbaRange::ClearContents() throw (uno::RuntimeException)
{
	//FIXME: should get cell range by name ? and not by position 
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	xSheetOperation->clearContents(sheet::CellFlags::VALUE | sheet::CellFlags::STRING );
}

void
ScVbaRange::ClearFormats() throw (uno::RuntimeException)
{
	//FIXME: should get cell range by name ? and not by position 
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	//FIXME: need to check if we need to combine sheet::CellFlags::FORMATTED
	xSheetOperation->clearContents(sheet::CellFlags::HARDATTR | sheet::CellFlags::FORMATTED | sheet::CellFlags::EDITATTR);
}

::rtl::OUString
ScVbaRange::getFormula() throw (::com::sun::star::uno::RuntimeException)
{
    uno::Reference< table::XCell > xCell = mxRange->getCellByPosition( 0, 0 );
    return xCell->getFormula();
}

void
ScVbaRange::setFormula(const ::rtl::OUString &rFormula ) throw (uno::RuntimeException)
{
	uno::Reference< table::XCell > xCell = mxRange->getCellByPosition( 0, 0 );
	xCell->setFormula( rFormula );
}

double 
ScVbaRange::getCount() throw (uno::RuntimeException)
{
	double rowCount, colCount;
	uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, ::uno::UNO_QUERY);
	rowCount = xColumnRowRange->getRows()->getCount();
	colCount = xColumnRowRange->getColumns()->getCount();
	return rowCount * colCount;
}

long 
ScVbaRange::getRow() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellAddressable > xCellAddressable(mxRange->getCellByPosition(0, 0), ::uno::UNO_QUERY);
	return xCellAddressable->getCellAddress().Row + 1; // Zero value indexing 
}	
		
long 
ScVbaRange::getColumn() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellAddressable > xCellAddressable(mxRange->getCellByPosition(0, 0), ::uno::UNO_QUERY);
	return xCellAddressable->getCellAddress().Column + 1; // Zero value indexing
}

sal_Bool
ScVbaRange::HasFormula() throw (uno::RuntimeException)
{
	uno::Reference< table::XCell > xCell = mxRange->getCellByPosition(0, 0);
	return( xCell->getType() == table::CellContentType_FORMULA );
}

