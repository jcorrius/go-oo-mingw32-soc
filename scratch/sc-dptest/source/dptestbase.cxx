
#include "dptestbase.hxx"
#include "global.hxx"
#include "dpcachetable.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "rtl/ustrbuf.hxx"

#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/sheet/DataPilotFieldFilter.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/sheet/DataPilotTableRegion.hpp>
#include <com/sun/star/sheet/DataPilotTablePositionData.hpp>
#include <com/sun/star/sheet/DataPilotTablePositionType.hpp>
#include <com/sun/star/sheet/DataPilotTableResultData.hpp>
#include <com/sun/star/sheet/GeneralFunction.hpp>
#include <com/sun/star/sheet/XDataPilotDescriptor.hpp>
#include <com/sun/star/sheet/XDataPilotField.hpp>
#include <com/sun/star/sheet/XDataPilotTable2.hpp>
#include <com/sun/star/sheet/XDataPilotTable.hpp>
#include <com/sun/star/sheet/XDataPilotTables.hpp>
#include <com/sun/star/sheet/XDataPilotTablesSupplier.hpp>
#include <com/sun/star/sheet/XSheetFilterDescriptor.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/CellContentType.hpp>

#include <stdio.h>
#include <cmath>
#include <vector>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sheet;

using ::com::sun::star::container::XIndexAccess;
using ::com::sun::star::beans::Property;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::table::CellAddress;
using ::com::sun::star::table::CellRangeAddress;
using ::com::sun::star::table::XCell;
using ::com::sun::star::table::CellContentType;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::std::vector;

namespace dptest {

// ============================================================================

template<typename Type>
void printName(const Type& obj)
{
    Reference< container::XNamed > xNamed(obj, UNO_QUERY);
    if (xNamed.is())
    {
        OUString name = xNamed->getName();
        printf("name = %s\n", OUStringToOString(name, RTL_TEXTENCODING_UTF8).getStr());
    }
}

template<typename IntType>
IntType rand(IntType minValue, IntType maxValue)
{
    if (maxValue < minValue)
        return static_cast<IntType>(0);

    if (minValue == maxValue)
        return minValue;

    IntType range = maxValue - minValue;
    double f = ::std::rand() / (RAND_MAX*1.0) * range;
    return static_cast<IntType>(::lround(f)) + minValue;
}

template<typename UnaryProc>
UnaryProc forEachCell(sal_Int32 tab, sal_Int32 row1, sal_Int32 col1, 
                      sal_Int32 row2, sal_Int32 col2, UnaryProc op)
{
    if (row1 > row2 || col1 > col2)
        return op;

    for (sal_Int32 row = row1; row <= row2; ++row)
    {
        for (sal_Int32 col = col1; col <= col2; ++col)
        {
            CellAddress cell;
            cell.Sheet = tab;
            cell.Column = col;
            cell.Row = row;
            op(cell);
        }
    }
    return op;
}

template<typename UnaryProc>
UnaryProc forEachCell(const CellRangeAddress& range, UnaryProc op)
{
    return forEachCell(range.Sheet, range.StartRow, range.StartColumn, range.EndRow, range.EndColumn, op);
}

// ============================================================================

class ResultTester
{
public:
    ResultTester(const Reference<XSpreadsheet>& xSheet, const Reference<XDataPilotTable2>& xDPTab) :
        mxSheet(xSheet), mxDPTab(xDPTab)
    {
        init();
    }

    ResultTester(const ResultTester& other) :
        mxSheet(other.mxSheet), mxDPTab(other.mxDPTab)
    {
        init();
    }

