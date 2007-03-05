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
		ScTabViewShell* getBestViewShell( css::uno::Reference< css::frame::XModel>& xModel ) ;
		ScDocShell* getDocShell( css::uno::Reference< css::frame::XModel>& xModel ) ;
		ScTabViewShell* getCurrentBestViewShell();
		SfxViewFrame* getCurrentViewFrame();
		sal_Int32 OORGBToXLRGB( sal_Int32 );
		sal_Int32 XLRGBToOORGB( sal_Int32 );
        sal_Int32 pt2mm( double pt ); //1/100mm
        double mm2pt( sal_Int32 mm );
		css::uno::Any OORGBToXLRGB( const css::uno::Any& );
		css::uno::Any XLRGBToOORGB( const css::uno::Any& );
		// provide a NULL object that can be passed as variant so that 
		// the object when passed to IsNull will return true. aNULL 
		// contains an empty object reference
		const css::uno::Any& aNULL();
		void PrintOutHelper( const css::uno::Any& From, const css::uno::Any& To, const css::uno::Any& Copies, const css::uno::Any& Preview, const css::uno::Any& ActivePrinter, const css::uno::Any& PrintToFile, const css::uno::Any& Collate, const css::uno::Any& PrToFileName, css::uno::Reference< css::frame::XModel >& xModel, sal_Bool bSelection  );
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
