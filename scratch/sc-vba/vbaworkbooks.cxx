#include <cppuhelper/queryinterface.hxx>

#include "vbaworkbooks.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XInterface implementation
uno::Any ScVbaWorkbooks::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XWorkbooks * >( this ) ),
			 static_cast< vba::XCollection * >( this ),
			 static_cast< vba::XWorkbooks * >( this ) );
}

// XCollection
::sal_Int32
ScVbaWorkbooks::getCount() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return 0;
}
uno::Any
ScVbaWorkbooks::getParent() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return uno::Any();
}
::sal_Int32
ScVbaWorkbooks::getCreator() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return 0;
}
uno::Reference< vba::XApplication >
ScVbaWorkbooks::getApplication() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return uno::Reference< vba::XApplication >( NULL );
}
uno::Any
ScVbaWorkbooks::Item( ::sal_Int32 Index ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return uno::Any();
}

// XWorkbooks
uno::Any
ScVbaWorkbooks::Add( const uno::Any& Before, const uno::Any& After,
					 const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return uno::Any();
}
void
ScVbaWorkbooks::Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
void
ScVbaWorkbooks::Move( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
void
ScVbaWorkbooks::Delete() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
