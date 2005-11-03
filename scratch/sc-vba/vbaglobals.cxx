#ifndef SC_VBA_GLOBALS
#include "vbaglobals.hxx"
#endif

#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include "vbaapplication.hxx"
#include "vbaworksheet.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::org::openoffice;



namespace vbaobj
{
    ::rtl::OUString SAL_CALL getImplementationName()
    {
        static ::rtl::OUString* pImplName = 0;
		if ( !pImplName )
		{
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if ( !pImplName )
			{
				static ::rtl::OUString aImplName( RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.vba.Globals" ) );
				pImplName = &aImplName;
			}
		}
		return *pImplName;
    }

    uno::Reference< XInterface > SAL_CALL create(
        Reference< XComponentContext > const & xContext )
        SAL_THROW( () )
    {
		OSL_TRACE("In create component for vbaglobals");
        return static_cast< lang::XTypeProvider * >( new ScVbaGlobals( xContext ) );
    }

	Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
	{
    	const ::rtl::OUString strName( ::vbaobj::getImplementationName() );
		return Sequence< ::rtl::OUString >( &strName, 1 );
	}	
}

// =============================================================================
// ScVbaGlobals
// =============================================================================

ScVbaGlobals::ScVbaGlobals( const Reference< XComponentContext >& rxContext )
        :m_xContext( rxContext )
{
//	OSL_TRACE("ScVbaGlobals::ScVbaGlobals()");
	mxApplication = uno::Reference< vba::XApplication > ( new ScVbaApplication( m_xContext) );
}

ScVbaGlobals::~ScVbaGlobals()
{
//	OSL_TRACE("ScVbaGlobals::~ScVbaGlobals");
}

// Will throw if singleton can't be accessed 
uno::Reference< vba::XGlobals >
ScVbaGlobals::getGlobalsImpl( uno::Reference< uno::XComponentContext >& xContext ) throw ( uno::RuntimeException )
{
	uno::Reference< vba::XGlobals > xGlobals( 
		xContext->getValueByName( ::rtl::OUString::createFromAscii( 
			"/singletons/org.openoffice.vba.theGlobals") ), uno::UNO_QUERY);

	if ( !xGlobals.is() )
	{
		throw uno::RuntimeException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": Couldn't access Globals" ) ),
                    uno::Reference< XInterface >() );
	}
	return xGlobals;
}

// -----------------------------------------------------------------------------
// XServiceInfo
// -----------------------------------------------------------------------------

::rtl::OUString ScVbaGlobals::getImplementationName(  ) throw (RuntimeException)
{
	return ::vbaobj::getImplementationName();
}

sal_Bool ScVbaGlobals::supportsService( const ::rtl::OUString& rServiceName ) throw (RuntimeException)
{
	Sequence< ::rtl::OUString > aNames( ::vbaobj::getSupportedServiceNames() );
	const ::rtl::OUString* pNames = aNames.getConstArray();
	const ::rtl::OUString* pEnd = pNames + aNames.getLength();
	for ( ; pNames != pEnd && !pNames->equals( rServiceName ); ++pNames )
		;

	return pNames != pEnd;
}

Sequence< ::rtl::OUString > ScVbaGlobals::getSupportedServiceNames(  ) throw (RuntimeException)
{
	return vbaobj::getSupportedServiceNames();
}
	
// =============================================================================
// XGlobals
// =============================================================================
uno::Reference<vba::XApplication >
ScVbaGlobals::getApplication() throw (uno::RuntimeException)
{
//	OSL_TRACE("In ScVbaGlobals::getApplication");	
    return mxApplication;
}

uno::Reference< vba::XWorkbook > SAL_CALL 
ScVbaGlobals::getActiveWorkbook() throw (uno::RuntimeException)
{
//	OSL_TRACE("In ScVbaGlobals::getActiveWorkbook");	
    uno::Reference< vba::XWorkbook > xWorkbook( mxApplication->getActiveWorkbook(), uno::UNO_QUERY);
    if ( xWorkbook.is() )
    {
        return xWorkbook;    
    }
// FIXME check if this is correct/desired behavior
    throw uno::RuntimeException( rtl::OUString::createFromAscii(
        "No activeWorkbook available" ), Reference< uno::XInterface >() );
}


uno::Reference< vba::XWorksheet > SAL_CALL 
ScVbaGlobals::getActiveSheet() throw (uno::RuntimeException)
{
//	OSL_TRACE("ScVbaGlobals::getActiveSheet()");
    uno::Reference< vba::XWorksheet > result;
    uno::Reference< vba::XWorkbook > xWorkbook( 
        mxApplication->getActiveWorkbook(), uno::UNO_QUERY );
    if ( xWorkbook.is() )
    {
        uno::Reference< vba::XWorksheet > xWorksheet( 
            xWorkbook->getActiveSheet(), uno::UNO_QUERY );
        if ( xWorksheet.is() )
        {
            result = xWorksheet;
        }	
    }

    if ( !result.is() )
    {
        // Fixme - check if this is reasonable/desired behavior
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
            "No activeSheet available" ), Reference< uno::XInterface >() );
    }
    return result;
}

uno::Any SAL_CALL 
ScVbaGlobals::WorkBooks( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	return uno::Any( mxApplication->Workbooks(aIndex) );
}

uno::Any SAL_CALL
ScVbaGlobals::WorkSheets(const uno::Any& aIndex) throw (uno::RuntimeException)
{
//	OSL_TRACE("ScVbaGlobals::getWorkSheets()");
    uno::Reference< vba::XWorkbook > xWorkbook( mxApplication->getActiveWorkbook(), uno::UNO_QUERY );
        uno::Any result;
    if ( xWorkbook.is() )
        result  = xWorkbook->Worksheets( aIndex );

    else
        // Fixme - check if this is reasonable/desired behavior
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
            "No ActiveWorkBook available" ), Reference< uno::XInterface >() );

	return result;
}
uno::Any SAL_CALL
ScVbaGlobals::Sheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	return WorkSheets( aIndex );
}

::uno::Sequence< ::uno::Any > SAL_CALL
ScVbaGlobals::getGlobals(  ) throw (::uno::RuntimeException)
{
	sal_uInt32 nMax = 0;
	uno::Sequence< uno::Any > maGlobals(4);
	maGlobals[ nMax++ ] <<= ScVbaGlobals::getGlobalsImpl(m_xContext);
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
