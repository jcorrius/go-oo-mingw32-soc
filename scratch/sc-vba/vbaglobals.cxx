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
			sal_uInt32 nMax = 0;
			uno::Sequence< uno::Any > maGlobals(4);
			maGlobals[ nMax++ ] <<= mxGlobals;
			maGlobals[ nMax++ ] <<= mxApplication;

			uno::Reference< vba::XWorkbook > xWorkbook = mxApplication->getActiveWorkbook();
			if( xWorkbook.is() )
			{
				maGlobals[ nMax++ ] <<= xWorkbook;
				uno::Reference< vba::XWorksheet > xWorksheet = xWorkbook->getActiveSheet();
				if( xWorksheet.is() )
					maGlobals[ nMax++ ] <<= xWorksheet;
			}
			maGlobals.realloc( nMax );
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
