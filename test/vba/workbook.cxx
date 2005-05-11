#include <cppuhelper/queryinterface.hxx>

#include "workbook.hxx"

WorkBook::WorkBook(const ::rtl::OUString& aFullName)
{
	maFullName = aFullName;
	//set maName and maPath extracted out of fullname?
}
WorkBook::~WorkBook()
{
}

// ??
uno::Any WorkBook::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{

}

// XWorkBook implementation
::rtl::OUString WorkBook::getName( ) throw (uno::RuntimeException)
{
    return maName;
}


::rtl::OUString WorkBook::getPath( ) throw (uno::RuntimeException)
{
    return maPath;
}

::rtl::OUString WorkBook::getFullName( ) throw (uno::RuntimeException)
{
    return maFullName;
}

