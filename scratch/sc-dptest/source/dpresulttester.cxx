
#include "dpresulttester.hxx"

#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNamed.hpp>
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
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/CellContentType.hpp>

#include <stdio.h>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include <rtl/ustrbuf.hxx>

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
using ::std::set;
using ::boost::shared_ptr;

namespace dptest {

// ============================================================================

class RunningTotalCalculator
{
public:
    RunningTotalCalculator(const DataTable& dataTable, const vector<DataTable::Filter>& filters,
                           sal_Int32 dataFieldId, GeneralFunction func, sal_Int32 refFieldPos) :
        maCacheTable(dataTable), maFilters(filters), mnDataFieldId(dataFieldId),
        meFunc(func), mnRefFieldPos(refFieldPos), mfRunningTotal(0.0)
    {
    }

    /** 
     * 
     * 
     * @param itemNameId string ID of current item name for the reference 
     *                   field
     */
    void operator() (const sal_Int32 itemNameId)
    {
        vector<DataTable::Filter> filters = maFilters;
        filters.at(mnRefFieldPos).MatchStrId = itemNameId;
        mfRunningTotal += maCacheTable.aggregateValue(filters, mnDataFieldId, meFunc);
    }

    double getRunningTotal() const
    {
        return mfRunningTotal;
    }

private:
    RunningTotalCalculator(); // disabled

private:
    const DataTable& maCacheTable;
    const vector<DataTable::Filter>& maFilters;
    const sal_Int32 mnDataFieldId;
    const GeneralFunction meFunc;
    const sal_Int32 mnRefFieldPos;

    double mfRunningTotal;
};

// ============================================================================

class FilterFinder
{
public:
    FilterFinder(sal_Int32 fieldId) :
        mnFieldIndex(fieldId)
    {
    }

