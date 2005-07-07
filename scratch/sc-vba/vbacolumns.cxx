#include <cppuhelper/queryinterface.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/table/XColumnRowRange.hpp>

#include "vbacolumns.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

double
ScVbaColumns::getCount() throw (uno::RuntimeException)
{
	return mxColumns->getCount();
}
