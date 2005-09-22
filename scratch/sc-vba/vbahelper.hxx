#ifndef SC_VBA_HELPER_HXX
#define SC_VBA_HELPER_HXX

#include <com/sun/star/frame/XModel.hpp>

using namespace ::com::sun::star;

namespace org 
{
	namespace openoffice 
	{
		void dispatchRequests (::com::sun::star::uno::Reference< frame::XModel>& xModel,rtl::OUString & aUrl) ;
		void implnCopy();
		void implnPaste();
		void implnCut();
		void implnPasteSpecial(sal_uInt16 nFlags,sal_uInt16 nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose);
	}
}

using namespace ::org::openoffice;

#ifdef DEBUG
#  define SC_VBA_FIXME(a) OSL_TRACE( a )
#  define SC_VBA_STUB() SC_VBA_FIXME(( "%s - stubbed\n", __FUNCTION__ ))
#else
#  define SC_VBA_FIXME(a)
#  define SC_VBA_STUB()
#endif

#endif