    bool operator() (const DataTable::Filter& filter)
    {
        return filter.FieldIndex == mnFieldIndex;
    }

private:
    sal_Int32 mnFieldIndex;
};

// ============================================================================

ResultTester::DataFieldSetting::DataFieldSetting() : 
    FieldRef(static_cast<DataPilotFieldReference*>(NULL))
{
}

// ============================================================================

ResultTester::ResultTester(const RuntimeData& data, const Reference<XDataPilotTable2>& xDPTab) :
    mxDPTab(xDPTab), maData(data), mnFailureCount(0)
{
    init();
}

ResultTester::ResultTester(const ResultTester& other) :
    mxDPTab(other.mxDPTab), 
    maData(other.maData), 
    maFieldItemMap(other.maFieldItemMap),
    maDataFieldSettings(other.maDataFieldSettings),
    maRowFieldIds(other.maRowFieldIds),
    maColFieldIds(other.maColFieldIds),
    mnFailureCount(other.mnFailureCount)
{
}

void ResultTester::init()
{
    Reference<XDataPilotDescriptor> xDPDesc(mxDPTab, UNO_QUERY_THROW);

    // Go though each data field and store a list of data field data.
    {
        Reference<container::XIndexAccess> xDataFields = xDPDesc->getDataFields();
        sal_Int32 fieldCount = xDataFields->getCount();
        maDataFieldSettings.reserve(fieldCount);
        for (sal_Int32 i = 0; i < fieldCount; ++i)
        {
            Reference<XDataPilotField> xField(xDataFields->getByIndex(i), UNO_QUERY_THROW);
            DataFieldSetting setting;
    
            // Get the field ID of a given data field.
            Reference<container::XNamed> xNamed(xField, UNO_QUERY_THROW);
            OUString name = xNamed->getName();
            setting.FieldId = maData.CacheTable.getFieldIndex(name);
    
            // Get the function used for aggregation.
            Reference<XPropertySet> xPS(xField, UNO_QUERY_THROW);
            getPropertyValue(xPS, ascii("Function"), setting.Function);
    
            // Get the referenced item information (if any).
            bool hasReference = false;
            getPropertyValue(xPS, ascii("HasReference"), hasReference);
            if (hasReference)
            {
                setting.FieldRef.reset(new DataPilotFieldReference);
                getPropertyValue(xPS, ascii("Reference"), *setting.FieldRef);
            }
            maDataFieldSettings.push_back(setting);
        }
    }

    // Store column row fields data.
    storeColRowFieldsData(true);
    storeColRowFieldsData(false);
}

void ResultTester::operator()(const CellAddress& cell)
{
    DataPilotTablePositionData posData = mxDPTab->getPositionData(cell);

    if (posData.PositionType != DataPilotTablePositionType::RESULT)
        // This cell doesn't belong to the result area.  Bail out.
        return;

    DataPilotTableResultData resData;
    if (!(posData.PositionData >>= resData))
        // For whatever reason unpacking of the result data failed.  Bail out.
        return;

    vector<DataTable::Filter> filters;
    sal_Int32 filterSize = resData.FieldFilters.getLength();
    filters.reserve(filterSize);
    for (sal_Int32 i = 0; i < filterSize; ++i)
    {
        sal_Int32 nFieldId = maData.CacheTable.getFieldIndex(resData.FieldFilters[i].FieldName);

        if (nFieldId >= 0)
        {
            DataTable::Filter filter;
            filter.FieldIndex = nFieldId;
            filter.MatchStrId = DataTable::getStringId(resData.FieldFilters[i].MatchValue);
            filters.push_back(filter);
        }
    }

    // ID only for the data field set, not the actual column ID.
    sal_Int32 nId = resData.DataFieldIndex; 
    const DataFieldSetting& setting = maDataFieldSettings.at(nId);

    if (setting.FieldRef.get())
    {
        // referenced item exists.
        switch (setting.FieldRef->ReferenceType)
        {
            case DataPilotFieldReferenceType::ITEM_DIFFERENCE:
            case DataPilotFieldReferenceType::ITEM_PERCENTAGE:
            case DataPilotFieldReferenceType::ITEM_PERCENTAGE_DIFFERENCE:
                verifyRefValue(cell, setting, filters, resData.Result);
            break;
            case DataPilotFieldReferenceType::RUNNING_TOTAL:
                verifyRunningTotal(cell, setting, filters, resData.Result);
            break;

            case DataPilotFieldReferenceType::ROW_PERCENTAGE:
            case DataPilotFieldReferenceType::COLUMN_PERCENTAGE:
            case DataPilotFieldReferenceType::TOTAL_PERCENTAGE:
                verifyPercentValue(cell, setting, filters, resData.Result);
            break;
            case DataPilotFieldReferenceType::INDEX:
            break;
        }
        return;
    }

    // normal display mode with no reference
    verifyNormal(cell, setting, filters, resData.Result);
}

sal_Int16 ResultTester::getFailureCount() const
{
    return mnFailureCount;
}

void ResultTester::verifyNormal(const CellAddress& cell, 
                                const DataFieldSetting& setting, 
                                const vector<DataTable::Filter>& filters,
                                const DataResult& result)
{
    Reference<XCell> xCell = maData.OutputSheetRef->getCellByPosition(cell.Column, cell.Row);

    switch (xCell->getType())
    {
        case table::CellContentType_VALUE:
        {
            double val1 = xCell->getValue();
            double val2 = maData.CacheTable.aggregateValue(filters, setting.FieldId, setting.Function);
            if (!compare(val1,val2))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %g  check value = %g (%s)\n", 
                        cell.Row, cell.Column,
                        val1, val2, getFunctionName(setting.Function).c_str());
                fflush(stdout);
                fail();
            }
        }
        break;
        case table::CellContentType_EMPTY:
        {
            double val = maData.CacheTable.aggregateValue(filters, setting.FieldId, GeneralFunction_COUNTNUMS);
            if (val != 0.0)
            {
                fprintf(stdout, "Error: value not empty (%ld, %ld) : display cell is empty but associated data set is not empty.",
                        cell.Row, cell.Column);
                fflush(stdout);
                fail();
            }
        }
        break;
        case table::CellContentType_TEXT:
            fail(cell.Row, cell.Column, "this cell is text!");
        break;
        case table::CellContentType_FORMULA:
        {
            sal_Int32 error = xCell->getError();
            if (!error)
                fail(cell.Row, cell.Column, "cell type is formula but it's not in error condition");

            double val = maData.CacheTable.aggregateValue(filters, setting.FieldId, setting.Function);
            if (finite(val))
                fail(cell.Row, cell.Column, "cell is in error condition but the value is finite");
        }
        break;
        default:
            fprintf(stdout, "I don't know how to handle this (%ld, %ld) (cell content type = %d)\n",
                    cell.Row, cell.Column, xCell->getType());
            fflush(stdout);
            fail();
    }
}

