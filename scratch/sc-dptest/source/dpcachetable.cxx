/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dpcachetable.cxx,v $
 *
 *  $Revision: 1.1.2.4 $
 *
 *  last change: $Author: kohei $ $Date: 2007/10/31 19:58:43 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#include "dpcachetable.hxx"
#include "global.hxx"
#include "rtl/ustrbuf.hxx"

#include <sstream>
#include <numeric>
#include <stdio.h>

#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/sdbc/XResultSetMetaData.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/sheet/DataPilotFieldFilter.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XCellRangeData.hpp>
#include <com/sun/star/table/XCellRange.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::sheet;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::std::vector;
using ::std::set;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::sheet::DataPilotFieldFilter;
using ::com::sun::star::sheet::XCellRangeData;
using ::com::sun::star::table::XCellRange;

namespace dptest {

ScSharedStringTable::ScSharedStringTable() :
    mnStrCount(0)
{
    // empty string (ID = 0)
    maSharedStrings.push_back(OUString());
    maSharedStringIds.insert( SharedStrMap::value_type(OUString(), mnStrCount++) );
}

ScSharedStringTable::~ScSharedStringTable()
{
}

sal_Int32 ScSharedStringTable::insertString(const OUString& aStr)
{
    SharedStrMap::const_iterator itr = maSharedStringIds.find(aStr), 
        itrEnd = maSharedStringIds.end();

    if (itr == itrEnd)
    {
        // new string.
        maSharedStrings.push_back(aStr);
        maSharedStringIds.insert( SharedStrMap::value_type(aStr, mnStrCount) );
        return mnStrCount++;
    }

    // existing string.
    return itr->second;
}

sal_Int32 ScSharedStringTable::getStringId(const OUString& aStr)
{
    SharedStrMap::const_iterator itr = maSharedStringIds.find(aStr), 
        itrEnd = maSharedStringIds.end();
    if (itr == itrEnd)
    {
        // string not found.
        return insertString(aStr);
    }
    return itr->second;
}

const OUString ScSharedStringTable::getString(sal_Int32 nId) const
{
    if (nId >= mnStrCount)
        return OUString();

    return maSharedStrings[nId];
}

// ----------------------------------------------------------------------------

DataTable::Cell::Cell() :
    StrId(0),
    Value(0.0),
    IsNumeric(false),
    Type(CellType_Empty)
{
}

DataTable::Filter::Filter()
{
}

DataTable::MultiStringFilter::MultiStringFilter()
{
}

const DataTable::MultiStringFilter& DataTable::MultiStringFilter::operator =(const MultiStringFilter& o)
{
    this->FieldIndex = o.FieldIndex;
    this->MatchStrIds = o.MatchStrIds;
}

ScSharedStringTable DataTable::maStringTable;

// ----------------------------------------------------------------------------
// function objects

class Resizer
{
public:
    Resizer(sal_Int16 size) :
        mSize(size)
    {
    }

    void operator() (vector<DataTable::Cell>& array)
    {
        array.resize(mSize);
    }

private:
    sal_Int16 mSize;
};

// ----------------------------------------------------------------------------

class FieldRowPrinter
{
public:
    void begin()
    {
        printf("|");
    }

    void end()
    {
        printf("\n");
    }

    void operator() (const DataTable::Field& field)
    {
        OUStringBuffer buf;
        OUString name = DataTable::getString(field.Name);
        for (sal_Int32 i = 0; i < field.Width - name.getLength() + 1; ++i)
            buf.appendAscii(" ");
        buf.append(name);
        printf("%s |", getStr(buf.makeStringAndClear()));
    }
};

// ----------------------------------------------------------------------------

class DataTablePrinter
{
public:
    DataTablePrinter(const vector<DataTable::Field>& fields) :
        mrFields(fields)
    {
    }

    void begin() const
    {
        printSeparator();
        FieldRowPrinter fldPrinter;
        fldPrinter.begin();
        for_each(mrFields.begin(), mrFields.end(), fldPrinter);
        fldPrinter.end();
    }

