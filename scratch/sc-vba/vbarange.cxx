#include <comphelper/processfactory.hxx>

#include <sfx2/objsh.hxx>

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
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/sheet/XFunctionAccess.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/table/XCellCursor.hpp>

#include "vbarange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

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

::rtl::OUString
ScVbaRange::Characters(long nIndex, long nCount) throw (uno::RuntimeException)
{
	::rtl::OUString rString;
	uno::Reference< text::XTextRange > xTextRange(mxRange, ::uno::UNO_QUERY);
	rString = xTextRange->getString();
	return rString.copy( --nIndex, nCount ); // Zero value indexing
}
    
::rtl::OUString
ScVbaRange::Address() throw (uno::RuntimeException)
{
	::rtl::OUString aStart, aEnd;
	uno::Sequence< uno::Any > aAddrArray1, aAddrArray2;	
	uno::Reference< lang::XMultiServiceFactory > xSMgr = ::comphelper::getProcessServiceFactory();
	uno::Reference< sheet::XFunctionAccess > xFunctionAccess( 
											 xSMgr->createInstance(::rtl::OUString::createFromAscii(
											 "com.sun.star.sheet.FunctionAccess")), ::uno::UNO_QUERY);
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable( mxRange, ::uno::UNO_QUERY );
	if( aAddrArray1.getLength() == 0 )
	{
		aAddrArray1.realloc(2);
		uno::Any* aArray = aAddrArray1.getArray();
		aArray[0] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().StartRow + 1 );
		aArray[1] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().StartColumn + 1 );
	}
	uno::Any aString1 = xFunctionAccess->callFunction(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADDRESS")), aAddrArray1);
	aString1 >>= aStart;

	if( xCellRangeAddressable->getRangeAddress().StartColumn == xCellRangeAddressable->getRangeAddress().EndColumn &&
        xCellRangeAddressable->getRangeAddress().StartRow == xCellRangeAddressable->getRangeAddress().EndRow )
		return aStart;

	String aString(aStart);
	aStart = rtl::OUString(aString.Append((sal_Unicode)':'));
	if( aAddrArray2.getLength() == 0 )
    {
		aAddrArray2.realloc(2);
		uno::Any* aArray = aAddrArray2.getArray();
		aArray[0] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().EndRow + 1 );
		aArray[1] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().EndColumn + 1 );
    }
    uno::Any aString2 = xFunctionAccess->callFunction(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADDRESS")), aAddrArray2);
	aString2 >>= aEnd;
	return aStart.concat( aEnd );
}

uno::Reference< vba::XRange >
ScVbaRange::Cells( const uno::Any &nRowIndex, const uno::Any &nColumnIndex ) throw(uno::RuntimeException)
{
	long nRow = 0, nCol = 0;
	sal_Bool bIsIndex =	nRowIndex >>= nRow, bIsColumnIndex = nColumnIndex >>= nCol;
	if( !bIsIndex && !bIsColumnIndex ) // .Cells
		return uno::Reference< vba::XRange >( new ScVbaRange( mxRange ) );
	if( bIsIndex && !bIsColumnIndex ) // .Cells(n)
	{
		int nIndex = nRow;
	    uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, ::uno::UNO_QUERY);
		int nColCount = xColumnRowRange->getColumns()->getCount();
        nRow = nIndex / nColCount;
		nCol = nIndex % nColCount;
	}
	return uno::Reference< vba::XRange >( new ScVbaRange( mxRange->getCellRangeByPosition( nCol, nRow, nCol, nRow ) ) );
}

void
ScVbaRange::Select() throw (uno::RuntimeException)
{
	sal_Bool bSelect;
	SfxObjectShell* pDoc = SfxObjectShell::Current();
    uno::Reference< frame::XModel > xModel;
                                                                                                                             
    if( pDoc )
        xModel = pDoc->GetModel();

	uno::Reference< view::XSelectionSupplier > xSelection( xModel->getCurrentController(), uno::UNO_QUERY);
	bSelect = xSelection->select( ( uno::Any )mxRange );
}

	