void ResultTester::verifyRefValue(const CellAddress& cell, 
                                  const DataFieldSetting& setting, 
                                  const vector<DataTable::Filter>& filters,
                                  const DataResult& result)
{
    Reference<XCell> xCell = maData.OutputSheetRef->getCellByPosition(cell.Column, cell.Row);
    table::CellContentType cellType = xCell->getType();
    double valCell = xCell->getValue();

    CellRangeAddress resRange = mxDPTab->getOutputRangeByType(DataPilotTableRegion::RESULT);
    bool isRowSubtotal = (result.Flags & DataResultFlags::SUBTOTAL) && (resRange.EndColumn == cell.Column);
    bool isColSubtotal = false;
    do
    {
        if (!(result.Flags & DataResultFlags::SUBTOTAL))
            break;
        sal_Int32 beginRow = resRange.EndRow - maDataFieldSettings.size() + 1;
        sal_Int32 endRow = resRange.EndRow;
        if (cell.Row < beginRow)
            break;
        if (cell.Row > endRow)
            break;
        isColSubtotal = true;
    }
    while (false);

    const DataPilotFieldReference& ref = *setting.FieldRef;
    sal_Int32 refFieldId = maData.CacheTable.getFieldIndex(ref.ReferenceField);
    DataPilotFieldOrientation refOrient = maData.FieldOrientations.at(refFieldId);

    if (refOrient == DataPilotFieldOrientation_COLUMN && isRowSubtotal && 
        cellType == table::CellContentType_EMPTY)
        // This is expected.
        return;

    if (refOrient == DataPilotFieldOrientation_ROW && isColSubtotal && 
        cellType == table::CellContentType_EMPTY)
        // This is also expected.
        return;

    const sal_Int32 nFieldId = setting.FieldId;
    const sheet::GeneralFunction func = setting.Function;

    // Obtain the aggregate value with the original filter set.
    double valOrig = maData.CacheTable.aggregateValue(filters, nFieldId, func);

    // Go through the filters and find the field that matches the referenced field, then
    // replace the match value with the referenced item name.
    vector<DataTable::Filter> filters2;
    vector<DataTable::Filter>::const_iterator itr = filters.begin(), itrEnd = filters.end();
    bool isRefItem = false;
    for (; itr != itrEnd; ++itr)
    {
        if (itr->FieldIndex == refFieldId)
        {
            // This is the referenced field.  Replace the match value with
            // the referenced item name.
            DataTable::Filter filter(*itr);
            sal_Int32 newStrId = DataTable::getStringId(ref.ReferenceItemName);
            if (filter.MatchStrId == newStrId)
                isRefItem = true;
            else
                filter.MatchStrId = newStrId;
            filters2.push_back(filter);
        }
        else
            filters2.push_back(*itr);
    }

    double valRef = maData.CacheTable.aggregateValue(filters2, nFieldId, func);

    switch (ref.ReferenceType)
    {
        case DataPilotFieldReferenceType::NONE:
            /* no reference mode. */
            fprintf(stdout, "Error: reference type is set to NONE\n");
            fail();
            return;

        case DataPilotFieldReferenceType::ITEM_DIFFERENCE:
        {
            /* subtract the reference value and display the difference. */

            if (isRefItem && cellType == table::CellContentType_EMPTY)
                // the referenced item should be empty.
                return;

            double res = valOrig - valRef;
            if (!compare(valCell, res))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value %g - %g = %.10f (%s)\n",
                        cell.Row, cell.Column,
                        valCell, valOrig, valRef, res,
                        getFunctionName(func).c_str());
                fail();
            }
            return;
        }
        case DataPilotFieldReferenceType::ITEM_PERCENTAGE:
        {
            /* each result is dividied by its reference value. */

            if (valRef == 0.0)
            {
                // This is division by zero.  The cell result should also be an error.
                if ((result.Flags & DataResultFlags::ERROR))
                    // This is expected.
                    fprintf(stdout, "Info: division by zero for referenced item (%s)\n",
                            getReferenceTypeName(ref.ReferenceType).c_str());
                else
                    fail();

                return;
            }

            double res = valOrig/valRef;
            if (!compare(valCell, res))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value %g/%g = %.10f (%s)\n",
                        cell.Row, cell.Column,
                        valCell, valOrig, valRef, res,
                        getFunctionName(func).c_str());
                fail();
            }
            return;
        }
        case DataPilotFieldReferenceType::ITEM_PERCENTAGE_DIFFERENCE:
        {
            /* from each result, its reference value is subtracted, and the
             * difference is further divided by the reference value. 
             */

            if (isRefItem && cellType == table::CellContentType_EMPTY)
                // the referenced item should be empty.
                return;

            if (valRef == 0.0)
            {
                // This is division by zero.  The cell result should also be an error.
                if ((result.Flags & DataResultFlags::ERROR))
                    // This is expected.
                    fprintf(stdout, "Info: division by zero for referenced item (%s)\n",
                            getReferenceTypeName(ref.ReferenceType).c_str());
                else
                    fail();

                return;
            }

            double res = (valOrig-valRef)/valRef;
            if (!compare(valCell, res))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value (%g-%g)/%g = %.10f (delta = %.10f) (%s)\n",
                        cell.Row, cell.Column,
                        valCell, valOrig, valRef, valRef, res, valCell-res,
                        getFunctionName(func).c_str());
                fail();
            }
            return;
        }
        case DataPilotFieldReferenceType::RUNNING_TOTAL:
        {
            /* Each result is added to the sum of the results for preceding
             * items in the base field, in the base field's sort order, and 
             * the total sum is shown.
             */

            fprintf(stdout, "* TEST CODE NOT IMPLEMENTED (%ld, %ld)\n", cell.Row, cell.Column);
            fail();
        }
        break;
        default:
            fprintf(stdout, "* UNKNOWN REFERENCE TYPE (%ld, %ld)\n",
                    cell.Row, cell.Column);
            fail();
    }
}

