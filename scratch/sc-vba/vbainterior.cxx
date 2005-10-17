#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/beans/XIntrospection.hpp>
#include <com/sun/star/beans/XIntrospectionAccess.hpp>
#include <com/sun/star/reflection/XIdlMethod.hpp>
#include <com/sun/star/beans/MethodConcept.hpp>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/queryinterface.hxx>

#include "vbainterior.hxx"
using namespace ::com::sun::star;
using namespace ::org::openoffice;

ScVbaInterior::ScVbaInterior( uno::Reference< uno::XComponentContext >& xContext, uno::Reference< css::table::XCellRange > range) throw ( lang::IllegalArgumentException) : m_xContext(xContext),mxRange(range)
{
	if ( !xContext.is() )
		throw lang::IllegalArgumentException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "context not set" ) ), uno::Reference< uno::XInterface >(), 1 ); 
	if ( !mxRange.is() )
		throw lang::IllegalArgumentException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "range not set") ), uno::Reference< uno::XInterface >(), 2 ); 
}

uno::Any
ScVbaInterior::getColor() throw (uno::RuntimeException) 
{
	uno::Any aAny;
	uno::Reference< table::XCell > xCell = mxRange->getCellByPosition(0,0 );
	uno::Reference< beans::XPropertySet > xProps(xCell, uno::UNO_QUERY_THROW );
	aAny = xProps->getPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ));
	return aAny;
}
 
void 
ScVbaInterior::setColor( const uno::Any& _color ) throw (uno::RuntimeException) 
{
	uno::Reference< beans::XPropertySet > xProps(mxRange, uno::UNO_QUERY_THROW );
	xProps->setPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ), _color);
}

uno::Reference< beans::XIntrospectionAccess >
ScVbaInterior::getIntrospection(void)  throw(uno::RuntimeException)
{
	return uno::Reference<beans::XIntrospectionAccess>();
}

uno::Any
ScVbaInterior::invoke(const rtl::OUString& FunctionName, const uno::Sequence< uno::Any >& Params, uno::Sequence< sal_Int16 >& OutParamIndex, uno::Sequence< uno::Any >& OutParam) throw( lang::IllegalArgumentException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
	uno::Any aAny;
	return aAny;
}

void
ScVbaInterior::setValue(const rtl::OUString& PropertyName, const uno::Any& Value) throw(beans::UnknownPropertyException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps(mxRange, uno::UNO_QUERY_THROW );
	if (PropertyName.equalsAscii("RotateAngle")) 
	{
		xProps->setPropertyValue(PropertyName,Value);
	}
	else if (PropertyName.equalsAscii("CellStyle")) 
	{
		xProps->setPropertyValue(PropertyName,Value);
	}
	else if (PropertyName.equalsAscii("ColorIndex")) 
	{
	//FIXME:Need to add constants as parameter
		xProps->setPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ),Value);		
	}
	else if (PropertyName.equalsAscii("Color")) 
	{
		setColor( Value );
	}
	else
	{
		rtl::OUString msg = rtl::OUString::createFromAscii("No property named ");	
	 	msg += PropertyName;	
		throw beans::UnknownPropertyException( msg, uno::Reference< uno::XInterface >() );
	}
}

uno::Any
ScVbaInterior::getValue(const rtl::OUString& PropertyName) throw(beans::UnknownPropertyException, uno::RuntimeException)
{
	uno::Any aAny;
	uno::Reference< beans::XPropertySet > xProps(mxRange, uno::UNO_QUERY_THROW );
	if (PropertyName.equalsAscii("ColorIndex"))
	{
		aAny =  xProps->getPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ));
	}
	else if (PropertyName.equalsAscii("Color"))
	{
		aAny = getColor();
	}
	else
	{
		rtl::OUString msg = rtl::OUString::createFromAscii("No property named ");	
	 	msg += PropertyName;	
		throw beans::UnknownPropertyException( msg, uno::Reference< uno::XInterface >() );
                        
	}
	return aAny;
}

sal_Bool
ScVbaInterior::hasMethod(const rtl::OUString& Name)  throw(uno::RuntimeException)
{
	return false;
}

sal_Bool
ScVbaInterior::hasProperty(const rtl::OUString& Name)  throw(uno::RuntimeException)
{

	if (Name.equalsAscii("RotateAngle"))
		return true;
	if (Name.equalsAscii("CellStyle"))
		return true;
	if (Name.equalsAscii("ColorIndex"))
		return true;
	if (Name.equalsAscii("Color"))
		return true;

 return false;
}
