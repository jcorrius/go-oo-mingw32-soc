#include <cppuhelper/queryinterface.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>

#include "vbacolumns.hxx"
#include "vbarange.hxx"
#include "vbaglobals.hxx"
#include "vbaapplication.hxx"

#include <tools/string.hxx>
using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XCollection
::sal_Int32
ScVbaColumns::getCount() throw (uno::RuntimeException)
{
	return mxColumns->getCount();
}

::sal_Int32
ScVbaColumns::getCreator() throw (uno::RuntimeException)
{
    SC_VBA_STUB();
    return 0;
}
uno::Reference< vba::XApplication >
ScVbaColumns::getApplication() throw (uno::RuntimeException)
{
    return ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
}
                                                                                                                             
uno::Any
ScVbaColumns::getParent() throw (uno::RuntimeException)
{
    uno::Reference< vba::XGlobals > xGlobals = ScVbaGlobals::getGlobalsImpl( m_xContext );
    uno::Reference< vba::XApplication > xApplication =
        xGlobals->getApplication();
    if ( !xApplication.is() )
    {
        throw uno::RuntimeException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScVbaWorkbooks::getParent: Couldn't access Application object" ) ),
                    uno::Reference< XInterface >() );
    }
    return uno::Any(xApplication);
}

uno::Any
ScVbaColumns::Item( const uno::Any &aIndex ) throw (uno::RuntimeException )
{
	String aString;
	long nIndexNumber;
	if( aIndex >>= nIndexNumber )
		nIndexNumber--;
	uno::Reference< sheet::XCellRangeAddressable > xAddressable( mxRange, uno::UNO_QUERY_THROW );
	table::CellRangeAddress xAddress = xAddressable->getRangeAddress();
	long nEndRow = xAddress.EndRow - xAddress.StartRow;
	uno::Reference< vba::XRange > aRange = new ScVbaRange( m_xContext,mxRange->getCellRangeByPosition(                                                                                  nIndexNumber, 0, nIndexNumber, nEndRow ) );
    return uno::Any( aRange );
} 	 