    void init()
    {
        Reference<XDataPilotDescriptor> xDPDesc(mxDPTab, UNO_QUERY_THROW);
        printf("  name: '%s' (tag: '%s')\n", OUStringToOString(xDPDesc->getName(), RTL_TEXTENCODING_UTF8).getStr(),
               OUStringToOString(xDPDesc->getTag(), RTL_TEXTENCODING_UTF8).getStr());

        Reference<container::XIndexAccess> xDataFields = xDPDesc->getDataFields();
        sal_Int32 fieldCount = xDataFields->getCount();
        maFuncs.reserve(fieldCount);
        for (sal_Int32 i = 0; i < fieldCount; ++i)
        {
            Reference<XDataPilotField> xField(xDataFields->getByIndex(i), UNO_QUERY_THROW);
            Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
            Any any = xPS->getPropertyValue(ascii("Function"));
            sheet::GeneralFunction func;
            any >>= func;
            maFuncs.push_back(func);
        }
    }

    void operator()(const CellAddress& cell)
    {
        using namespace ::com::sun::star::sheet;

        DataPilotTablePositionData posData = mxDPTab->getPositionData(cell);
        if (posData.PositionType != DataPilotTablePositionType::RESULT)
            return;

        DataPilotTableResultData resData;
        if (!(posData.PositionData >>= resData))
            return;

        Reference<XCell> xCell = mxSheet->getCellByPosition(cell.Column, cell.Row);

        if (xCell->getType() != table::CellContentType_VALUE)
            return;

        double val = xCell->getValue();
        fprintf(stdout, "ResultTester::(): (%ld, %ld) = %g\n", cell.Row, cell.Column, val);fflush(stdout);

        sal_Int32 nId = resData.DataFieldIndex;
        const Sequence<DataPilotFieldFilter>& filters = resData.FieldFilters;
        for (sal_Int32 i = 0; i < filters.getLength(); ++i)
        {
            fprintf(stdout, "ResultTester::(): field name = '%s'  match value = '%s'\n",
                    OUStringToOString(filters[i].FieldName, RTL_TEXTENCODING_UTF8).getStr(),
                    OUStringToOString(filters[i].MatchValue, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
        }

        const sheet::GeneralFunction func = maFuncs.at(nId);
        switch (func)
        {
            case GeneralFunction_NONE:
                fprintf(stdout, "ResultTester::(): NONE\n");fflush(stdout);
            break;
            case GeneralFunction_AUTO:
                fprintf(stdout, "ResultTester::(): AUTO\n");fflush(stdout);
            break;
            case GeneralFunction_SUM:
                fprintf(stdout, "ResultTester::(): SUM\n");fflush(stdout);
            break;
            case GeneralFunction_COUNT:
                fprintf(stdout, "ResultTester::(): COUNT\n");fflush(stdout);
            break;
            case GeneralFunction_AVERAGE:
                fprintf(stdout, "ResultTester::(): AVERAGE\n");fflush(stdout);
            break;
            case GeneralFunction_MAX:
                fprintf(stdout, "ResultTester::(): MAX\n");fflush(stdout);
            break;
            case GeneralFunction_MIN:
                fprintf(stdout, "ResultTester::(): MIN\n");fflush(stdout);
            break;
            case GeneralFunction_PRODUCT:
                fprintf(stdout, "ResultTester::(): PRODUCT\n");fflush(stdout);
            break;
            case GeneralFunction_COUNTNUMS:
                fprintf(stdout, "ResultTester::(): COUNTNUMS\n");fflush(stdout);
            break;
            case GeneralFunction_STDEV:
                fprintf(stdout, "ResultTester::(): STDEV\n");fflush(stdout);
            break;
            case GeneralFunction_STDEVP:
                fprintf(stdout, "ResultTester::(): STDEVP\n");fflush(stdout);
            break;
            case GeneralFunction_VAR:
                fprintf(stdout, "ResultTester::(): VAR\n");fflush(stdout);
            break;
            case GeneralFunction_VARP:
                fprintf(stdout, "ResultTester::(): VARP\n");fflush(stdout);
            break;
        }
    }

private:
    ResultTester(); // disabled
    Reference<XDataPilotTable2>     mxDPTab;
    Reference<XSpreadsheet>         mxSheet;
    vector<sheet::GeneralFunction>  maFuncs;
};

// ============================================================================

DPTestBase::DPTestBase(const Reference<XSpreadsheetDocument>& rSpDoc) :
    mxSpDoc(rSpDoc)
{
}

DPTestBase::~DPTestBase()
{
}

void DPTestBase::run()
{
    TestParam param;
    param.FieldCount = 6;
    param.RowCount   = 1000;
    param.DataCount  = 3;
    param.StartCol   = 0;
    param.StartRow   = 0;
    param.FieldItemCountLower = 3;
    param.FieldItemCountUpper = 3;

    genSrcData(param);
    if (!mpSrcRange.get())
        return;

    Reference<XSpreadsheets> xSheets = mxSpDoc->getSheets();
    xSheets->insertNewByName(ascii("DPTable"), mpSrcRange->Sheet+1);

    RuntimeData data;
    data.OutputSheetRef.set( getSheetByName(mxSpDoc, ascii("DPTable")) );
    data.OutputSheetId = mpSrcRange->Sheet + 1;

    genDPTable(param, *mpSrcRange, data.OutputSheetRef);
    dumpTableProperties(data.OutputSheetRef);
    verifyTableResults(data);
}

const OUString DPTestBase::getFieldName(sal_Int16 fieldId) const
{
    OUStringBuffer buf;
    buf.appendAscii("Field");
    buf.append(static_cast<sal_Int32>(fieldId+1));
    OUString fldName = buf.makeStringAndClear();
    return fldName;
}

const OUString DPTestBase::getFieldItemName(sal_Int16 fieldId, sal_Int32 itemId) const
{
    OUString fldName = getFieldName(fieldId);
    OUStringBuffer buf(fldName);
    buf.appendAscii("-");
    buf.append(itemId);
    return buf.makeStringAndClear();
}

void DPTestBase::genSrcData(const TestParam& param)
{
    if (param.FieldCount < param.DataCount)
        return;

    Reference<XSpreadsheets> xSheets = mxSpDoc->getSheets();

    // Remove all but one sheet.
    Sequence<OUString> names = xSheets->getElementNames();
    sal_Int32 n = names.getLength();
    for (sal_Int32 i = 0; i < n-param.DataCount; ++i)
        xSheets->removeByName(names[i]);

    // Rename the only sheet.
    Reference<XSpreadsheet> xSheet;
    {
        Reference<container::XIndexAccess> xIA(xSheets, UNO_QUERY_THROW);
        xSheet.set(xIA->getByIndex(0), UNO_QUERY_THROW);
        Reference<container::XNamed> xNamed(xSheet, UNO_QUERY_THROW);
        xNamed->setName(ascii("DPSrcData"));
    }

    mpSrcRange.reset(new CellRangeAddress);
    mpSrcRange->Sheet = 0;
    mpSrcRange->StartColumn = param.StartCol;
    mpSrcRange->StartRow    = param.StartRow;
    mpSrcRange->EndColumn   = param.StartCol + param.FieldCount - 1;
    mpSrcRange->EndRow      = param.StartRow + param.RowCount;

    // Construct a random data table, and put it into the sheet.
    DataTable table;
    table.setTableSize(param.RowCount, param.FieldCount);
    for (sal_Int16 field = 0; field < param.FieldCount-param.DataCount; ++field)
    {
        OUString fldName = getFieldName(field);
        table.setFieldName(field, fldName);

        sal_Int32 itemCount = rand<sal_Int32>(
            param.FieldItemCountLower, param.FieldItemCountUpper);

        for (sal_Int32 row = 0; row < param.RowCount; ++row)
            table.setCell(row, field, getFieldItemName(field, rand<sal_Int32>(0, itemCount)));
    }

    // Value fields come last.
    for (sal_Int32 vfield = 0; vfield < param.DataCount; ++vfield)
    {
        sal_Int32 offset = param.DataCount - vfield;
        OUStringBuffer buf(ascii("Value"));
        buf.append(vfield+1);
        table.setFieldName(param.FieldCount-offset, buf.makeStringAndClear());
        for (sal_Int32 row = 0; row < param.RowCount; ++row)
            table.setCell(row, param.FieldCount-offset, rand(0, 100));
    }

//  table.output();
    table.output(xSheet, param.StartRow, param.StartCol);
}

void DPTestBase::genDPTable(const TestParam& param, const CellRangeAddress& srcRange, 
                            const Reference<XSpreadsheet>& xDestSheet)
{
    // Create a data pilot table.
    sal_Int32 fieldCount = srcRange.EndColumn - srcRange.StartColumn + 1;

    Reference<XDataPilotTablesSupplier> xDPTSupplier(xDestSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);
    Reference<XDataPilotDescriptor> xDPDesc(xDPTables->createDataPilotDescriptor(), UNO_QUERY_THROW);
    xDPDesc->setSourceRange(srcRange);
    xDPDesc->setTag(ascii("MY CUSTOM TAG"));
    printf("tag is set to '%s'\n", OUStringToOString(xDPDesc->getTag(), RTL_TEXTENCODING_UTF8).getStr());

    // Define non-data fields.
    Reference<container::XIndexAccess> xIA = xDPDesc->getDataPilotFields();
    for (sal_Int32 i = 0; i < fieldCount - param.DataCount; ++i)
    {
        Reference<XDataPilotField> xField(xIA->getByIndex(i), UNO_QUERY_THROW);
        printName(xField);
        Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
        if (i == fieldCount - param.DataCount - 1)
            xPS->setPropertyValue(ascii("Orientation"), makeAny(DataPilotFieldOrientation_PAGE));
        else if (i % 3)
            xPS->setPropertyValue(ascii("Orientation"), makeAny(DataPilotFieldOrientation_ROW));
        else
            xPS->setPropertyValue(ascii("Orientation"), makeAny(DataPilotFieldOrientation_COLUMN));
    }

    // Define data fields.
    for (sal_Int32 i = 0; i < param.DataCount; ++i)
    {
        sal_Int32 offset = param.DataCount - i;
        Reference<XDataPilotField> xField(xIA->getByIndex(fieldCount-offset), UNO_QUERY_THROW);
        printName(xField);
        Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
        xPS->setPropertyValue(ascii("Orientation"), makeAny(DataPilotFieldOrientation_DATA));
        if (i % 5 == 0)
            xPS->setPropertyValue(ascii("Function"), makeAny(GeneralFunction_SUM));
        else if (i % 5 == 1)
            xPS->setPropertyValue(ascii("Function"), makeAny(GeneralFunction_COUNT));
        else if (i % 5 == 2)
            xPS->setPropertyValue(ascii("Function"), makeAny(GeneralFunction_AVERAGE));
        else if (i % 5 == 3)
            xPS->setPropertyValue(ascii("Function"), makeAny(GeneralFunction_MAX));
        else if (i % 5 == 4)
            xPS->setPropertyValue(ascii("Function"), makeAny(GeneralFunction_MIN));
    }

    table::CellAddress cell;
    cell.Sheet  = 1;
    cell.Column = 0;
    cell.Row    = 0;
    xDPTables->insertNewByName(ascii("MyDataPilot"), cell, xDPDesc);
}

void DPTestBase::dumpTableProperties(const Reference<XSpreadsheet>& xSheet)
{
    Reference<XDataPilotTablesSupplier> xDPTSupplier(xSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);

    Reference<container::XEnumerationAccess> xEA(xDPTables, UNO_QUERY_THROW);
    Reference<container::XEnumeration> xIter = xEA->createEnumeration();
    while (xIter->hasMoreElements())
    {
        printf("--------------------------------------------------------------------\n");
        try
        {
            Reference<XDataPilotTable2> xDPTab(xIter->nextElement(), UNO_QUERY_THROW);
            Reference<XDataPilotDescriptor> xDPDesc(xDPTab, UNO_QUERY_THROW);
            printf("  name: '%s' (tag: '%s')\n", OUStringToOString(xDPDesc->getName(), RTL_TEXTENCODING_UTF8).getStr(),
                   OUStringToOString(xDPDesc->getTag(), RTL_TEXTENCODING_UTF8).getStr());
    
            CellRangeAddress range = xDPTab->getOutputRangeByType(DataPilotTableRegion::WHOLE);
            printf("  output range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
               range.Sheet, range.StartRow, range.StartColumn,
               range.EndRow, range.EndColumn);
            
            range = xDPTab->getOutputRangeByType(DataPilotTableRegion::TABLE);
            printf("  table range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
               range.Sheet, range.StartRow, range.StartColumn,
               range.EndRow, range.EndColumn);
            
            range = xDPTab->getOutputRangeByType(DataPilotTableRegion::RESULT);
            printf("  data range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
               range.Sheet, range.StartRow, range.StartColumn,
               range.EndRow, range.EndColumn);

            range = xDPDesc->getSourceRange();
            printf("  source range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
                   range.Sheet, range.StartRow, range.StartColumn,
                   range.EndRow, range.EndColumn);
    
            // data pilot table properties
            Reference<XPropertySet> xProps(xDPDesc, UNO_QUERY_THROW);
            dumpAllPropertyNames(xProps);
            sal_Bool ignoreEmptyRows, repeatIfEmpty, columnGrand, rowGrand, showFilterButton, drillDownOnDoubleClick;
            getPropertyValue(xProps, ascii("IgnoreEmptyRows"), ignoreEmptyRows);
            getPropertyValue(xProps, ascii("RepeatIfEmpty"), repeatIfEmpty);
            getPropertyValue(xProps, ascii("ColumnGrand"), columnGrand);
            getPropertyValue(xProps, ascii("RowGrand"), rowGrand);
            getPropertyValue(xProps, ascii("ShowFilterButton"), showFilterButton);
            getPropertyValue(xProps, ascii("DrillDownOnDoubleClick"), drillDownOnDoubleClick);
    
            printf("  (IgnoreEmptyRows: %d;  RepeatIfEmpty: %d;  ColumnGrand: %d;  RowGrand: %d;  ShowFilterButton: %d)\n",
                   ignoreEmptyRows, repeatIfEmpty, columnGrand, rowGrand, showFilterButton, drillDownOnDoubleClick);

            continue;

            // fields and their properties
            printf("  column fields\n");
            dumpFields(xDPDesc->getColumnFields());
            printf("  row fields\n");
            dumpFields(xDPDesc->getRowFields());
            printf("  page fields\n");
            dumpFields(xDPDesc->getPageFields());
            printf("  data fields\n");
            dumpFields(xDPDesc->getDataFields());
            printf("  hidden fields\n");
            dumpFields(xDPDesc->getHiddenFields());    
        }
        catch (const RuntimeException&)
        {
            fprintf(stdout, "DPTestBase::dumpDPProperties: BOO!\n");fflush(stdout);
        }
    }
}

void DPTestBase::dumpFields(const Reference<XIndexAccess>& xFields) const
{
    sal_Int32 fieldCount = xFields->getCount();
    for (sal_Int32 i = 0; i < fieldCount; ++i)
    {
        Reference<XDataPilotField> xField(xFields->getByIndex(i), UNO_QUERY_THROW);
        Reference<XPropertySet> xProps(xField, UNO_QUERY_THROW);
//          dumpAllPropertyNames(xProps);
        Reference<container::XNamed> xNamed(xField, UNO_QUERY_THROW);
        printf("    field name: '%s'\n", OUStringToOString(xNamed->getName(), RTL_TEXTENCODING_UTF8).getStr());
        OUString selectedPage, usedHierarchy;
        sal_Bool useSelectedPage;
        getPropertyValue(xProps, ascii("SelectedPage"), selectedPage);
        getPropertyValue(xProps, ascii("UseSelectedPage"), useSelectedPage);
        getPropertyValue(xProps, ascii("UsedHierarchy"), usedHierarchy);
        printf("    (SelectedPage: '%s';  UseSelectedPage: %d;  UsedHierarchy: '%s')\n", 
               OUStringToOString(selectedPage, RTL_TEXTENCODING_UTF8).getStr(),
               useSelectedPage,
               OUStringToOString(usedHierarchy, RTL_TEXTENCODING_UTF8).getStr());

        Reference<XIndexAccess> xItems = xField->getItems();
        dumpItems(xItems);
    }
}

void DPTestBase::dumpItems(const Reference<XIndexAccess>& xItems) const
{
    sal_Int32 itemCount = xItems->getCount();
    for (sal_Int32 j = 0; j < itemCount; ++j)
    {
        Any item = xItems->getByIndex(j);
        Reference<container::XNamed> xNamed(item, UNO_QUERY_THROW);
        Reference<XPropertySet> xProps(item, UNO_QUERY_THROW);

        sal_Bool showDetail, isHidden;
        getPropertyValue(xProps, ascii("ShowDetail"), showDetail);
        getPropertyValue(xProps, ascii("IsHidden"), isHidden);
        printf("      item name: '%s' (ShowDetail: %d;  IsHidden: %d)\n", 
               OUStringToOString(xNamed->getName(), RTL_TEXTENCODING_UTF8).getStr(),
               showDetail?1:0, isHidden?1:0);
    }
}

void DPTestBase::verifyTableResults(const RuntimeData& data)
{
    fprintf(stdout, "now verifying calculation results....\n");
    const Reference<XSpreadsheet>& xSheet = data.OutputSheetRef;
    Reference<XDataPilotTablesSupplier> xDPTSupplier(xSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);

    Reference<container::XEnumerationAccess> xEA(xDPTables, UNO_QUERY_THROW);
    Reference<container::XEnumeration> xIter = xEA->createEnumeration();
    while (xIter->hasMoreElements())
    {
        printf("--------------------------------------------------------------------\n");
        try
        {
            Reference<XDataPilotTable2> xDPTab(xIter->nextElement(), UNO_QUERY_THROW);
            CellRangeAddress range = xDPTab->getOutputRangeByType(DataPilotTableRegion::RESULT);
            printf("  data range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
               range.Sheet, range.StartRow, range.StartColumn,
               range.EndRow, range.EndColumn);
            ResultTester tester(xSheet, xDPTab);
            forEachCell(range, tester);
        }
        catch (const RuntimeException&)
        {
            fprintf(stdout, "DPTestBase::verifyDPResults: runtime error occurred.\n");
        }
    }
}

void DPTestBase::verifyCellResult(const RuntimeData& data, const CellAddress& cell,
                                  const Reference<XDataPilotTable2>& xTable)
{
    Reference<XCell> xCell = data.OutputSheetRef->getCellByPosition(cell.Column, cell.Row);
    if (xCell->getType() != table::CellContentType_VALUE)
    {
        fprintf(stdout, "DPTestBase::verifyDPResults: Cell(%ld, %ld) not a value cell\n",
                cell.Row, cell.Column);
        return;
    }

    double val = xCell->getValue();
    printf("  (%ld, %ld) = %g\n", cell.Row, cell.Column, val);
    Sequence< Sequence<Any> > table = xTable->getDrillDownData(cell);

}

const Reference<XSpreadsheet> DPTestBase::getSrcSheet() const
{
    Reference<XSpreadsheets> xSheets = mxSpDoc->getSheets();

    // Remove all but one sheet.
    Reference<container::XIndexAccess> xIA(xSheets, UNO_QUERY_THROW);
    Reference<XSpreadsheet> xSheet(xIA->getByIndex(mpSrcRange->Sheet), UNO_QUERY_THROW);
    return xSheet;
}

}
