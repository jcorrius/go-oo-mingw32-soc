#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/beans/XIntrospection.hpp>
#include <com/sun/star/beans/XIntrospectionAccess.hpp>
#include <com/sun/star/reflection/XIdlMethod.hpp>
#include <com/sun/star/beans/MethodConcept.hpp>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/queryinterface.hxx>

#include "vbawsfunction.hxx"

using namespace com::sun::star;
using namespace org::openoffice;

void SAL_CALL 
ScVbaWSFunction::dummy(  ) throw (uno::RuntimeException)
{
}

uno::Reference< beans::XIntrospectionAccess >
ScVbaWSFunction::getIntrospection(void)  throw(uno::RuntimeException)
{
	return uno::Reference<beans::XIntrospectionAccess>();
}

uno::Any SAL_CALL
ScVbaWSFunction::invoke(const rtl::OUString& FunctionName, const uno::Sequence< uno::Any >& Params, uno::Sequence< sal_Int16 >& OutParamIndex, uno::Sequence< uno::Any >& OutParam) throw(lang::IllegalArgumentException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
	uno::Reference< lang::XMultiComponentFactory > xSMgr( m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XFunctionAccess > xFunctionAccess(
                        xSMgr->createInstanceWithContext(::rtl::OUString::createFromAscii(
                        "com.sun.star.sheet.FunctionAccess"), m_xContext),
                        ::uno::UNO_QUERY_THROW);
	uno::Sequence< uno::Any > aParamTemp;
	sal_Int32 nParamCount = Params.getLength();
	aParamTemp.realloc(nParamCount);
	const uno::Any* aArray = Params.getConstArray();
	uno::Any* aArrayTemp = aParamTemp.getArray();

	for (int i=0; i < Params.getLength();i++) 
	{
		uno::Reference<vba::XRange> myRange( aArray[ i ], uno::UNO_QUERY_THROW );
		if ( myRange.is() ) 
			aArrayTemp[i] = myRange->getCellRange();
		else
			aArrayTemp[i]= aArray[i];
	}

	uno::Any aAny = xFunctionAccess->callFunction(FunctionName,aParamTemp);

	return aAny;
}

void SAL_CALL
ScVbaWSFunction::setValue(const rtl::OUString& PropertyName, const uno::Any& Value) throw(beans::UnknownPropertyException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
}

uno::Any SAL_CALL
ScVbaWSFunction::getValue(const rtl::OUString& PropertyName) throw(beans::UnknownPropertyException, uno::RuntimeException)
{
	return uno::Any();
}

sal_Bool SAL_CALL
ScVbaWSFunction::hasMethod(const rtl::OUString& Name)  throw(uno::RuntimeException)
{
	return true;
}

sal_Bool SAL_CALL
ScVbaWSFunction::hasProperty(const rtl::OUString& Name)  throw(uno::RuntimeException)
{
	 return false;
}