void ResultTester::verifyPercentValue(const ::com::sun::star::table::CellAddress& cell, 
                                      const DataFieldSetting& setting,
                                      const vector<DataTable::Filter>& filters,
                                      const DataResult& result)
{
    const DataPilotFieldReference& ref = *setting.FieldRef;
    sal_Int32 refFieldId = maData.CacheTable.getFieldIndex(ref.ReferenceField);
    DataPilotFieldOrientation refOrient = maData.FieldOrientations.at(refFieldId);

    // Get the value displayed in the cell.
    Reference<XCell> xCell = maData.OutputSheetRef->getCellByPosition(cell.Column, cell.Row);
    table::CellContentType cellType = xCell->getType();
    double valCell = xCell->getValue();

    // Obtain the aggregate value with the original filter set.
    double valOrig = maData.CacheTable.aggregateValue(filters, setting.FieldId, setting.Function);

    switch (ref.ReferenceType)
    {
        case DataPilotFieldReferenceType::ROW_PERCENTAGE:
        case DataPilotFieldReferenceType::COLUMN_PERCENTAGE:
        {
            // Each result is divided by the total result for its row in 
            // the DataPilot table. 

            double valTotal = getColRowTotal(setting, filters, 
                                             ref.ReferenceType == DataPilotFieldReferenceType::ROW_PERCENTAGE);

            if (valTotal == 0.0)
            {
                // This is division by zero.  The cell result should also be an error.
                if ((result.Flags & DataResultFlags::ERROR))
                    // This is expected.
                    fprintf(stdout, "Info: division by zero for referenced item (%s)\n",
                            getReferenceTypeName(ref.ReferenceType).c_str());
                else
                    fail("row total is zero but the displayed cell value is not an error");

                return;
            }

            double res = valOrig/valTotal;
            if (!compare(res, valCell))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value %g/%g = %.10f (delta = %.10f) (%s)\n",
                        cell.Row, cell.Column,
                        valCell, valOrig, valTotal, res, valCell-res,
                        getFunctionName(setting.Function).c_str());
                fail();
            }
        }
        break;
        case DataPilotFieldReferenceType::TOTAL_PERCENTAGE:
        {
            // Same as DataPilotFieldReferenceType::ROW_PERCENTAGE , but the grand 
            // total for the result's data field is used. 
            double valTotal = getGrandTotal(setting);
            double res = valOrig/valTotal;
            if (!compare(res, valCell))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value %g/%g = %.10f (delta = %.10f) (%s)\n",
                        cell.Row, cell.Column,
                        valCell, valOrig, valTotal, res, valCell-res,
                        getFunctionName(setting.Function).c_str());
                fail();
            }
        }
        break;
        case DataPilotFieldReferenceType::INDEX:
        {
            // The row and column totals and the grand total, following the same 
            // rules as above, are used to calculate the following expression.
            //   ( original result * grand total ) / ( row total * column total )
            // Division by zero results in an error. Otherwise, empty results remain empty.
            double valTotal = getGrandTotal(setting);
            double rowTotal = getColRowTotal(setting, filters, true);
            double colTotal = getColRowTotal(setting, filters, false);
            if (rowTotal == 0.0|| colTotal == 0.0)
            {
                // This is division by zero.  The cell result should also be an error.
                if ((result.Flags & DataResultFlags::ERROR))
                    // This is expected.
                    fprintf(stdout, "Info: division by zero for referenced item (%s)\n",
                            getReferenceTypeName(ref.ReferenceType).c_str());
                else
                    fail("row total or column total is zero but the displayed cell value is not an error");

                return;
            }
            double res = (valOrig*valTotal)/(rowTotal*colTotal);
            if (!compare(valCell, res))
            {
                fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value (%g*%g)/(%g*%g) = %.10f (delta = %.10f) (%s)\n",
                        cell.Row, cell.Column,
                        valCell, valOrig, valTotal, rowTotal, colTotal, res, valCell-res,
                        getFunctionName(setting.Function).c_str());
                fail();
            }
        }
        break;
        default:
            fprintf(stdout, "* UNKNOWN REFERENCE TYPE (%ld, %ld)\n",
                    cell.Row, cell.Column);
            fail();
    }
}

