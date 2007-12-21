
#include "dptestbase.hxx"
#include "global.hxx"

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/util/XCloseable.hpp>

#include <vector>
#include <memory>

using ::com::sun::star::frame::XDesktop;
using ::com::sun::star::util::XCloseable;
using ::com::sun::star::sheet::XSpreadsheetDocument;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::std::auto_ptr;
using ::std::vector;
using ::rtl::OUString;

int main()
{
    using namespace dptest;

    TestParam param;
    param.FieldCount = 7;
    param.RowCount   = 2000;
    param.DataCount  = 3;
    param.StartCol   = 0;
    param.StartRow   = 0;
    param.FieldItemCountLower = 2;
    param.FieldItemCountUpper = 3;
    param.RandomString = true;

#if 0
    param.Fields.push_back(FieldParam());
    param.Fields.back().Name = ascii("Company");
    param.Fields.back().ItemNames.push_back(ascii("Novell"));
    param.Fields.back().ItemNames.push_back(ascii("IBM"));
    param.Fields.back().ItemNames.push_back(ascii("Sun"));
    param.Fields.back().ItemNames.push_back(ascii("Microsoft"));

    param.Fields.push_back(FieldParam());
    param.Fields.back().Name = ascii("Period");
    param.Fields.back().ItemNames.push_back(ascii("Q1"));
    param.Fields.back().ItemNames.push_back(ascii("Q2"));
    param.Fields.back().ItemNames.push_back(ascii("Q3"));
    param.Fields.back().ItemNames.push_back(ascii("Q4"));

    param.Fields.push_back(FieldParam());
    param.Fields.back().Name = ascii("Category");
    param.Fields.back().ItemNames.push_back(ascii("Revenue"));
    param.Fields.back().ItemNames.push_back(ascii("Expense"));
    param.Fields.back().ItemNames.push_back(ascii("Other"));

    param.Fields.push_back(FieldParam());
    param.Fields.back().Name = ascii("Year");
    param.Fields.back().ItemNames.push_back(ascii("2001"));
    param.Fields.back().ItemNames.push_back(ascii("2002"));
    param.Fields.back().ItemNames.push_back(ascii("2003"));
    param.Fields.back().ItemNames.push_back(ascii("2004"));
    param.Fields.back().ItemNames.push_back(ascii("2005"));
    param.Fields.back().ItemNames.push_back(ascii("2006"));
    param.Fields.back().ItemNames.push_back(ascii("2007"));

    param.DataFields.push_back(DataFieldParam());
    DataFieldParam& rParam = param.DataFields.back();
    rParam.Name = ascii("USD (millions)");
    rParam.ValueLower = 10;
    rParam.ValueUpper = 1000;
#endif    

    Reference<XDesktop> xDesktop = bootstrap();

    Reference<XSpreadsheetDocument> xSpDoc = loadComponent(xDesktop, "private:factory/scalc");
    DPTestBase test(xSpDoc, param);
    test.run();
    return 0;

    for (sal_Int32 i = 0; i < 10; ++i)
    {
        Reference<XSpreadsheetDocument> xSpDoc = loadComponent(xDesktop, "private:factory/scalc");
        DPTestBase test(xSpDoc, param);
        test.run();
        Reference<XCloseable> xCloseable(xSpDoc, UNO_QUERY_THROW);
        xCloseable->close(false);
    }

    xDesktop->terminate();
}
