#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/sheet/XSheetOperation.hpp>
#include <com/sun/star/sheet/CellFlags.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/sheet/XCellAddressable.hpp>
#include <com/sun/star/table/CellContentType.hpp>
#include <com/sun/star/sheet/XCellSeries.hpp>
#include <com/sun/star/sheet/FillDateMode.hpp>
#include <com/sun/star/sheet/FillMode.hpp>
#include <com/sun/star/sheet/FillDirection.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSheetCellCursor.hpp>
#include <com/sun/star/sheet/XArrayFormulaRange.hpp>

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
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	//FIXME: add all flags here?
	xSheetOperation->clearContents(sheet::CellFlags::VALUE );
}

void
ScVbaRange::ClearComments() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	//FIXME: STRING is not the correct type; May require special handling; clearNotes?
	xSheetOperation->clearContents(sheet::CellFlags::STRING);
}

void
ScVbaRange::ClearContents() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY);
	xSheetOperation->clearContents(sheet::CellFlags::VALUE | sheet::CellFlags::STRING );
}

void
ScVbaRange::ClearFormats() throw (uno::RuntimeException)
{
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

void 
ScVbaRange::FillLeft() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellSeries > xCellSeries(mxRange, ::uno::UNO_QUERY);
	xCellSeries->fillSeries(sheet::FillDirection_TO_LEFT, 
							sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

void 
ScVbaRange::FillRight() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellSeries > xCellSeries(mxRange, ::uno::UNO_QUERY);
	xCellSeries->fillSeries(sheet::FillDirection_TO_RIGHT, 
							sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

void 
ScVbaRange::FillUp() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellSeries > xCellSeries(mxRange, ::uno::UNO_QUERY);
    xCellSeries->fillSeries(sheet::FillDirection_TO_TOP, 
							sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

void 
ScVbaRange::FillDown() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellSeries > xCellSeries(mxRange, ::uno::UNO_QUERY);
    xCellSeries->fillSeries(sheet::FillDirection_TO_BOTTOM, 
							sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

::rtl::OUString
ScVbaRange::getText() throw (uno::RuntimeException)
{
	uno::Reference< text::XTextRange > xTextRange(mxRange, ::uno::UNO_QUERY);
	return xTextRange->getString();
}

void 
ScVbaRange::setText( const ::rtl::OUString &rString ) throw (uno::RuntimeException)
{
	uno::Reference< text::XTextRange > xTextRange(mxRange, ::uno::UNO_QUERY);
	xTextRange->setString( rString );
}

uno::Reference< vba::XRange >
ScVbaRange::Offset( const ::uno::Any &nRowOff, const uno::Any &nColOff ) throw (uno::RuntimeException)
{
	sal_Int16 nRowOffset = 0, nColOffset = 0;
	nRowOff >>= nRowOffset;
	nColOff >>= nColOffset;
	uno::Reference< sheet::XSheetCellRange > xSheetCellRange(mxRange, ::uno::UNO_QUERY);
	uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(mxRange, ::uno::UNO_QUERY);
	uno::Reference< table::XCellRange > xRange(xSheet, uno::UNO_QUERY);
	return uno::Reference< vba::XRange >( new ScVbaRange( xRange->getCellRangeByPosition(
										xCellRangeAddressable->getRangeAddress().StartColumn + nColOffset,
                                        xCellRangeAddressable->getRangeAddress().StartRow + nRowOffset,
                                        xCellRangeAddressable->getRangeAddress().EndColumn + nColOffset,
                                        xCellRangeAddressable->getRangeAddress().EndRow + nRowOffset ) ) );
}

uno::Reference< vba::XRange >
ScVbaRange::CurrentRegion() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetCellRange > xSheetCellRange(mxRange, ::uno::UNO_QUERY);
	uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
	uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = xSheet->createCursorByRange( xSheetCellRange );
	xSheetCellCursor->collapseToCurrentRegion();
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xSheetCellCursor, ::uno::UNO_QUERY);
	uno::Reference< table::XCellRange > xRange( xSheet, ::uno::UNO_QUERY);
	return uno::Reference< vba::XRange >( new ScVbaRange( xRange->getCellRangeByPosition(
                                        xCellRangeAddressable->getRangeAddress().StartColumn,
                                        xCellRangeAddressable->getRangeAddress().StartRow,
                                        xCellRangeAddressable->getRangeAddress().EndColumn,
                                        xCellRangeAddressable->getRangeAddress().EndRow ) ) );
}

uno::Reference< vba::XRange >
ScVbaRange::CurrentArray() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetCellRange > xSheetCellRange(mxRange, ::uno::UNO_QUERY);
    uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
    uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = xSheet->createCursorByRange( xSheetCellRange );
	xSheetCellCursor->collapseToCurrentArray();
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xSheetCellCursor, ::uno::UNO_QUERY);
    uno::Reference< table::XCellRange > xRange( xSheet, ::uno::UNO_QUERY);
    return uno::Reference< vba::XRange >( new ScVbaRange( xRange->getCellRangeByPosition(
                                        xCellRangeAddressable->getRangeAddress().StartColumn,
                                        xCellRangeAddressable->getRangeAddress().StartRow,
                                        xCellRangeAddressable->getRangeAddress().EndColumn,
                                        xCellRangeAddressable->getRangeAddress().EndRow ) ) );
}

::rtl::OUString
ScVbaRange::getFormulaArray() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XArrayFormulaRange > xArrayFormulaRange(mxRange, ::uno::UNO_QUERY);
	return xArrayFormulaRange->getArrayFormula();
}

void 
ScVbaRange::setFormulaArray(const ::rtl::OUString &rFormula) throw (uno::RuntimeException)
{
	uno::Reference< sheet::XArrayFormulaRange > xArrayFormulaRange(mxRange, ::uno::UNO_QUERY);
	xArrayFormulaRange->setArrayFormula( rFormula );
}
