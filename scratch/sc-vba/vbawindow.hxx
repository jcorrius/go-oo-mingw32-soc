#ifndef SC_VBA_WINDOW_HXX
#define SC_VBA_WINDOW_HXX
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XWindow.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1<oo::vba::XWindow > WindowImpl_BASE;

class ScVbaWindow : public WindowImpl_BASE
{		
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	void  Scroll( const css::uno::Any& Down, const css::uno::Any& Up, const css::uno::Any& ToRight, const css::uno::Any& ToLeft, bool bLargeScroll = false ) throw (css::uno::RuntimeException);	
public:
	ScVbaWindow( css::uno::Reference< css::uno::XComponentContext >& xContext ) : m_xContext(xContext) {}
	//XWindow
	virtual void SAL_CALL SmallScroll( const css::uno::Any& Down, const css::uno::Any& Up, const css::uno::Any& ToRight, const css::uno::Any& ToLeft ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL LargeScroll( const css::uno::Any& Down, const css::uno::Any& Up, const css::uno::Any& ToRight, const css::uno::Any& ToLeft ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL SelectedSheets( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException); 
	virtual void SAL_CALL ScrollWorkbookTabs( const css::uno::Any& Sheets, const css::uno::Any& Position ) throw (css::uno::RuntimeException);
};

#endif //SC_VBA_WINDOW_HXX
