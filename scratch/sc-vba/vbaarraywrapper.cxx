#include "vbaarraywrapper.hxx"
using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScArrayWrapper::ScArrayWrapper( const uno::Any& aArray, sal_Bool bZeroBased ) : maArray( aArray ), mbZeroBased( bZeroBased )
{
}


sal_Bool SAL_CALL 
ScArrayWrapper::getIsZeroIndex() throw (css::uno::RuntimeException)
{
	return mbZeroBased;
}


css::uno::Any SAL_CALL 
ScArrayWrapper::getArray() throw (css::uno::RuntimeException)
{
	return maArray;
}

