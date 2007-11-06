
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
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/sheet/DataPilotTableRegion.hpp>
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
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/CellContentType.hpp>

#include <stdio.h>
#include <cmath>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sheet;
using ::com::sun::star::container::XIndexAccess;
using ::com::sun::star::beans::Property;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::table::CellRangeAddress;
using ::com::sun::star::table::XCell;
using ::com::sun::star::table::CellContentType;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;

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

    IntType range = maxValue - minValue;
    double f = ::std::rand() / (RAND_MAX*1.0) * range;
    return static_cast<IntType>(::lround(f)) + minValue;
}

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
    param.FieldCount = 4;
    param.RowCount   = 10000;
    param.DataCount  = 1;
    param.StartCol   = 0;
    param.StartRow   = 0;
    param.FieldItemCountLower = 50;
    param.FieldItemCountUpper = 55;

    genSrcData(param);
    if (!mpSrcRange.get())
        return;

    Reference<XSpreadsheets> xSheets = mxSpDoc->getSheets();
    xSheets->insertNewByName(ascii("DPTable"), mpSrcRange->Sheet+1);
    Reference<XSpreadsheet> xSheet = getSheetByName(mxSpDoc, ascii("DPTable"));
    genDPTable(param, *mpSrcRange, xSheet);
//  dumpDPProperties(xSheet);
//  verifyDPResults(xSheet);
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

    for (sal_Int32 i = 0; i < param.DataCount; ++i)
    {
        sal_Int32 offset = param.DataCount - i;
        Reference<XDataPilotField> xField(xIA->getByIndex(fieldCount-offset), UNO_QUERY_THROW);
        printName(xField);
        Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
        xPS->setPropertyValue(ascii("Orientation"), makeAny(DataPilotFieldOrientation_DATA));
    }

    table::CellAddress cell;
    cell.Sheet  = 1;
    cell.Column = 0;
    cell.Row    = 0;
    xDPTables->insertNewByName(ascii("MyDataPilot"), cell, xDPDesc);
}

void DPTestBase::dumpDPProperties(const Reference<XSpreadsheet>& xSheet)
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
            
            range = xDPTab->getOutputRangeByType(DataPilotTableRegion::DATA);
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

void DPTestBase::verifyDPResults(const Reference<XSpreadsheet>& xSheet)
{
    fprintf(stdout, "now verifying calculation results....\n");
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
    
            CellRangeAddress range = xDPTab->getOutputRangeByType(DataPilotTableRegion::DATA);
            printf("  data range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
               range.Sheet, range.StartRow, range.StartColumn,
               range.EndRow, range.EndColumn);

            for (sal_Int32 nRow = range.StartRow; nRow <= range.EndRow; ++nRow)
            {
                for (sal_Int32 nCol = range.StartColumn; nCol <= range.EndColumn; ++nCol)
                {
                    Reference<XCell> xCell = xSheet->getCellByPosition(nCol, nRow);
                    if (xCell->getType() != table::CellContentType_VALUE)
                    {
                        fprintf(stdout, "DPTestBase::verifyDPResults: Cell(%ld, %ld) not a value cell\n",
                                nRow, nCol);
                        continue;
                    }

                    double val = xCell->getValue();
                    printf("  (%ld, %ld) = %g\n", nRow, nCol, val);
                }
            }
        }
        catch (const RuntimeException&)
        {
            fprintf(stdout, "DPTestBase::verifyDPResults: runtime error occurred.\n");
        }
    }
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