void ResultTester::verifyRunningTotal(const ::com::sun::star::table::CellAddress& cell, 
                                      const DataFieldSetting& setting,
                                      const vector<DataTable::Filter>& filters,
                                      const DataResult& result)
{
    Reference<XCell> xCell = maData.OutputSheetRef->getCellByPosition(cell.Column, cell.Row);
    table::CellContentType cellType = xCell->getType();
    double valCell = xCell->getValue();

    CellRangeAddress resRange = mxDPTab->getOutputRangeByType(DataPilotTableRegion::RESULT);
    bool isRowSubtotal = (result.Flags & DataResultFlags::SUBTOTAL) && (resRange.EndColumn == cell.Column);
    bool isColSubtotal = false;
    do
    {
        if (!(result.Flags & DataResultFlags::SUBTOTAL))
            break;
        sal_Int32 beginRow = resRange.EndRow - maDataFieldSettings.size() + 1;
        sal_Int32 endRow = resRange.EndRow;
        if (cell.Row < beginRow)
            break;
        if (cell.Row > endRow)
            break;
        isColSubtotal = true;
    }
    while (false);

    const DataPilotFieldReference& ref = *setting.FieldRef;
    sal_Int32 refFieldId = maData.CacheTable.getFieldIndex(ref.ReferenceField);
    DataPilotFieldOrientation refOrient = maData.FieldOrientations.at(refFieldId);

    // When the referenced field is a column field, the row subtotal cells should
    // be empty, whereas when the referenced field is a row field, then the column
    // subtotal cells should be empty.  When the referenced field is either a page
    // field or a data field, then the cell value should be value error.

    if (refOrient == DataPilotFieldOrientation_COLUMN && isRowSubtotal && 
        cellType == table::CellContentType_EMPTY)
        // This is expected.
        return;

    if (refOrient == DataPilotFieldOrientation_ROW && isColSubtotal && 
        cellType == table::CellContentType_EMPTY)
        // This is also expected.
        return;

    sal_Int32 posRefId = -1, curItemName = -1;
    {
        FilterFinder finder(refFieldId);
        vector<DataTable::Filter>::const_iterator pos = find_if(filters.begin(), filters.end(), finder);
        if (pos == filters.end())
            fail("filter for the reference field not found (ResultTester::verifyRunningTotal)");
        posRefId = distance(filters.begin(), pos);
        curItemName = pos->MatchStrId;
    }

    FieldItemMapType::const_iterator itr = maFieldItemMap.find(refFieldId);
    if (itr == maFieldItemMap.end())
        fail("failed to find the item list (ResultTester::verifyRunningTotal)");

    vector<sal_Int32>::const_iterator curItemPos = find(itr->second.begin(), itr->second.end(), curItemName);
    if (curItemPos == itr->second.end())
        fail("current item not found (ResultTester::verifyRunningTotal)");

    // Run through each individual field item name up to the current item name,
    // and accumulate individual aggregate values of those items.
    RunningTotalCalculator calculator(maData.CacheTable, filters, setting.FieldId, setting.Function, posRefId);
    double valOrig = for_each(itr->second.begin(), ++curItemPos, calculator).getRunningTotal();

    if (!compare(valCell, valOrig))
    {
        fprintf(stdout, "Error: values differ (%ld, %ld) : real value = %.10f  check value = %.10f (delta = %.10f) (%s)\n",
                cell.Row, cell.Column,
                valCell, valOrig, valCell-valOrig,
                getFunctionName(setting.Function).c_str());
        fail();
    }
}

