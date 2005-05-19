#include <cppuhelper/queryinterface.hxx>

#include "vbaglobals.hxx"
#include "vbaapplication.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

class ScVbaGlobalsImpl
{
	public:
		uno::Reference< vba::XGlobals >   mxGlobals;
		uno::Reference< vba::XApplication > mxApplication;

		ScVbaGlobalsImpl( ScVbaGlobals *pGlobals )
		{
			mxGlobals = uno::Reference< vba::XGlobals > ( pGlobals );
			mxApplication = uno::Reference< vba::XApplication > ( new ScVbaApplication() );
		}		

		uno::Sequence< uno::Any > getGlobals()
		{
			uno::Sequence< uno::Any > maGlobals(3);
			maGlobals[0] <<= mxGlobals;
			maGlobals[1] <<= mxApplication;
			maGlobals[2] <<= mxApplication->getActiveWorkbook();
			return maGlobals;
		}
};

ScVbaGlobals::ScVbaGlobals( )
{
	mnRefCount = 1;
	pImpl = new ScVbaGlobalsImpl( this );
}

// XInterface implementation
uno::Any ScVbaGlobals::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XGlobals * >( this ) ),
			 static_cast< vba::XGlobals * >( this ) );
}

uno::Sequence< uno::Any > ScVbaGlobals::getGlobals()
{
	static ScVbaGlobals *pGlobals = NULL;

	if( !pGlobals )
		pGlobals = new ScVbaGlobals();

	return pGlobals->pImpl->getGlobals();
}

uno::Reference<vba::XApplication >
ScVbaGlobals::getApplication() throw (uno::RuntimeException)
{
	return pImpl->mxApplication;
}
