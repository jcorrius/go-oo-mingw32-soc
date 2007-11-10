
#include "dptestbase.hxx"
#include "global.hxx"

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/util/XCloseable.hpp>

#include <memory>

using ::com::sun::star::frame::XDesktop;
using ::com::sun::star::util::XCloseable;
using ::com::sun::star::sheet::XSpreadsheetDocument;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::std::auto_ptr;

int main()
{
    using namespace dptest;

    TestParam param;
    param.FieldCount = 9;
    param.RowCount   = 100;
    param.DataCount  = 6;
    param.StartCol   = 0;
    param.StartRow   = 0;
    param.FieldItemCountLower = 3;
    param.FieldItemCountUpper = 3;

    Reference<XDesktop> xDesktop = bootstrap();

    Reference<XSpreadsheetDocument> xSpDoc = loadComponent(xDesktop, "private:factory/scalc");
    DPTestBase test(xSpDoc);
    test.run(param);
    return 0;

    for (sal_Int32 i = 0; i < 1; ++i)
    {
        Reference<XSpreadsheetDocument> xSpDoc = loadComponent(xDesktop, "private:factory/scalc");
        DPTestBase test(xSpDoc);
        test.run(param);
        Reference<XCloseable> xCloseable(xSpDoc, UNO_QUERY_THROW);
        xCloseable->close(false);
    }

    xDesktop->terminate();
}