void ResultTester::storeColRowFieldsData(bool isRow)
{
    Reference<XDataPilotDescriptor> xDPDesc(mxDPTab, UNO_QUERY_THROW);
    Reference<container::XIndexAccess> xFields = isRow ? xDPDesc->getRowFields() : xDPDesc->getColumnFields();
    sal_Int32 fieldCount = xFields->getCount();
    for (sal_Int32 i = 0; i < fieldCount; ++i)
    {
        Reference<container::XNamed> xField(xFields->getByIndex(i), UNO_QUERY_THROW);
        OUString fieldNameStr = xField->getName();
        if (isRow && i == fieldCount - 1 && fieldNameStr.equalsAscii("Data"))
        {
            fprintf(stdout, "INFO: skipping data dimension....\n");
            continue;
        }

        sal_Int32 nFldId = maData.CacheTable.getFieldIndex(fieldNameStr);
        if (nFldId < 0)
            fail("field ID is negative (ResultTester::storeColRowFieldsData");

        if (isRow)
            maRowFieldIds.insert(nFldId);
        else
            maColFieldIds.insert(nFldId);

        Reference<XDataPilotField> xField2(xField, UNO_QUERY_THROW);
        Reference<XIndexAccess> xItems(xField2->getItems(), UNO_QUERY_THROW);
        sal_Int32 itemCount = xItems->getCount();
        vector<sal_Int32> fieldItems;
        fieldItems.reserve(itemCount);
        for (sal_Int32 j = 0; j < itemCount; ++j)
        {
            Reference<container::XNamed> xName(xItems->getByIndex(j), UNO_QUERY_THROW);
            sal_Int32 itemId = DataTable::getStringId(xName->getName());
            if (itemId < 0)
                fail("item name string ID is negative");
            fieldItems.push_back(itemId);
        }
        maFieldItemMap.insert( FieldItemMapType::value_type(nFldId, fieldItems) );
    }
}

double ResultTester::getGrandTotal(const DataFieldSetting& setting)
{
    return maData.CacheTable.aggregateValue(vector<DataTable::Filter>(), 
                                            setting.FieldId,
                                            setting.Function);
}

double ResultTester::getColRowTotal(const DataFieldSetting& setting, const vector<DataTable::Filter>& filters, bool isRow)
{
    Reference<XDataPilotDescriptor> xDPDesc(mxDPTab, UNO_QUERY_THROW);

    // Now, calculate the column/row total.
    vector<DataTable::Filter> filters2;
    vector<DataTable::Filter>::const_iterator itr = filters.begin(), itrEnd = filters.end();
    for (; itr != itrEnd; ++itr)
    {
        if ( (isRow && maRowFieldIds.find(itr->FieldIndex) == maRowFieldIds.end()) ||
             (!isRow && maColFieldIds.find(itr->FieldIndex) == maColFieldIds.end()) )
            // This is not a column/row field.  Skip it.
            continue;
        filters2.push_back(*itr);
    }
    return maData.CacheTable.aggregateValue(filters2, setting.FieldId, setting.Function);
}

void ResultTester::fail(sal_Int32 row, sal_Int32 col, const char* reason)
{
    fprintf(stdout, "ERROR: (%ld, %ld) %s\n", row, col, reason);
    ++mnFailureCount;

    throw RuntimeException();
}

void ResultTester::fail(const char* reason)
{
    if (reason)
        fprintf(stdout, "ERROR: %s\n", reason);
    ++mnFailureCount;

    throw RuntimeException();
}

}