    void operator() (const vector<DataTable::Cell>& rows) const
    {
        printSeparator();
        printf("|");

        OUStringBuffer buf;
        size_t n = mrFields.size();
        for (size_t col = 0; col < n; ++col)
        {
            const OUString str = DataTable::getString(rows.at(col).StrId);
            for (sal_Int32 i = 0; i < mrFields.at(col).Width - str.getLength() + 1; ++i)
                buf.appendAscii(" ");
            buf.append(str);
            printf("%s |", getStr(buf.makeStringAndClear()));
        }

        printf("\n");
    }

    void printSeparator() const
    {
        printf("+");
        vector<DataTable::Field>::const_iterator itr, itrEnd = mrFields.end();
        for (itr = mrFields.begin(); itr != itrEnd; ++itr)
        {
            vector<char> line;
            line.resize(itr->Width + 2, '-');
            line.push_back('\0');
            printf("%s+", &line[0]);
        }
        printf("\n");
    }

    void end() const
    {
        printSeparator();
    }

private:
    const vector<DataTable::Field>& mrFields;
};

// ----------------------------------------------------------------------------

class Filter2MultiFilter
{
public:
    Filter2MultiFilter(size_t filterSize)
    {
        maFilters.reserve(filterSize);
    }

    void operator() (const DataTable::Filter& filter)
    {
        DataTable::MultiStringFilter msfilter;
        msfilter.FieldIndex = filter.FieldIndex;
        msfilter.MatchStrIds.push_back(filter.MatchStrId);
        maFilters.push_back(msfilter);
    }

    void swapFilters(vector<DataTable::MultiStringFilter>& dest)
    {
        maFilters.swap(dest);
    }
private:
    Filter2MultiFilter(); // disabled
private:
    vector<DataTable::MultiStringFilter> maFilters;
};

// ----------------------------------------------------------------------------

class ResultAggregator
{
public:
    ResultAggregator(const vector<DataTable::Filter>& filters, sal_Int32 dataFieldId, 
                     GeneralFunction func, size_t rowCount) :
        mnDataFieldId(dataFieldId), meFunc(func)
    {
        // Convert those single-string filters to multi-string ones.
        Filter2MultiFilter converter(filters.size());
        for_each(filters.begin(), filters.end(), converter).swapFilters(maFilters);

        maValues.reserve(rowCount);
    }

    ResultAggregator(const vector<DataTable::MultiStringFilter>& filters, sal_Int32 dataFieldId, 
                     GeneralFunction func, size_t rowCount) :
        maFilters(filters), mnDataFieldId(dataFieldId), meFunc(func)
    {
        maValues.reserve(rowCount);
    }

    void operator()(const vector<DataTable::Cell>& row)
    {
        sal_Int32 rowSize = row.size();
        if (mnDataFieldId >= rowSize)
            return;

        bool includeRow = true;
        vector<DataTable::MultiStringFilter>::const_iterator itr, itrEnd = maFilters.end();
        for (itr = maFilters.begin(); itr != itrEnd; ++itr)
        {
            const sal_Int32 fieldId = itr->FieldIndex;
            if (fieldId >= rowSize || fieldId < 0)
                continue;

            if (find(itr->MatchStrIds.begin(), itr->MatchStrIds.end(), row.at(fieldId).StrId) == itr->MatchStrIds.end())
            {
                includeRow = false;
                break;
            }
        }
        if (includeRow)
            maValues.push_back(row.at(mnDataFieldId).Value);
    }

    double getValue() const
    {
        using ::std::accumulate;
        using ::std::multiplies;

        size_t valueSize = maValues.size();
        if (!valueSize)
            return 0.0;

        switch (meFunc)
        {
            case GeneralFunction_NONE:
                return 0.0;
            case GeneralFunction_AUTO:
                // AUTO == SUM !?
            case GeneralFunction_SUM:
                return accumulate(maValues.begin(), maValues.end(), 0.0);
            case GeneralFunction_AVERAGE:
                return accumulate(maValues.begin(), maValues.end(), 0.0) / valueSize;
            case GeneralFunction_COUNT:
                return static_cast<double>(maValues.size());
            case GeneralFunction_MAX:
            {
                double val = maValues.front();
                return accumulate(++maValues.begin(), maValues.end(), val, maxValue);
            }
            case GeneralFunction_MIN:
            {
                double val = maValues.front();
                return accumulate(++maValues.begin(), maValues.end(), val, minValue);
            }
            case GeneralFunction_PRODUCT:
                return accumulate(maValues.begin(), maValues.end(), 1.0, multiplies<double>());
            case GeneralFunction_COUNTNUMS:
            case GeneralFunction_STDEV:
            case GeneralFunction_STDEVP:
            case GeneralFunction_VAR:
            case GeneralFunction_VARP:
                break;
        }
        return 0.0;
    }

private:
    ResultAggregator(); // disabled

