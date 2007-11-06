#ifndef GLOBAL_HXX
#define GLOBAL_HXX

#include "cppuhelper/implementationentry.hxx"

#include <com/sun/star/beans/XPropertySet.hpp>

#include <stdio.h>

namespace com { namespace sun { namespace star { 
    namespace beans {
        class XPropertySet;
    }
    namespace sheet {
        class XSpreadsheet;
        class XSpreadsheets;
        class XSpreadsheetDocument;
    }
}}}

namespace dptest {

::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument > 
    bootstrap(const char* docPath);

::rtl::OUString ascii(const sal_Char* cstr);

const sal_Char* getStr(const ::rtl::OUString& ou);

::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > 
    getSheetByName(const ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument >& xSpDoc, 
                   const ::rtl::OUString& name);

void dumpAllPropertyNames(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xProps);

template<typename T>
void getPropertyValue(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xProps,
                      const ::rtl::OUString& name, T& value)
{
    ::com::sun::star::uno::Any any = xProps->getPropertyValue(name);
    any >>= value;
}

}

#endif
