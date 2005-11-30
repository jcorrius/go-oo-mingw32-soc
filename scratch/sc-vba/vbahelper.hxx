#ifndef SC_VBA_HELPER_HXX
#define SC_VBA_HELPER_HXX

#include <com/sun/star/frame/XModel.hpp>
#include <docsh.hxx>

#define css com::sun::star
#define oo org::openoffice
namespace org 
{
	namespace openoffice 
	{
		void dispatchRequests (css::uno::Reference< css::frame::XModel>& xModel,rtl::OUString & aUrl) ;
		void dispatchRequests (css::uno::Reference< css::frame::XModel>& xModel,rtl::OUString & aUrl, css::uno::Sequence< css::beans::PropertyValue >& sProps ) ;
		void implnCopy();
		void implnPaste();
		void implnCut();
		void implnPasteSpecial(sal_uInt16 nFlags,sal_uInt16 nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose);
		bool isRangeShortCut( const ::rtl::OUString& sParam );
		css::uno::Reference< css::frame::XModel >
			getCurrentDocument() throw (css::uno::RuntimeException);
		ScTabViewShell* getCurrentBestViewShell();
		SfxViewFrame* getCurrentViewFrame();
	}
}

#ifdef DEBUG
#  define SC_VBA_FIXME(a) OSL_TRACE( a )
#  define SC_VBA_STUB() SC_VBA_FIXME(( "%s - stubbed\n", __FUNCTION__ ))
#else
#  define SC_VBA_FIXME(a)
#  define SC_VBA_STUB()
#endif

#endif
