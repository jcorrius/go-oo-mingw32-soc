#ifndef __DPTESTBASE_HXX__
#define __DPTESTBASE_HXX__

#include "cppuhelper/implementationentry.hxx"
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>

#include <memory>
#include <boost/shared_ptr.hpp>

#include <dpcachetable.hxx>

namespace com { namespace sun { namespace star { 
    namespace container {
        class XIndexAccess;
    }
    namespace sheet {
        class XSpreadsheetDocument;
        class XSpreadsheet;
        class XDataPilotTable2;
    }
    namespace table {
        struct CellAddress;
    }
}}}

namespace dptest {

struct FieldParam;
struct DataFieldParam;

/** 
 *  parameters that influence the size and other characteristics of data
 *  source.
 */
struct TestParam
{
    sal_Int32 FieldCount;
    sal_Int32 RowCount;
    sal_Int32 DataCount;
    sal_Int32 StartCol;
    sal_Int32 StartRow;

    sal_Int32 FieldItemCountLower;
    sal_Int32 FieldItemCountUpper;

    /** 
     * Whether to randomly insert a string data into data field.
     */
    bool RandomString;

    ::std::vector<FieldParam>       Fields;
    ::std::vector<DataFieldParam>   DataFields;
};

/** 
 *  information referenced during run-time testing.  It gets updated
 *  dynamically as test progresses.
 */
struct RuntimeData
{
    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > OutputSheetRef;
    sal_Int32 OutputSheetId;
    DataTable CacheTable;

    /** 
     * Stores the number of items created in each field.
     */
    ::std::vector<sal_Int32> FieldItemCounts;

    /** 
     * Stores the orientation of each field. 
     */
    std::vector< ::com::sun::star::sheet::DataPilotFieldOrientation > FieldOrientations;
};

struct FieldParam
{
    ::rtl::OUString Name;
    ::std::vector< ::rtl::OUString > ItemNames;
};

struct DataFieldParam
{
    ::rtl::OUString Name;
    sal_Int32       ValueLower;
    sal_Int32       ValueUpper;
};

class DPTestBase
{
public:
    DPTestBase(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument >& rSpDoc,
               const TestParam& param);

    ~DPTestBase();

    void run();

private:
    DPTestBase(); // disabled

    const ::rtl::OUString getFieldName(sal_Int16 fieldId) const;
    const ::rtl::OUString getFieldItemName(sal_Int16 fieldId, sal_Int32 itemId) const;
    const sal_Int32 getFieldItemCount(sal_Int16 fieldId) const;

    const ::rtl::OUString getDataFieldName(sal_Int16 fieldId) const;
    const sal_Int32 getDataFieldValueLower(sal_Int16 fieldId) const;
    const sal_Int32 getDataFieldValueUpper(sal_Int16 fieldId) const;

    void genSrcData(RuntimeData& data);
    void genDPTable(const ::com::sun::star::table::CellRangeAddress& srcRange, 
                    RuntimeData& data);
    void dumpTableProperties(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >& xSheet);
    void dumpFields(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& xFields) const;
    void dumpItems(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& xItems) const;

    void verifyTableResults(const RuntimeData& data);
    void setReferenceToField(const RuntimeData& data, sal_Int32 fieldId, sal_Int32 fieldItemId, sal_Int32 refType);
    void removeAllReferences(const RuntimeData& data);

    void groupRowFields(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotTable2 >& xDPTab, sal_Int32 groupSize = 2) const;

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument > mxSpDoc;
    ::boost::shared_ptr< ::com::sun::star::table::CellRangeAddress > mpSrcRange;
    TestParam maTestParam;
};

}

#endif
