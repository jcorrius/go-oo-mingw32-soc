#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/drawing/TextFitToSizeType.hpp>
#include "vbatextframe.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;


ScVbaTextFrame::ScVbaTextFrame( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, uno::Reference< drawing::XShape > xShape ) : ScVbaTextFrame_BASE( xParent, xContext ), m_xShape( xShape )
{
}

// Attributes
sal_Bool SAL_CALL 
ScVbaTextFrame::getAutoSize() throw (uno::RuntimeException)
{
    sal_Bool autosize = sal_False;
    uno::Reference< beans::XPropertySet > xPropertySet( m_xShape, uno::UNO_QUERY_THROW );
    com::sun::star::drawing::TextFitToSizeType eTextFitToSizeType;
    uno::Any aTextFitToSize = xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "TextFitToSize" ) );
    aTextFitToSize >>= autosize;
    return autosize;
}

void SAL_CALL 
ScVbaTextFrame::setAutoSize( sal_Bool _autosize ) throw (uno::RuntimeException)
{
}

float SAL_CALL 
ScVbaTextFrame::getMarginBottom() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaTextFrame::setMarginBottom( float _marginbottom ) throw (uno::RuntimeException)
{
}

float SAL_CALL 
ScVbaTextFrame::getMarginTop() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaTextFrame::setMarginTop( float _margintop ) throw (uno::RuntimeException)
{
}

float SAL_CALL 
ScVbaTextFrame::getMarginLeft() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaTextFrame::setMarginLeft( float _marginleft ) throw (uno::RuntimeException)
{
}

float SAL_CALL 
ScVbaTextFrame::getMarginRight() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaTextFrame::setMarginRight( float _marginright ) throw (uno::RuntimeException)
{
}


// Methods
uno::Reference< oo::excel::XCharacters > SAL_CALL 
ScVbaTextFrame::Characters(  ) throw (uno::RuntimeException)
{
    return uno::Reference< oo::excel::XCharacters >();
}

