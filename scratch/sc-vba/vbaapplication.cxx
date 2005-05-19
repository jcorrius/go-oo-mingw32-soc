#include <cppuhelper/queryinterface.hxx>
#include <sfx2/objsh.hxx>

#include "vbaapplication.hxx"
#include "vbaworkbook.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaApplication::ScVbaApplication()
{
	mnRefCount = 1;
}

ScVbaApplication::~ScVbaApplication()
{
}

// XInterface implementation
uno::Any ScVbaApplication::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XApplication * >( this ) ),
			 static_cast< vba::XApplication * >( this ) );
}

uno::Reference< vba::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
	SfxObjectShell* pDoc = SfxObjectShell::Current();
	uno::Reference< frame::XModel > xModel;

	if( pDoc )
		xModel = pDoc->GetModel();

	if( xModel.is() )
		return uno::Reference< vba::XWorkbook >(new ScVbaWorkbook( xModel ) );
	else
		return uno::Reference< vba::XWorkbook >( NULL );
}

uno::Reference< vba::XRange >
ScVbaApplication::getSelection() throw (uno::RuntimeException)
{
	fprintf( stderr, "GetSelection stub ...\n" );
	return uno::Reference< vba::XRange >( NULL );
}
