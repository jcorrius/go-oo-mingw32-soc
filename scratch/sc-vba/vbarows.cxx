#include <cppuhelper/queryinterface.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>

#include "vbarows.hxx"
#include "vbarange.hxx"
#include "vbaglobals.hxx"
#include "vbaapplication.hxx"

#include <tools/string.hxx>
using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XCollection
::sal_Int32
ScVbaRows::getCount() throw (uno::RuntimeException)
{
	return mxRows->getCount();
}

::sal_Int32
ScVbaRows::getCreator() throw (uno::RuntimeException)
{
    SC_VBA_STUB();
    return 0;
}
uno::Reference< vba::XApplication >
ScVbaRows::getApplication() throw (uno::RuntimeException)
{
    return ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
}
                                                                                                                             
uno::Any
ScVbaRows::getParent() throw (uno::RuntimeException)
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
ScVbaRows::Item( const uno::Any &aIndex ) throw (uno::RuntimeException )
{
	String aString;
	long nIndexNumber, nStartRow, nEndRow;
	uno::Reference< vba::XRange > aRange;
	if( aIndex >>= nIndexNumber )
		nIndexNumber--;
	uno::Reference< sheet::XCellRangeAddressable > xAddressable( mxRange, uno::UNO_QUERY_THROW );
	table::CellRangeAddress xAddress = xAddressable->getRangeAddress();
	long nEndColumn = xAddress.EndColumn - xAddress.StartColumn;
	aRange = new ScVbaRange( m_xContext,mxRange->getCellRangeByPosition(                                                                                  0, nIndexNumber,nEndColumn, nIndexNumber ) );
    return uno::Any( aRange );
} 	 
