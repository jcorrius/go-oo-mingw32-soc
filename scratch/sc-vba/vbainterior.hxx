#ifndef SC_VBA_INTERIOR_HXX
#define SC_VBA_INTERIOR_HXX

#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XInterior.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include "vbarange.hxx"

#include <com/sun/star/script/XInvocation.hpp>

class ScDocument;

typedef ::cppu::WeakImplHelper1< oo::vba::XInterior > ScVbaInterior_BASE;

class ScVbaInterior :  public ScVbaInterior_BASE
{
	css::uno::Reference< css::beans::XPropertySet > m_xProps;
        css::uno::Reference< css::uno::XComponentContext > m_xContext;
	ScDocument* m_pScDoc;

        css::uno::Reference< css::container::XIndexAccess > getPalette(); 
public:
        ScVbaInterior( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                 const css::uno::Reference< css::beans::XPropertySet >& xProps, ScDocument* pScDoc ) throw ( css::lang::IllegalArgumentException);

        virtual ~ScVbaInterior(){}

	virtual css::uno::Any SAL_CALL getColor() throw (css::uno::RuntimeException) ;
	virtual void SAL_CALL setColor( const css::uno::Any& _color ) throw (css::uno::RuntimeException) ;

	virtual css::uno::Any SAL_CALL getColorIndex() throw ( css::uno::RuntimeException);
	virtual void SAL_CALL setColorIndex( const css::uno::Any& _colorindex ) throw ( css::uno::RuntimeException );
};
#endif

