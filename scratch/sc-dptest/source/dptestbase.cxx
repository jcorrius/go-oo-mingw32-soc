
#include "dptestbase.hxx"
#include "dpresulttester.hxx"
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
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/sheet/DataPilotFieldFilter.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#include <com/sun/star/sheet/DataPilotFieldReferenceItemType.hpp>
#include <com/sun/star/sheet/DataPilotFieldReferenceType.hpp>
#include <com/sun/star/sheet/DataPilotTableRegion.hpp>
#include <com/sun/star/sheet/DataPilotTablePositionData.hpp>
#include <com/sun/star/sheet/DataPilotTablePositionType.hpp>
#include <com/sun/star/sheet/DataPilotTableResultData.hpp>
#include <com/sun/star/sheet/DataResultFlags.hpp>
#include <com/sun/star/sheet/GeneralFunction.hpp>
#include <com/sun/star/sheet/DataResult.hpp>
#include <com/sun/star/sheet/XDataPilotDescriptor.hpp>
#include <com/sun/star/sheet/XDataPilotField.hpp>
#include <com/sun/star/sheet/XDataPilotFieldGrouping.hpp>
#include <com/sun/star/sheet/XDataPilotTable2.hpp>
#include <com/sun/star/sheet/XDataPilotTable.hpp>
#include <com/sun/star/sheet/XDataPilotTables.hpp>
#include <com/sun/star/sheet/XDataPilotTablesSupplier.hpp>
#include <com/sun/star/sheet/XSheetFilterDescriptor.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/CellContentType.hpp>

#include <stdio.h>
#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>

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
using ::boost::shared_ptr;

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

DPTestBase::DPTestBase(const Reference<XSpreadsheetDocument>& rSpDoc, const TestParam& param) :
    mxSpDoc(rSpDoc), maTestParam(param)
{
}

DPTestBase::~DPTestBase()
{
}

void DPTestBase::run()
{
    RuntimeData data;

    // Generate the source data.
    genSrcData(data);
    if (!mpSrcRange.get())
        return;

    // Create the destination sheet where the DataPilot output will be constructed.
    const OUString sheetName = ascii("DPTable");
    Reference<XSpreadsheets> xSheets = mxSpDoc->getSheets();
    xSheets->insertNewByName(sheetName, mpSrcRange->Sheet+1);

    data.OutputSheetRef.set(getSheetByName(mxSpDoc, sheetName));
    data.OutputSheetId = mpSrcRange->Sheet + 1;

    // Activate the DataPilot output sheet.
    Reference<frame::XModel> xModel(mxSpDoc, UNO_QUERY_THROW);
    Reference<XSpreadsheetView> xSpView(xModel->getCurrentController(), UNO_QUERY_THROW);
    xSpView->setActiveSheet(data.OutputSheetRef);

    genDPTable(*mpSrcRange, data);
//  dumpTableProperties(data.OutputSheetRef);
//  fprintf(stdout, "----------------------------------------------------------------------\n");
//  fprintf(stdout, "checking results for normal display mode...\n");
//  verifyTableResults(data);
//  sleep(1);

    // ----------------------------------------------------------------------
    // Verify results with reference item.

    static const sal_Int32 refTypeList[] = {
//      DataPilotFieldReferenceType::ITEM_DIFFERENCE,
//      DataPilotFieldReferenceType::ITEM_PERCENTAGE,
//      DataPilotFieldReferenceType::ITEM_PERCENTAGE_DIFFERENCE,
//      DataPilotFieldReferenceType::RUNNING_TOTAL,
        DataPilotFieldReferenceType::ROW_PERCENTAGE,
//      DataPilotFieldReferenceType::COLUMN_PERCENTAGE,
//      DataPilotFieldReferenceType::TOTAL_PERCENTAGE,
//      DataPilotFieldReferenceType::INDEX
    };
    static const sal_Int32 refTypeCount = sizeof(refTypeList) / sizeof(refTypeList[0]);

    sal_Int32 fieldCount = data.FieldItemCounts.size();
    for (sal_Int32 refTypeId = 0; refTypeId < refTypeCount; ++refTypeId)
    {
        fprintf(stdout, "----------------------------------------------------------------------\n");
        fprintf(stdout, "checking results for referenced item mode (%s)...\n", 
                getReferenceTypeName(refTypeList[refTypeId]).c_str());
        for (sal_Int32 fieldId = 0; fieldId < fieldCount; ++fieldId)
        {
            if (data.FieldOrientations.at(fieldId) == DataPilotFieldOrientation_PAGE)
            {
                fprintf(stdout, "Info: skipping page field.....\n");fflush(stdout);
                continue;
            }
    
            sal_Int32 itemCount = data.FieldItemCounts.at(fieldId);
            for (sal_Int32 itemId = 0; itemId < itemCount; ++itemId)
            {
                setReferenceToField(data, fieldId, itemId, refTypeList[refTypeId]);
                verifyTableResults(data);
//              sleep(1);
            }
        }
    }
    removeAllReferences(data);
}

