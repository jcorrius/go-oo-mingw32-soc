#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/awt/Gradient.hpp>
#include <com/sun/star/awt/GradientStyle.hpp>
#include "vbafillformat.hxx"

using namespace org::openoffice;
using namespace com::sun::star;

ScVbaFillFormat::ScVbaFillFormat( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape ) : ScVbaFillFormat_BASE( xParent, xContext ), m_xShape( xShape )
{
    m_xPropertySet.set( xShape, uno::UNO_QUERY_THROW );
    m_nFillStyle = drawing::FillStyle_SOLID;
    m_nForeColor = 0;
    m_nBackColor = 0;
}

void
ScVbaFillFormat::setFillStyle( drawing::FillStyle nFillStyle ) throw (uno::RuntimeException)
{
    m_nFillStyle = nFillStyle;
    if( m_nFillStyle == drawing::FillStyle_GRADIENT )
    {
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii("FillStyle"), uno::makeAny( drawing::FillStyle_GRADIENT ) );
        awt::Gradient *aGradient = new awt::Gradient();
        // AXIAL
        // RADIAL
        // ELLIPTICAL
        // SQUARE
        // RECT
        aGradient->Style = awt::GradientStyle_LINEAR;
        //aGradient->StartColor = ForeColor().getRGB();
        //aGradient->EndColor = BackColor().getRGB();
        //aGradient->Angle = m_nGradientAngle;
        aGradient->Border = 0;
        aGradient->XOffset = 0;
        aGradient->YOffset = 0;
        aGradient->StartIntensity = 100;
        aGradient->EndIntensity = 100;
        aGradient->StepCount = 1;
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii("FillGradient"), uno::makeAny( *aGradient ) );
    }
    else if( m_nFillStyle == drawing::FillStyle_SOLID )
    {
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii("FillStyle"), uno::makeAny(drawing::FillStyle_SOLID) );
    }
}

void
ScVbaFillFormat::setForeColorAndInternalStyle( sal_Int32 nForeColor ) throw (css::uno::RuntimeException)
{
    m_nForeColor = nForeColor;
    setFillStyle( m_nFillStyle );
}

// Attributes
sal_Bool SAL_CALL 
ScVbaFillFormat::getVisible() throw (uno::RuntimeException)
{
    drawing::FillStyle nFillStyle;
    m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii("FillStyle") ) >>= nFillStyle;
    if( nFillStyle == drawing::FillStyle_NONE )
        return sal_False;
    return sal_True;
}

void SAL_CALL 
ScVbaFillFormat::setVisible( sal_Bool _visible ) throw (uno::RuntimeException)
{
    drawing::FillStyle aFillStyle;
    m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii("FillStyle") ) >>= aFillStyle;
    if( !_visible )
    {
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii("FillStyle"), uno::makeAny( drawing::FillStyle_NONE ) );
    }
    else
    {
        if( aFillStyle == drawing::FillStyle_NONE )
        {
            setFillStyle( m_nFillStyle );
        }
    }
}

double SAL_CALL 
ScVbaFillFormat::getTransparency() throw (uno::RuntimeException)
{
    return double(0);
}

void SAL_CALL 
ScVbaFillFormat::setTransparency( double _transparency ) throw (uno::RuntimeException)
{
}


// Methods
void SAL_CALL 
ScVbaFillFormat::Solid() throw (uno::RuntimeException)
{
}

void SAL_CALL 
ScVbaFillFormat::TwoColorGradient( sal_Int32 style, sal_Int32 variant ) throw (uno::RuntimeException)
{
}

uno::Reference< msforms::XColorFormat > SAL_CALL 
ScVbaFillFormat::BackColor() throw (uno::RuntimeException)
{
    return uno::Reference< msforms::XColorFormat >();
}

uno::Reference< msforms::XColorFormat > SAL_CALL 
ScVbaFillFormat::ForeColor() throw (uno::RuntimeException)
{
    return uno::Reference< msforms::XColorFormat >();
}


rtl::OUString&
ScVbaFillFormat::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaFillFormat") );
    return sImplName;
}

uno::Sequence< rtl::OUString >
ScVbaFillFormat::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msforms.FillFormat" ) );
    }
    return aServiceNames;
}

