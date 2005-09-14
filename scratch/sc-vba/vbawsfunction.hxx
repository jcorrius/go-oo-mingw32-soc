#ifndef SC_VBA_INTERIOR_HXX
#define SC_VBA_INTERIOR_HXX

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <org/openoffice/vba/XWorksheetFunction.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "vbarange.hxx"

#include <com/sun/star/script/XInvocation.hpp>
#include <com/sun/star/sheet/XFunctionAccess.hpp>

using namespace ::org::openoffice;
using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::script;
using namespace com::sun::star::beans;
using namespace com::sun::star::reflection;
using namespace ::com::sun::star;

typedef ::cppu::WeakImplHelper2< org::openoffice::vba::XWorksheetFunction,XInvocation > ScVbaWSFunction_BASE;

class ScVbaWSFunction :  public ScVbaWSFunction_BASE
{
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
public:
	ScVbaWSFunction( uno::Reference< uno::XComponentContext >& xContext
				):m_xContext(xContext)
	{
	}
	virtual ~ScVbaWSFunction(){}

	virtual void SAL_CALL dummy(  ) throw (::com::sun::star::uno::RuntimeException);

	virtual Reference< XIntrospectionAccess >  SAL_CALL getIntrospection(void)  throw(RuntimeException);
	virtual Any  SAL_CALL invoke(const OUString& FunctionName, const Sequence< Any >& Params, Sequence< sal_Int16 >& OutParamIndex, Sequence< Any >& OutParam) throw(IllegalArgumentException, CannotConvertException, InvocationTargetException, RuntimeException);
	virtual void  SAL_CALL setValue(const OUString& PropertyName, const Any& Value) throw(UnknownPropertyException, CannotConvertException, InvocationTargetException, RuntimeException);
	virtual Any  SAL_CALL getValue(const OUString& PropertyName) throw(UnknownPropertyException, RuntimeException);
	virtual sal_Bool  SAL_CALL hasMethod(const OUString& Name)  throw(RuntimeException);
	virtual sal_Bool  SAL_CALL hasProperty(const OUString& Name)  throw(RuntimeException);

};
#endif

