#ifndef __DPRESULTTESTER_HXX__
#define __DPRESULTTESTER_HXX__

#include "global.hxx"
#include "dpcachetable.hxx"
#include "dptestbase.hxx"
#include "cppuhelper/implementationentry.hxx"

#include <com/sun/star/sheet/GeneralFunction.hpp>
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#include <com/sun/star/sheet/XDataPilotTable2.hpp>

#include <vector>

namespace com { namespace sun { namespace star { 
    namespace sheet {
        struct DataResult;
    }

    namespace table {
        struct CellAddress;
        struct CellRangeAddress;
    }
}}}

namespace dptest {

class ResultTester
{
    struct DataFieldSetting
    {
        sal_Int32 FieldId;
        ::com::sun::star::sheet::GeneralFunction Function;
        ::boost::shared_ptr< ::com::sun::star::sheet::DataPilotFieldReference > FieldRef;

        DataFieldSetting();
    };

public:
    ResultTester(const RuntimeData& data, 
                 const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotTable2 >& xDPTab);
    ResultTester(const ResultTester& other);

    void init();

    void operator()(const ::com::sun::star::table::CellAddress& cell);

    sal_Int16 getFailureCount() const;

    void verifyRefValue(const ::com::sun::star::table::CellAddress& cell, 
                        const DataFieldSetting& setting, 
                        const ::std::vector<DataTable::Filter>& filters,
                        const ::com::sun::star::sheet::DataResult& result);

    void verifyPercentValue(const ::com::sun::star::table::CellAddress& cell, 
                            const DataFieldSetting& setting,
                            const ::std::vector<DataTable::Filter>& filters,
                            const ::com::sun::star::sheet::DataResult& result);

private:
    ResultTester(); // disabled
    void fail();

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotTable2 >     mxDPTab;
    RuntimeData                         maData;
    ::std::vector<DataFieldSetting>     maDataFieldSettings;
    sal_Int16                           mnFailureCount;
};


}

#endif
