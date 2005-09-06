#ifndef SC_VBA_INTERIOR_HXX
#define SC_VBA_INTERIOR_HXX

#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XInterior.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "vbarange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XInterior > ScVbaInterior_BASE;

class ScVbaInterior :  public ScVbaInterior_BASE
{
	uno::Reference< table::XCellRange > mxRange;
        uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
public:
        ScVbaInterior( uno::Reference< uno::XComponentContext >& xContext,
                 uno::Reference< table::XCellRange > range):m_xContext(xContext),mxRange(range)
        {}
        virtual ~ScVbaInterior(){}

    virtual ::com::sun::star::uno::Any SAL_CALL getColor() throw (::com::sun::star::uno::RuntimeException) ;
    virtual void SAL_CALL setColor( const ::com::sun::star::uno::Any& _color ) throw (::com::sun::star::uno::RuntimeException) ;
};
#endif

