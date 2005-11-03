#include "vbawindow.hxx"
#include <sc.hrc>

using namespace ::com::sun::star;
using namespace ::org::openoffice;

void  
ScVbaWindow::Scroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft, bool bLargeScroll ) throw (uno::RuntimeException)
{
	sal_Int16 down = 0;	
	sal_Int16 up = 0;	
	sal_Int16 toRight = 0;	
	sal_Int16 toLeft = 0;	
	Down >>= down;
	Up >>= up;
	ToRight >>= toRight;
	ToLeft >>= toLeft;
	uno::Sequence< beans::PropertyValue > args1(2);
	args1[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "By" ) );
	args1[1].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Sel" ) );
	args1[1].Value <<= false;
	
	sal_Int16 totalUp = up - down;
	sal_Int16 totalLeft = toLeft - toRight;
	
	if ( totalUp != 0 )
	{
		args1[0].Value <<= totalUp;
		rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:GoUp"));
		if ( bLargeScroll )
			url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:GoUpBlock"));
		uno::Reference< frame::XModel > xModel = getCurrentDocument();
		dispatchRequests( xModel, url, args1 );
	}
	
	if ( totalLeft != 0 )
	{
		args1[0].Value <<= totalLeft;
		rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:GoLeft"));
		if ( bLargeScroll )
			url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "slot:")) + rtl::OUString::valueOf( (sal_Int32)SID_CURSORPAGELEFT_ );
		uno::Reference< frame::XModel > xModel = getCurrentDocument();
		dispatchRequests( xModel, url, args1 );
	}

}
void SAL_CALL 
ScVbaWindow::SmallScroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft ) throw (uno::RuntimeException)
{
	Scroll( Down, Up, ToRight, ToLeft );
}
void SAL_CALL 
ScVbaWindow::LargeScroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft ) throw (uno::RuntimeException)
{
	Scroll( Down, Up, ToRight, ToLeft, true );
}
