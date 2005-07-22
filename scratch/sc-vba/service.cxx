#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"

// =============================================================================
// component exports
// =============================================================================
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace vbaobj
{
    // =============================================================================
    // component operations
    // =============================================================================

    uno::Reference< XInterface > SAL_CALL create(
        Reference< XComponentContext > const & xContext )
        SAL_THROW( () );

    // -----------------------------------------------------------------------------

    ::rtl::OUString SAL_CALL getImplementationName();

	Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	Reference<XInterface> SAL_CALL create(
		Sequence<Any> const &, Reference<XComponentContext> const & );
} // end namespace vbaobj


    // =============================================================================

    const ::cppu::ImplementationEntry s_component_entries [] =
    {
        {
            ::vbaobj::create, ::vbaobj::getImplementationName,
            ::vbaobj::getSupportedServiceNames, 
		::cppu::createSingleComponentFactory,
            0, 0
        },
        { 0, 0, 0, 0, 0, 0 }
    };

extern "C"
{
    void SAL_CALL component_getImplementationEnvironment( 
        const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv )
    {
		OSL_TRACE("In component_getImplementationEnv");
        *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
    }

    sal_Bool SAL_CALL component_writeInfo( 
        lang::XMultiServiceFactory * pServiceManager, registry::XRegistryKey * pRegistryKey )
    {
		OSL_TRACE("In component_writeInfo");
        if ( ::cppu::component_writeInfoHelper(
            pServiceManager, pRegistryKey, s_component_entries ) )
		{
			try
			{
				registry::XRegistryKey * pKey =
					reinterpret_cast< registry::XRegistryKey * >(pRegistryKey);

				Reference< registry::XRegistryKey >xKey = pKey->createKey(
					rtl::OUString::createFromAscii( ("org.openoffice.vba.Globals/UNO/SINGLETONS/org.openoffice.vba.theGlobals") ) );
				xKey->setStringValue( ::rtl::OUString::createFromAscii( 
					("org.openoffice.vba.Globals") ) );
				return sal_True;
			}
			catch( uno::Exception& e )
			{
				//recomp & friends will detect false returned and fail
			}
		}
		return sal_False;
    }

    void * SAL_CALL component_getFactory( 
        const sal_Char * pImplName, lang::XMultiServiceFactory * pServiceManager,
        registry::XRegistryKey * pRegistryKey )
    {
		OSL_TRACE("In component_getFactory");
        return ::cppu::component_getFactoryHelper( 
            pImplName, pServiceManager, pRegistryKey, s_component_entries );
    }
}