const OUString DPTestBase::getFieldName(sal_Int16 fieldId) const
{
    if (fieldId < maTestParam.Fields.size())
        return maTestParam.Fields.at(fieldId).Name;

    OUStringBuffer buf;
    buf.appendAscii("Field");
    buf.append(static_cast<sal_Int32>(fieldId+1));
    OUString fldName = buf.makeStringAndClear();
    return fldName;
}

const OUString DPTestBase::getFieldItemName(sal_Int16 fieldId, sal_Int32 itemId) const
{
    if (fieldId < maTestParam.Fields.size())
    {
        const FieldParam& field = maTestParam.Fields.at(fieldId);
        if (itemId < field.ItemNames.size())
            return field.ItemNames.at(itemId);
    }

    OUString fldName = getFieldName(fieldId);
    OUStringBuffer buf(fldName);
    buf.appendAscii("-");
    buf.append(itemId);
    return buf.makeStringAndClear();
}

const sal_Int32 DPTestBase::getFieldItemCount(sal_Int16 fieldId) const
{
    if (fieldId < maTestParam.Fields.size())
        return maTestParam.Fields.at(fieldId).ItemNames.size();

    sal_Int32 itemCount = rand<sal_Int32>(
        maTestParam.FieldItemCountLower, maTestParam.FieldItemCountUpper);
}

const OUString DPTestBase::getDataFieldName(sal_Int16 fieldId) const
{
    if (fieldId < maTestParam.DataFields.size())
        return maTestParam.DataFields.at(fieldId).Name;

    OUStringBuffer buf(ascii("Value"));
    buf.append(static_cast<sal_Int32>(fieldId+1));
    return buf.makeStringAndClear();
}

const sal_Int32 DPTestBase::getDataFieldValueLower(sal_Int16 fieldId) const
{
    if (fieldId < maTestParam.DataFields.size())
        return maTestParam.DataFields.at(fieldId).ValueLower;

    return 1;
}

const sal_Int32 DPTestBase::getDataFieldValueUpper(sal_Int16 fieldId) const
{
    if (fieldId < maTestParam.DataFields.size())
        return maTestParam.DataFields.at(fieldId).ValueUpper;

    return 100;
}

void DPTestBase::genSrcData(RuntimeData& data)
{
    if (maTestParam.FieldCount < maTestParam.DataCount)
        return;

    Reference<XSpreadsheets> xSheets = mxSpDoc->getSheets();

    // Remove all but one sheet.
    Sequence<OUString> names = xSheets->getElementNames();
    sal_Int32 n = names.getLength();
    for (sal_Int32 i = 0; i < n-maTestParam.DataCount; ++i)
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
    mpSrcRange->StartColumn = maTestParam.StartCol;
    mpSrcRange->StartRow    = maTestParam.StartRow;
    mpSrcRange->EndColumn   = maTestParam.StartCol + maTestParam.FieldCount - 1;
    mpSrcRange->EndRow      = maTestParam.StartRow + maTestParam.RowCount;

    // Construct a random data table, and put it into the sheet.
    DataTable table;
    table.setTableSize(maTestParam.RowCount, maTestParam.FieldCount);
    vector<sal_Int32> fieldItemCounts;
    for (sal_Int16 field = 0; field < maTestParam.FieldCount-maTestParam.DataCount; ++field)
    {
        OUString fldName = getFieldName(field);
        table.setFieldName(field, fldName);

        sal_Int32 itemCount = getFieldItemCount(field);
        fieldItemCounts.push_back(itemCount);
        for (sal_Int32 row = 0; row < maTestParam.RowCount; ++row)
            table.setCell(row, field, getFieldItemName(field, rand<sal_Int32>(0, itemCount-1)));
    }

    // Value fields come last.
    for (sal_Int32 vfield = 0; vfield < maTestParam.DataCount; ++vfield)
    {
        sal_Int32 offset = maTestParam.DataCount - vfield;
        table.setFieldName(maTestParam.FieldCount-offset, getDataFieldName(vfield));
        const sal_Int32 lower = getDataFieldValueLower(vfield);
        const sal_Int32 upper = getDataFieldValueUpper(vfield);
        for (sal_Int32 row = 0; row < maTestParam.RowCount; ++row)
            table.setCell(row, maTestParam.FieldCount-offset, rand(lower, upper)/1.0);
    }

//  table.output();
    table.output(xSheet, maTestParam.StartRow, maTestParam.StartCol);
    data.CacheTable.swap(table);
    data.FieldItemCounts.swap(fieldItemCounts);
}

