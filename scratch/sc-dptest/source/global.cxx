
#include "global.hxx"
#include "cppuhelper/bootstrap.hxx"

#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/sheet/DataPilotFieldReferenceType.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::sheet;

using ::com::sun::star::beans::Property;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::container::XNameContainer;
using ::com::sun::star::frame::XDesktop;
using ::com::sun::star::lang::XComponent;
using ::com::sun::star::lang::XMultiComponentFactory;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::uno::XComponentContext;
using ::com::sun::star::uno::XInterface;
using ::com::sun::star::sheet::GeneralFunction;
using ::rtl::OUString;

namespace dptest {

Reference<XDesktop> bootstrap()
{
    Reference<XSpreadsheetDocument> xSpDoc;
    Reference<XComponentContext> xCC = ::cppu::bootstrap();
    Reference<XMultiComponentFactory> xFactory = xCC->getServiceManager();

    Reference<XDesktop> xDesktop(xFactory->createInstanceWithContext(
        ascii("com.sun.star.frame.Desktop"), xCC), UNO_QUERY_THROW);

    return xDesktop;
}

Reference<XSpreadsheetDocument> loadComponent(const Reference<XDesktop>& xDesktop, const char* docPath)
{
    Reference<XSpreadsheetDocument> xSpDoc;
    Reference<frame::XComponentLoader> xLoader(xDesktop, UNO_QUERY_THROW);
    Sequence< beans::PropertyValue > args;
    Reference< XComponent > xComponent = xLoader->loadComponentFromURL( 
        ascii(docPath), ascii("_default"), 0, args);

    if ( !xComponent.is() )
    {
        printf("component failed to load\n");
        return xSpDoc;
    }

    xSpDoc.set(xComponent, UNO_QUERY_THROW);
    return xSpDoc;
}

OUString ascii(const sal_Char* cstr)
{
	return OUString::createFromAscii(cstr);
}

const sal_Char* getStr(const OUString& ou)
{
    return OUStringToOString(ou, RTL_TEXTENCODING_UTF8).getStr();
}

Reference<XSpreadsheet> getSheetByName(const Reference<XSpreadsheetDocument>& xSpDoc, const OUString& name)
{
    Reference<container::XNameAccess> xNA(xSpDoc->getSheets(), UNO_QUERY_THROW);
    Reference<XSpreadsheet> xSheet(xNA->getByName(name), UNO_QUERY_THROW);
    return xSheet;
}

void dumpAllPropertyNames(const Reference<XPropertySet>& xPropSet)
{
    
    Reference<XPropertySetInfo> xPSInfo = xPropSet->getPropertySetInfo();
    Sequence<Property> props = xPSInfo->getProperties();
    for (sal_Int32 j = 0; j < props.getLength(); ++j)
    {
        printf("  property name = '%s'\n", 
               OUStringToOString(props[j].Name, RTL_TEXTENCODING_UTF8).getStr());
    }
}

::std::string getFunctionName(const GeneralFunction func)
{
    using ::std::string;

    switch (func)
    {
        case GeneralFunction_NONE:
            return string("NONE");
        case GeneralFunction_AUTO:
            return string("AUTO");
        case GeneralFunction_SUM:
            return string("SUM");
        case GeneralFunction_COUNT:
            return string("COUNT");
        case GeneralFunction_AVERAGE:
            return string("AVERAGE");
        case GeneralFunction_MAX:
            return string("MAX");
        case GeneralFunction_MIN:
            return string("MIN");
        case GeneralFunction_PRODUCT:
            return string("PRODUCT");
        case GeneralFunction_COUNTNUMS:
            return string("COUNTNUMS");
        case GeneralFunction_STDEV:
            return string("STDEV");
        case GeneralFunction_STDEVP:
            return string("STDEVP");
        case GeneralFunction_VAR:
            return string("VAR");
        case GeneralFunction_VARP:
            return string("VARP");
    }
    return string("");
}

::std::string getReferenceTypeName(const sal_Int32 refType)
{
    using ::std::string;

    switch (refType)
    {
        case DataPilotFieldReferenceType::NONE:
            return string("NONE");
        case DataPilotFieldReferenceType::ITEM_DIFFERENCE:
            return string("ITEM_DIFFERENCE");
        case DataPilotFieldReferenceType::ITEM_PERCENTAGE:
            return string("ITEM_PERCENTAGE");
        case DataPilotFieldReferenceType::ITEM_PERCENTAGE_DIFFERENCE:
            return string("ITEM_PERCENTAGE_DIFFERENCE");
        case DataPilotFieldReferenceType::RUNNING_TOTAL:
            return string("RUNNING_TOTAL");
        case DataPilotFieldReferenceType::ROW_PERCENTAGE:
            return string("ROW_PERCENTAGE");
        case DataPilotFieldReferenceType::COLUMN_PERCENTAGE:
            return string("COLUMN_PERCENTAGE");
        case DataPilotFieldReferenceType::TOTAL_PERCENTAGE:
            return string("TOTAL_PERCENTAGE");
        case DataPilotFieldReferenceType::INDEX:
            return string("INDEX");
    }
    return string("");
}

bool compare(double a, double b, double tol)
{
    if (a == 0.0)
        return b == 0.0;

    if (a == b)
        return true;

    double diff = a > b ? a - b : b - a;
    double c = a > 0 ? a : -a;
    return (diff < c*tol);
}

}
