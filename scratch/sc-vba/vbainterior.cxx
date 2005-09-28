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

::com::sun::star::uno::Any
ScVbaInterior::getColor() throw (::com::sun::star::uno::RuntimeException) 
{
	uno::Any aAny;
	uno::Reference< table::XCell > xCell = mxRange->getCellByPosition(0,0 );
	uno::Reference< beans::XPropertySet > xProps(xCell, uno::UNO_QUERY );
	aAny = xProps->getPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ));
	return aAny;
}
 
void 
ScVbaInterior::setColor( const ::com::sun::star::uno::Any& _color ) throw (::com::sun::star::uno::RuntimeException) 
{
	uno::Reference< beans::XPropertySet > xProps(mxRange, uno::UNO_QUERY );
	xProps->setPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ), _color);
}

Reference< XIntrospectionAccess >
ScVbaInterior::getIntrospection(void)  throw(RuntimeException)
{
	return uno::Reference<beans::XIntrospectionAccess>();
}

Any
ScVbaInterior::invoke(const OUString& FunctionName, const Sequence< Any >& Params, Sequence< sal_Int16 >& OutParamIndex, Sequence< Any >& OutParam) throw(IllegalArgumentException, CannotConvertException, InvocationTargetException, RuntimeException)
{
	uno::Any aAny;
	return aAny;
}

void
ScVbaInterior::setValue(const OUString& PropertyName, const Any& Value) throw(UnknownPropertyException, CannotConvertException, InvocationTargetException, RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps(mxRange, uno::UNO_QUERY );

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
		throw UnknownPropertyException( msg, uno::Reference< uno::XInterface >() );
	}
}

Any
ScVbaInterior::getValue(const OUString& PropertyName) throw(UnknownPropertyException, RuntimeException)
{
	uno::Any aAny;
	uno::Reference< beans::XPropertySet > xProps(mxRange, uno::UNO_QUERY );
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
		throw UnknownPropertyException( msg, uno::Reference< uno::XInterface >() );
                        
	}
	return aAny;
}

sal_Bool
ScVbaInterior::hasMethod(const OUString& Name)  throw(RuntimeException)
{
	return false;
}

sal_Bool
ScVbaInterior::hasProperty(const OUString& Name)  throw(RuntimeException)
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
