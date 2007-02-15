#include <comphelper/processfactory.hxx>

#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <org/openoffice/excel/XApplication.hpp>
#include <org/openoffice/excel/XDialog.hpp>

#include <tools/string.hxx>

#include "vbahelper.hxx"
#include "vbadialogs.hxx"
#include "vbaglobals.hxx"
#include "vbadialog.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

void
ScVbaDialogs::Dummy() throw (uno::RuntimeException)
{
}

uno::Any
ScVbaDialogs::getParent() throw (uno::RuntimeException)
{
	uno::Reference< vba::XGlobals > xGlobals = ScVbaGlobals::getGlobalsImpl( m_xContext );
	uno::Reference< excel::XApplication > xApplication = xGlobals->getApplication();
	if ( !xApplication.is() )
	{
		throw uno::RuntimeException(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScVbaWorkbooks::getParent: Couldn't access Application				object") ),uno::Reference< XInterface >() );
	}
	return uno::Any(xApplication);
}
                                                                                                                             
::sal_Int32
ScVbaDialogs::getCreator() throw (uno::RuntimeException)
{
	// #FIXME #TODO
	return 0;
}
uno::Reference< excel::XApplication >
ScVbaDialogs::getApplication() throw (uno::RuntimeException)
{
	uno::Reference< excel::XApplication > xApplication =  ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
	if ( !xApplication.is() )
	{
		throw uno::RuntimeException(
		::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScVbaWorkbooks::getParent: Couldn't access Application object" ) ),
		uno::Reference< XInterface >() );
	}
	return xApplication;
}

::sal_Int32
ScVbaDialogs::getCount() throw (uno::RuntimeException)
{
	//#TODO #FIXEME
	//SC_VBA_STUB();
	return 0;
}

uno::Any
ScVbaDialogs::Item( const uno::Any &aItem ) throw (uno::RuntimeException)
{
	sal_Int32 nIndex;
	aItem >>= nIndex;
	uno::Reference< excel::XDialog > aDialog( new ScVbaDialog( nIndex, m_xContext ) );
	return uno::Any( aDialog );
}
