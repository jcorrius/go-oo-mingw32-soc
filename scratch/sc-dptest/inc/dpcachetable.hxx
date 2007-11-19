#ifndef SC_DPCACHETABLE_HXX
#define SC_DPCACHETABLE_HXX

#include "cppuhelper/implementationentry.hxx"

#include <com/sun/star/sheet/GeneralFunction.hpp>

#include <vector>
#include <set>
#include <ext/hash_map>
#include <boost/shared_ptr.hpp>

namespace com { namespace sun { namespace star {
    namespace sdbc {
        class XRowSet;
    }
    namespace sheet {
        struct DataPilotFieldFilter;
        class XSpreadsheet;
    }
}}}

namespace dptest {

class ScSharedStringTable
{
public:
    static const sal_Int32 EMPTY = 0;

    sal_Int32 insertString(const ::rtl::OUString& aStr);
    sal_Int32 getStringId(const ::rtl::OUString& aStr);
    const ::rtl::OUString getString(sal_Int32 nId) const;

    ScSharedStringTable();
    ~ScSharedStringTable();

private:
    typedef ::__gnu_cxx::hash_map< ::rtl::OUString, sal_Int32, ::rtl::OUStringHash, ::std::equal_to< ::rtl::OUString > > SharedStrMap;

    ::std::vector< ::rtl::OUString > maSharedStrings;
    SharedStrMap maSharedStringIds;
    sal_Int32 mnStrCount;
};

// ----------------------------------------------------------------------------

class DataTable
{
public:
    enum CellType
    {
        CellType_Empty,
        CellType_Value,
        CellType_String,
        CellType_Error
    };

    struct Field
    {
        sal_Int32   Name;
        sal_Int32   Width; // field width for printing table (= max string length)
    };

    /** individual cell within table. */
    struct Cell
    {
        sal_Int32   StrId;
        double      Value;
        bool        IsNumeric;
        CellType    Type;

        Cell();
    };

    /** filtering criteria */
    struct Filter
    {
        sal_Int32   FieldIndex;
        sal_Int32   MatchStrId;

        Filter();
    };

    DataTable();
    ~DataTable();

    sal_Int32 getFieldCount() const;
    sal_Int32 getRowSize() const;
    sal_Int32 getColSize() const;

    void setTableSize(sal_Int32 rowsize, sal_Int16 colsize);
    void setFieldName(sal_Int16 fieldId, const ::rtl::OUString& name);
    void setCell(sal_Int32 row, sal_Int16 col, const DataTable::Cell& cell);
    void setCell(sal_Int32 row, sal_Int16 col, const ::rtl::OUString& str);
    void setCell(sal_Int32 row, sal_Int16 col, double value);

    /** 
     * Check whether a specified row is active or not.  When a row is active, 
     * it is used in calculation of the results data.  A row becomes inactive 
     * when it is filtered out by page field. 
     */ 
    bool isRowActive(sal_Int32 nRow) const;

    /** 
     * Get the cell instance at specified location within the data grid. Note 
     * that the data grid doesn't include the header row.  Don't delete the 
     * returned object! 
     */
    const DataTable::Cell* getCell(sal_Int32 nRow, sal_Int16 nCol) const;

    const ::rtl::OUString getFieldName(sal_Int32 nIndex) const;

    /** 
     * Get the field index (i.e. column ID in the original data source) based
     * on the string value that corresponds with the column title.  It returns 
     * -1 if no field matching the string value exists. 
     */
    sal_Int32 getFieldIndex(const ::rtl::OUString& rStr) const;

    void clear();
    void swap(DataTable& rOther);
    bool empty() const;

    void output() const;

    void output(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >& xSheet,
                sal_Int32 row, sal_Int16 col) const;

    double aggregateValue(const ::std::vector<DataTable::Filter>& filters, sal_Int32 dataFieldId,
                          ::com::sun::star::sheet::GeneralFunction func) const;

    static const ::rtl::OUString getString(sal_Int32 nId);
    static sal_Int32 getStringId(const ::rtl::OUString& aStr);

private:
    void updateFieldWidth(sal_Int16 col, const DataTable::Cell& cell);
    bool isValidPosition(sal_Int32 row, sal_Int16 col) const;

private:
    /** main data table. */
    ::std::vector< ::std::vector< DataTable::Cell > > maTable;

    /** header string IDs */
    ::std::vector<Field> maFields;

    /** 
     * used to track visibility of rows.  The first row below the header row
     * has the index of 0.
     */
    ::std::vector<bool> maRowsVisible;

    static sal_Int32 insertString(const ::rtl::OUString& aStr);
    static ScSharedStringTable maStringTable;
};

}
#endif
