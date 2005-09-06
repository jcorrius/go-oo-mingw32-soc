#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>

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
	long nValue;
        long nRowCount, nColCount;
        rtl::OUString aString;

        uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, uno::UNO_QUERY);
        nRowCount = xColumnRowRange->getRows()->getCount();
        nColCount = xColumnRowRange->getColumns()->getCount();

        if ( _color >>= nValue)
        {
                for (long i = 0;i < nRowCount; i++)
                {
                        for (long j = 0;j < nColCount; j++)
                        {
                                uno::Reference< table::XCell > xCell = mxRange->getCellByPosition(j, i );
				uno::Reference< beans::XPropertySet > xProps(xCell, uno::UNO_QUERY );
				xProps->setPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) ), _color);
                        }
                }
        }

}

