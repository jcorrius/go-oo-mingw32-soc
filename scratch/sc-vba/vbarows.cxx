#include <cppuhelper/queryinterface.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/table/XColumnRowRange.hpp>

#include "vbarows.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

double
ScVbaRows::getCount() throw (uno::RuntimeException)
{
	return mxRows->getCount();
}