    static double maxValue(double a, double b)
    {
        return a > b ? a : b;
    }

    static double minValue(double a, double b)
    {
        return a < b ? a : b;
    }

private:
    vector<DataTable::MultiStringFilter> maFilters;
    const sal_Int32 mnDataFieldId;
    const GeneralFunction meFunc;

    vector<double> maValues;
};

// ----------------------------------------------------------------------------

class Data2SheetConverter
{
public:
    Data2SheetConverter(sal_Int32 rowCount) :
        mRowId(0)
    {
        mDataArray.realloc(rowCount);
    }

    ~Data2SheetConverter()
    {
    }

    void setFieldHeader(const vector<DataTable::Field>& field)
    {
        sal_Int32 n = field.size();
        Sequence<Any> row(n);
        for (sal_Int32 i = 0; i < n; ++i)
        {
            Any any;
            any <<= DataTable::getString(field.at(i).Name);
            row[i] = any;
        }
        mDataArray[mRowId++] = row;
    }

    void operator() (const vector<DataTable::Cell>& row)
    {
        sal_Int32 n = row.size();
        Sequence<Any> row2(n);
        for (sal_Int32 i = 0; i < n; ++i)
        {
            Any any;
            const DataTable::Cell& cell = row.at(i);
            if (cell.IsNumeric)
                any <<= cell.Value;
            else
                any <<= DataTable::getString(cell.StrId);
            row2[i] = any;
        }
        mDataArray[mRowId++] = row2;
    }

