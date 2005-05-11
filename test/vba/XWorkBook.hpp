#ifndef _ORG_OPENOFFICE_VBA_XWORKBOOK_HPP_
#define _ORG_OPENOFFICE_VBA_XWORKBOOK_HPP_

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif

#ifndef _ORG_OPENOFFICE_VBA_XWORKBOOK_HDL_
#include "org/openoffice/vba/XWorkBook.hdl"
#endif

#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include "com/sun/star/uno/RuntimeException.hpp"
#endif
#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include "com/sun/star/uno/XInterface.hpp"
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include "com/sun/star/uno/Reference.hxx"
#endif
#ifndef _COM_SUN_STAR_UNO_TYPE_HXX_
#include "com/sun/star/uno/Type.hxx"
#endif
#ifndef _OSL_MUTEX_HXX_
#include "osl/mutex.hxx"
#endif
#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif
#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

inline const ::com::sun::star::uno::Type& SAL_CALL getCppuType( const ::com::sun::star::uno::Reference< ::org::openoffice::vba::XWorkBook >* ) SAL_THROW( () )
{
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_static_mi_interface_type_init( &the_type, "org.openoffice.vba.XWorkBook", 0, 0 );
    }
    return * reinterpret_cast< ::com::sun::star::uno::Type * >( &the_type );
}

::com::sun::star::uno::Type const & ::org::openoffice::vba::XWorkBook::static_type(void *) {
    return ::getCppuType(static_cast< ::com::sun::star::uno::Reference< ::org::openoffice::vba::XWorkBook > * >(0));
}

#endif // _ORG_OPENOFFICE_VBA_XWORKBOOK_HPP_
