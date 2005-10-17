#ifndef SC_VBA_INTERIOR_HXX
#define SC_VBA_INTERIOR_HXX

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <org/openoffice/vba/XInterior.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "vbarange.hxx"

#include <com/sun/star/script/XInvocation.hpp>


typedef ::cppu::WeakImplHelper2< oo::vba::XInterior,css::script::XInvocation > ScVbaInterior_BASE;

class ScVbaInterior :  public ScVbaInterior_BASE
{
	css::uno::Reference< css::table::XCellRange > mxRange;
        css::uno::Reference< css::uno::XComponentContext > m_xContext;
	css::uno::Reference< css::beans::XIntrospectionAccess> m_xAcc;

public:
        ScVbaInterior( css::uno::Reference< css::uno::XComponentContext >& xContext,
                 css::uno::Reference< css::table::XCellRange > range) throw ( css::lang::IllegalArgumentException);

        virtual ~ScVbaInterior(){}

	virtual css::uno::Any SAL_CALL getColor() throw (css::uno::RuntimeException) ;
	virtual void SAL_CALL setColor( const css::uno::Any& _color ) throw (css::uno::RuntimeException) ;

	virtual css::uno::Reference< css::beans::XIntrospectionAccess >  SAL_CALL getIntrospection(void)  throw(css::uno::RuntimeException);
	virtual css::uno::Any  SAL_CALL invoke(const rtl::OUString& FunctionName, const css::uno::Sequence< css::uno::Any >& Params, css::uno::Sequence< sal_Int16 >& OutParamIndex, css::uno::Sequence< css::uno::Any >& OutParam) throw(css::lang::IllegalArgumentException, css::script::CannotConvertException, css::reflection::InvocationTargetException, css::uno::RuntimeException);
	virtual void  SAL_CALL setValue(const rtl::OUString& PropertyName, const css::uno::Any& Value) throw(css::beans::UnknownPropertyException, css::script::CannotConvertException, css::reflection::InvocationTargetException, css::uno::RuntimeException);
	virtual css::uno::Any  SAL_CALL getValue(const rtl::OUString& PropertyName) throw(css::beans::UnknownPropertyException, css::uno::RuntimeException);
	virtual sal_Bool  SAL_CALL hasMethod(const rtl::OUString& Name)  throw(css::uno::RuntimeException);
	virtual sal_Bool  SAL_CALL hasProperty(const rtl::OUString& Name)  throw(css::uno::RuntimeException);

};
#endif