    Sequence< Sequence<Any> > getDataArray() const
    {
        return mDataArray;
    }

private:
    Sequence< Sequence<Any> > mDataArray;
    sal_Int32 mRowId;
};

// ----------------------------------------------------------------------------

DataTable::DataTable()
{
}

DataTable::~DataTable()
{
}

sal_Int32 DataTable::getFieldCount() const
{
    return maFields.size();
}

sal_Int32 DataTable::getRowSize() const
{
    return maTable.size();
}

sal_Int32 DataTable::getColSize() const
{
    return maTable.empty() ? 0 : maTable[0].size();
}

void DataTable::setTableSize(sal_Int32 rowsize, sal_Int16 colsize)
{
    if (colsize < 0 || rowsize < 0)
        return;

    maFields.resize(colsize);
    maTable.resize(rowsize);
    for_each(maTable.begin(), maTable.end(), Resizer(colsize));
    maRowsVisible.resize(rowsize, true);
}

void DataTable::setFieldName(sal_Int16 fieldId, const OUString& name)
{
    if (fieldId < 0)
        return;

    Field field;
    field.Name = getStringId(name);
    field.Width = name.getLength();

    maFields.at(fieldId) = field;
}

void DataTable::setCell(sal_Int32 row, sal_Int16 col, const DataTable::Cell& cell)
{
    if (!isValidPosition(row, col))
        return;

    maTable.at(row).at(col) = cell;
    updateFieldWidth(col, cell);
}

void DataTable::setCell(sal_Int32 row, sal_Int16 col, const OUString& str)
{
    if (!isValidPosition(row, col))
        return;

    Cell cell;
    cell.StrId = getStringId(str);
    cell.IsNumeric = false;
    cell.Type = CellType_String;
    maTable.at(row).at(col) = cell;
    updateFieldWidth(col, cell);
}

void DataTable::setCell(sal_Int32 row, sal_Int16 col, double value)
{
    if (!isValidPosition(row, col))
        return;

    ::std::ostringstream os;
    os << value;
    OUStringBuffer buf;
    buf.appendAscii(os.str().c_str());

    Cell cell;
    cell.Value = value;
    cell.StrId = getStringId(buf.makeStringAndClear());
    cell.IsNumeric = true;
    cell.Type = CellType_Value;
    maTable.at(row).at(col) = cell;
    updateFieldWidth(col, cell);
}

bool DataTable::isRowActive(sal_Int32 nRow) const
{
    if (nRow < 0 || nRow >= maRowsVisible.size())
        // row index out of bound
        return false;

    return maRowsVisible[nRow];
}

const DataTable::Cell* DataTable::getCell(sal_Int32 nRow, sal_Int16 nCol) const
{
    if ( nRow >= static_cast<sal_Int32>(maTable.size()) )
        return NULL;

    const vector<Cell>& rRow = maTable.at(nRow);
    if ( nCol < 0 || static_cast<size_t>(nCol) >= rRow.size() )
        return NULL;

    return &rRow.at(nCol);
}

const OUString DataTable::getFieldName(sal_Int32 nIndex) const
{
    return getString(maFields[nIndex].Name);
}

sal_Int32 DataTable::getFieldIndex(const OUString& rStr) const
{
    sal_Int32 nStrId = getStringId(rStr);
    if (nStrId < 0)
        // string not found.
        return nStrId;

    sal_Int32 n = maFields.size();
    for (sal_Int32 i = 0; i < n; ++i)
    {
        if (maFields[i].Name == nStrId)
            return i;
    }

    return -1;
}

void DataTable::clear()
{
    maTable.clear();
    maFields.clear();
    maRowsVisible.clear();
}

void DataTable::swap(DataTable& rOther)
{
    maTable.swap(rOther.maTable);
    maFields.swap(rOther.maFields);
    maRowsVisible.swap(rOther.maRowsVisible);
}

bool DataTable::empty() const
{
    return maTable.empty();
}

void DataTable::output() const
{
    DataTablePrinter dataPrinter(maFields);
    dataPrinter.begin();
    for_each(maTable.begin(), maTable.end(), dataPrinter);
    dataPrinter.end();
}

void DataTable::output(const Reference<XSpreadsheet>& xSheet, sal_Int32 row, sal_Int16 col) const
{
    // Calculate the entire table size.
    sal_Int32 width  = maFields.size();
    sal_Int32 height = maTable.size() + 1;
    Reference<XCellRange> xRange = xSheet->getCellRangeByPosition(col, row, col + width - 1, row + height - 1);

    // Construct a data table.
    Data2SheetConverter converter(height);
    converter.setFieldHeader(maFields);
    Sequence< Sequence<Any> > array = for_each(maTable.begin(), maTable.end(), converter).getDataArray();

    Reference<XCellRangeData> xRangeData(xRange, UNO_QUERY_THROW);
    xRangeData->setDataArray(array);
}

double DataTable::aggregateValue(const vector<DataTable::Filter>& filters, sal_Int32 dataFieldId,
                                 GeneralFunction func) const
{
    ResultAggregator aggregator(filters, dataFieldId, func, maTable.size());
    return for_each(maTable.begin(), maTable.end(), aggregator).getValue();
}

double DataTable::aggregateValue(const vector<DataTable::MultiStringFilter>& filters, sal_Int32 dataFieldId,
                                 GeneralFunction func) const
{
    ResultAggregator aggregator(filters, dataFieldId, func, maTable.size());
    return for_each(maTable.begin(), maTable.end(), aggregator).getValue();
}

// static 
sal_Int32 DataTable::insertString(const OUString& aStr)
{
    return maStringTable.insertString(aStr);
}

const OUString DataTable::getString(sal_Int32 nId)
{
    return maStringTable.getString(nId);
}

sal_Int32 DataTable::getStringId(const OUString& aStr)
{
    return maStringTable.getStringId(aStr);
}

void DataTable::updateFieldWidth(sal_Int16 col, const DataTable::Cell& cell)
{
    const OUString str = getString(cell.StrId);
    maFields.at(col).Width = ::std::max(maFields.at(col).Width, str.getLength());
}

bool DataTable::isValidPosition(sal_Int32 row, sal_Int16 col) const
{
    return (row >= 0) && (col >= 0);
}

}
