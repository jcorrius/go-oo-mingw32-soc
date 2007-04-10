#include <com/sun/star/beans/XPropertySet.hpp>
#include <org/openoffice/msforms/XLineFormat.hpp>
#include "vbacolorformat.hxx"

using namespace org::openoffice;
using namespace com::sun::star;

ScVbaColorFormat::ScVbaColorFormat( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< vba::XHelperInterface > xInternalParent, const uno::Reference< drawing::XShape > xShape, const sal_Int16 nColorFormatType ) : ScVbaColorFormat_BASE( xParent, xContext ), m_xInternalParent( xInternalParent ), m_xShape( xShape ), m_nColorFormatType( nColorFormatType )
{
    m_xPropertySet.set( xShape, uno::UNO_QUERY_THROW );
    m_nFillFormatBackColor = 0;
    try
    {
        uno::Reference< oo::msforms::XFillFormat > xFillFormat( xInternalParent, uno::UNO_QUERY_THROW );
        m_pFillFormat = ( ScVbaFillFormat* )( xFillFormat.get() );
    }catch ( uno::RuntimeException  e )
    {
        m_pFillFormat = NULL;
    }
}

void
ScVbaColorFormat::setColorFormat( sal_Int16 _ntype )
{
    m_nColorFormatType = _ntype;
}

// Attribute
sal_Int32 SAL_CALL 
ScVbaColorFormat::getRGB() throw (uno::RuntimeException)
{
    sal_Int32 nRGB = 0;
    switch( m_nColorFormatType )
    {
    case ColorFormatType::LINEFORMAT_FORECOLOR:
        m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii("LineColor") ) >>= nRGB;
        break;
    case ColorFormatType::LINEFORMAT_BACKCOLOR:
        //TODO BackColor not supported
        // m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii("Color"), uno::makeAny( nRGB ) );
        break;
    case ColorFormatType::FILLFORMAT_FORECOLOR:
        m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii("FillColor") ) >>= nRGB;
        break;
    case ColorFormatType::FILLFORMAT_BACKCOLOR:
        nRGB = m_nFillFormatBackColor;
        break;
    default:
        throw uno::RuntimeException( rtl::OUString::createFromAscii("Second parameter of ColorFormat is wrong."), uno::Reference< uno::XInterface >() );
    }
    nRGB = OORGBToXLRGB( nRGB );
    return nRGB;
}

void SAL_CALL 
ScVbaColorFormat::setRGB( sal_Int32 _rgb ) throw (uno::RuntimeException)
{
    sal_Int32 nRGB = XLRGBToOORGB( _rgb );
    switch( m_nColorFormatType )
    {
    case ColorFormatType::LINEFORMAT_FORECOLOR:
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "LineColor" ), uno::makeAny( _rgb ) );
        break;
    case ColorFormatType::LINEFORMAT_BACKCOLOR:
        // TODO BackColor not supported
        break;
    case ColorFormatType::FILLFORMAT_FORECOLOR:
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "FillColor" ), uno::makeAny( _rgb ) );
        if( m_pFillFormat )
        {
            //TODO
            m_pFillFormat->setForeColorAndInternalStyle(nRGB);
        }
        break;
    case ColorFormatType::FILLFORMAT_BACKCOLOR:
        m_nFillFormatBackColor = nRGB;
        if( m_pFillFormat )
        {
            //TODO
            m_pFillFormat->setForeColorAndInternalStyle(nRGB);
        }
        break;
    default:    
        throw uno::RuntimeException( rtl::OUString::createFromAscii("Second parameter of ColorFormat is wrong."), uno::Reference< uno::XInterface >() );
    }
}

sal_Int32 SAL_CALL 
ScVbaColorFormat::getSchemeColor() throw (uno::RuntimeException)
{
    sal_Int32 nColor = getRGB();
    sal_Int32 i;
    for( i = 0; i < 56; i++ )
    {
        //TODO
        //if( nColor == com.sun.star.helper.calc.WorkbookImpl.getColorAtIndex(i) )
        //    break; 
    }
    if( i == 56 )
    {
        i = -2;
    }
    return ( i + 2 );
}

void SAL_CALL 
ScVbaColorFormat::setSchemeColor( sal_Int32 _schemecolor ) throw (uno::RuntimeException)
{
    //TODO
    //sal_Int32 nColor = com.sun.star.helper.calc.WorkbookImpl.getColorAtIndex( _schemecolor );
    //setRGB( com.sun.star.helper.calc.CalcHelper.swapFirstAndThirdByte( nColor ) );
}


rtl::OUString&
ScVbaColorFormat::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaColorFormat") );
    return sImplName;
}

uno::Sequence< rtl::OUString >
ScVbaColorFormat::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msforms.ColorFormat" ) );
    }
    return aServiceNames;
}

