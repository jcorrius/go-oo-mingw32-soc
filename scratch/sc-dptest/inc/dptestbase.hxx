#ifndef __DPTESTBASE_HXX__
#define __DPTESTBASE_HXX__

#include "cppuhelper/implementationentry.hxx"
#include <com/sun/star/table/CellRangeAddress.hpp>

#include <memory>
#include <boost/shared_ptr.hpp>

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
};

/** 
 * information referenced during run-time testing.
 */
struct RuntimeData
{
    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > OutputSheetRef;
    sal_Int32 OutputSheetId;
};

class DPTestBase
{
public:
    DPTestBase(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument >& rSpDoc);

    ~DPTestBase();

    void run();

private:
    DPTestBase(); // disabled

    const ::rtl::OUString getFieldName(sal_Int16 fieldId) const;
    const ::rtl::OUString getFieldItemName(sal_Int16 fieldId, sal_Int32 itemId) const;

    void genSrcData(const TestParam& param);
    void genDPTable(const TestParam& param, const ::com::sun::star::table::CellRangeAddress& srcRange, 
                    const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >& xDestSheet);
    void dumpTableProperties(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >& xSheet);
    void dumpFields(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& xFields) const;
    void dumpItems(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& xItems) const;

    void verifyTableResults(const RuntimeData& data);
    void verifyCellResult(const RuntimeData& data, const ::com::sun::star::table::CellAddress& cell,
                          const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotTable2 >& xTable);

    const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >
        getSrcSheet() const;

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument > mxSpDoc;
    ::boost::shared_ptr< ::com::sun::star::table::CellRangeAddress > mpSrcRange;
};

}

#endif
