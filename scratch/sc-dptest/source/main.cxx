
#include "dptestbase.hxx"
#include "global.hxx"

#include <memory>

using ::com::sun::star::sheet::XSpreadsheetDocument;
using ::com::sun::star::uno::Reference;
using ::std::auto_ptr;

int main()
{
    using namespace dptest;

    Reference<XSpreadsheetDocument> xSpDoc = bootstrap("private:factory/scalc");
    DPTestBase test(xSpDoc);
    test.run();
}