void DPTestBase::genDPTable(const CellRangeAddress& srcRange, 
                            RuntimeData& data)
{
    static const GeneralFunction funcTable[] = {
//      GeneralFunction_NONE,
//      GeneralFunction_AUTO,
        GeneralFunction_SUM,
        GeneralFunction_COUNT,
        GeneralFunction_AVERAGE,
        GeneralFunction_MAX,
        GeneralFunction_MIN,
        GeneralFunction_PRODUCT,
//      GeneralFunction_COUNTNUMS,
//      GeneralFunction_STDEV,
//      GeneralFunction_STDEVP,
//      GeneralFunction_VAR,
//      GeneralFunction_VARP
    };

    const Reference<XSpreadsheet>& xDestSheet = data.OutputSheetRef;

    static sal_Int32 funcTableSize = sizeof(funcTable)/sizeof(funcTable[0]);

    // Create a data pilot table.
    sal_Int32 fieldCount = srcRange.EndColumn - srcRange.StartColumn + 1;

    Reference<XDataPilotTablesSupplier> xDPTSupplier(xDestSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);
    Reference<XDataPilotDescriptor> xDPDesc(xDPTables->createDataPilotDescriptor(), UNO_QUERY_THROW);
    xDPDesc->setSourceRange(srcRange);

    // Define non-data fields.
    Reference<container::XIndexAccess> xIA = xDPDesc->getDataPilotFields();
    vector<DataPilotFieldOrientation> orients;
    for (sal_Int32 i = 0; i < fieldCount - maTestParam.DataCount; ++i)
    {
        Reference<XDataPilotField> xField(xIA->getByIndex(i), UNO_QUERY_THROW);
        printName(xField);
        Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
        DataPilotFieldOrientation orient;
        if (i == fieldCount - maTestParam.DataCount - 1)
            orient = com::sun::star::sheet::DataPilotFieldOrientation_PAGE;
        else if (i % 3)
            orient = com::sun::star::sheet::DataPilotFieldOrientation_ROW;
        else
            orient = com::sun::star::sheet::DataPilotFieldOrientation_COLUMN;

        xPS->setPropertyValue(ascii("Orientation"), makeAny(orient));
        orients.push_back(orient);
    }
    data.FieldOrientations.swap(orients);

    // Define data fields.
    for (sal_Int32 i = 0; i < maTestParam.DataCount; ++i)
    {
        sal_Int32 offset = maTestParam.DataCount - i;
        Reference<XDataPilotField> xField(xIA->getByIndex(fieldCount-offset), UNO_QUERY_THROW);
        printName(xField);
        Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
        xPS->setPropertyValue(ascii("Orientation"), makeAny(DataPilotFieldOrientation_DATA));
        xPS->setPropertyValue(ascii("Function"), makeAny(funcTable[i % funcTableSize]));
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
            fprintf(stdout, "DPTestBase::dumpDPProperties: RuntimeException!\n");fflush(stdout);
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
    const Reference<XSpreadsheet>& xSheet = data.OutputSheetRef;
    Reference<XDataPilotTablesSupplier> xDPTSupplier(xSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);

    Reference<container::XEnumerationAccess> xEA(xDPTables, UNO_QUERY_THROW);
    Reference<container::XEnumeration> xIter = xEA->createEnumeration();
    while (xIter->hasMoreElements())
    {
        try
        {
            Reference<XDataPilotTable2> xDPTab(xIter->nextElement(), UNO_QUERY_THROW);
            Reference<XDataPilotDescriptor> xDesc(xDPTab, UNO_QUERY_THROW);
            printf("* DataPilot Table (%s)\n",
                   OUStringToOString(xDesc->getName(), RTL_TEXTENCODING_UTF8).getStr());
            CellRangeAddress range = xDPTab->getOutputRangeByType(DataPilotTableRegion::RESULT);
            printf("  data range: sheet: %d;  range (%ld, %ld) - (%ld, %ld)\n",
               range.Sheet, range.StartRow, range.StartColumn,
               range.EndRow, range.EndColumn);
            ResultTester tester(data, xDPTab);
            sal_Int16 failCount = forEachCell(range, tester).getFailureCount();
            printf("  number of failures = %d\n", failCount);
        }
        catch (const RuntimeException&)
        {
            fprintf(stdout, "DPTestBase::verifyTableResults: runtime error occurred.\n");
            throw RuntimeException();
        }
    }
}

void DPTestBase::setReferenceToField(const RuntimeData& data, sal_Int32 fieldId, sal_Int32 fieldItemId, sal_Int32 refType)
{
    const Reference<XSpreadsheet>& xSheet = data.OutputSheetRef;
    Reference<XDataPilotTablesSupplier> xDPTSupplier(xSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);

    Reference<container::XEnumerationAccess> xEA(xDPTables, UNO_QUERY_THROW);
    Reference<container::XEnumeration> xIter = xEA->createEnumeration();
    while (xIter->hasMoreElements())
    {
        try
        {
            Reference<XDataPilotTable2> xDPTab(xIter->nextElement(), UNO_QUERY_THROW);
            Reference<XDataPilotDescriptor> xDesc(xDPTab, UNO_QUERY_THROW);
            Reference<XIndexAccess> xDataFields = xDesc->getDataFields();
            sal_Int32 fieldCount = xDataFields->getCount();
            if (!fieldCount)
                continue;

            for (sal_Int32 i = 0; i < fieldCount; ++i)
            {
                Reference<XDataPilotField> xField(xDataFields->getByIndex(i), UNO_QUERY_THROW);
                DataPilotFieldReference ref;
                ref.ReferenceField = getFieldName(fieldId);
                ref.ReferenceType = refType;
                ref.ReferenceItemName = getFieldItemName(fieldId, fieldItemId);
                ref.ReferenceItemType = DataPilotFieldReferenceItemType::NAMED;
                Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
                xPS->setPropertyValue(ascii("Reference"), makeAny(ref));
            }
        }
        catch (const RuntimeException&)
        {
            fprintf(stdout, "DPTestBase::setReferenceToField: runtime error occurred.\n");
        }
    }
}

void DPTestBase::removeAllReferences(const RuntimeData& data)
{
    const Reference<XSpreadsheet>& xSheet = data.OutputSheetRef;
    Reference<XDataPilotTablesSupplier> xDPTSupplier(xSheet, UNO_QUERY_THROW);
    Reference<XDataPilotTables> xDPTables(xDPTSupplier->getDataPilotTables(), UNO_QUERY_THROW);

    Reference<container::XEnumerationAccess> xEA(xDPTables, UNO_QUERY_THROW);
    Reference<container::XEnumeration> xIter = xEA->createEnumeration();
    while (xIter->hasMoreElements())
    {
        try
        {
            Reference<XDataPilotTable2> xDPTab(xIter->nextElement(), UNO_QUERY_THROW);
            Reference<XDataPilotDescriptor> xDesc(xDPTab, UNO_QUERY_THROW);
            Reference<XIndexAccess> xDataFields = xDesc->getDataFields();
            sal_Int32 fieldCount = xDataFields->getCount();
            if (!fieldCount)
                continue;

            for (sal_Int32 i = 0; i < fieldCount; ++i)
            {
                Reference<XDataPilotField> xField(xDataFields->getByIndex(i), UNO_QUERY_THROW);
                DataPilotFieldReference ref;
                ref.ReferenceType = DataPilotFieldReferenceType::NONE;
                Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
                xPS->setPropertyValue(ascii("Reference"), makeAny(ref));
            }
        }
        catch (const RuntimeException&)
        {
            fprintf(stdout, "DPTestBase::removeAllReferences: runtime error occurred.\n");
        }
    }
}

void DPTestBase::groupRowFields(const Reference<XDataPilotTable2>& xDPTab, sal_Int32 groupSize) const
{
    Reference<XDataPilotDescriptor> xDesc(xDPTab, UNO_QUERY_THROW);
    Reference<container::XIndexAccess> xRowFields = xDesc->getRowFields();
    sal_Int32 fieldCount = xRowFields->getCount();
    if (!fieldCount)
        // No field exists !?
        return;

    Reference<XDataPilotField> xField(xRowFields->getByIndex(0), UNO_QUERY_THROW);
    Reference<XDataPilotFieldGrouping> xGrp(xField, UNO_QUERY_THROW);
    Reference<XIndexAccess> xItems = xField->getItems();
    sal_Int32 itemCount = xItems->getCount();
    if (itemCount < 2)
        return;

    Sequence<OUString> names(groupSize);
    for (sal_Int32 i = 0; i < itemCount; ++i)
    {
        Reference<container::XNamed> xItem(xItems->getByIndex(i), UNO_QUERY_THROW);
        names[i%groupSize] = xItem->getName();
        fprintf(stdout, "DPTestBase::foo: item = '%s'", 
                OUStringToOString(names[i%groupSize], RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);

        if (i > 0 && (i % groupSize == 0))
            xGrp->createNameGroup(names);

        Reference<beans::XPropertySet> xPS(xItem, UNO_QUERY_THROW);
        bool bShowDetail = true, bIsHidden = true;
        getPropertyValue(xPS, ascii("ShowDetail"),  bShowDetail);
        getPropertyValue(xPS, ascii("IsHidden"),    bIsHidden);
        fprintf(stdout, " show detail (%d)  is hidden (%d)\n", bShowDetail, bIsHidden);
    }
}

}
