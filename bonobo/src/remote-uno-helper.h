#ifndef REMOTE_UNO_HELPER_H
#define REMOTE_UNO_HELPER_H

#include <com/sun/star/uno/XComponentContext.hpp>

::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > getRemoteComponentContext( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xComponentContext );

#endif
