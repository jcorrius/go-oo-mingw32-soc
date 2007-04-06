#include <org/openoffice/office/MsoArrowheadStyle.hpp>
#include <org/openoffice/office/MsoArrowheadLength.hpp>
#include <org/openoffice/office/MsoArrowheadWidth.hpp>
#include <org/openoffice/office/MsoLineDashStyle.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/LineDash.hpp>
#include "vbalineformat.hxx"

using namespace org::openoffice;
using namespace com::sun::star;

ScVbaLineFormat::ScVbaLineFormat( const uno::Reference< oo::vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape ) : ScVbaLineFormat_BASE( xParent, xContext ), m_xShape( xShape ) 
{
    m_xPropertySet.set( xShape, uno::UNO_QUERY_THROW );
    m_nLineDashStyle = office::MsoLineDashStyle::msoLineSolid;
    m_nLineWeight = 1;
}

sal_Int32
ScVbaLineFormat::calculateArrowheadSize()
{
    return 0;
}

sal_Int32
ScVbaLineFormat::convertLineStartEndNameToArrowheadStyle( rtl::OUString sLineName )
{
    sal_Int32 nLineType = office::MsoArrowheadStyle::msoArrowheadNone;
    if (sLineName.equals(rtl::OUString::createFromAscii("Small Arrow")) ||
        sLineName.equals(rtl::OUString::createFromAscii("Arrow")) ||
        sLineName.equals(rtl::OUString::createFromAscii("msArrowEnd")) ||
        sLineName.equals(rtl::OUString::createFromAscii("Double Arrow")))
    {
        // msoArrowheadTriangle
        nLineType = office::MsoArrowheadStyle::msoArrowheadTriangle;
    }
    else if (sLineName.equals(rtl::OUString::createFromAscii("Square 45")) ||
             sLineName.equals(rtl::OUString::createFromAscii("Square")) ||
             sLineName.equals(rtl::OUString::createFromAscii("msArrowDiamondEnd")))
    {
        // msoArrowheadDiamond
        nLineType = office::MsoArrowheadStyle::msoArrowheadDiamond;
    }
    else if (sLineName.equals(rtl::OUString::createFromAscii("Circle")) ||
             sLineName.equals(rtl::OUString::createFromAscii("msArrowOvalEnd")) ||
             sLineName.equals(rtl::OUString::createFromAscii("Dimension Lines")) )
    {
        // msoArrowheadOval
        nLineType = office::MsoArrowheadStyle::msoArrowheadOval;
    }
    else if (sLineName.equals(rtl::OUString::createFromAscii("Arrow concave")) ||
             sLineName.equals(rtl::OUString::createFromAscii("msArrowStealthEnd")))
    {
        // msoArrowheadStealth
        nLineType = office::MsoArrowheadStyle::msoArrowheadStealth;
    }
    else if (sLineName.equals(rtl::OUString::createFromAscii("Rounded short Arrow")) ||
             sLineName.equals(rtl::OUString::createFromAscii("Rounded large Arrow")) ||
             sLineName.equals(rtl::OUString::createFromAscii("Symmetric Arrow")) ||
             sLineName.equals(rtl::OUString::createFromAscii("msArrowOpenEnd")) ||
             sLineName.equals(rtl::OUString::createFromAscii("Line Arrow")))
    {
        // msoArrowheadOpen
        nLineType = office::MsoArrowheadStyle::msoArrowheadOpen;
    }
    else
    {
        // msoArrowheadNone
        nLineType = office::MsoArrowheadStyle::msoArrowheadNone;
    }
    return nLineType;
}

rtl::OUString 
ScVbaLineFormat::convertArrowheadStyleToLineStartEndName( sal_Int32 nArrowheadStyle ) throw (uno::RuntimeException)
{
    switch( nArrowheadStyle )
    {
        case office::MsoArrowheadStyle::msoArrowheadNone:
            return rtl::OUString(rtl::OUString::createFromAscii( "" ) );
        case office::MsoArrowheadStyle::msoArrowheadStealth:
            return rtl::OUString::createFromAscii( "Arrow concave" );
        case office::MsoArrowheadStyle::msoArrowheadOpen:
            return rtl::OUString::createFromAscii("Line Arrow" );
        case office::MsoArrowheadStyle::msoArrowheadOval:
            return  rtl::OUString::createFromAscii("Circle" );
        case office::MsoArrowheadStyle::msoArrowheadDiamond:
            return rtl::OUString::createFromAscii( "Square 45" );
        case office::MsoArrowheadStyle::msoArrowheadTriangle:
            return rtl::OUString::createFromAscii( "Arrow" );
        default:
            throw uno::RuntimeException( rtl::OUString::createFromAscii("Invalid Arrow Style!"), uno::Reference< uno::XInterface >() );
    }
}

// Attributes
sal_Int32 SAL_CALL 
ScVbaLineFormat::getBeginArrowheadStyle() throw (uno::RuntimeException)
{
    sal_Int32 nLineType = office::MsoArrowheadStyle::msoArrowheadNone;
    rtl::OUString sLineName;
    m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "LineStartName" ) ) >>= sLineName;
    if( ( sLineName.getLength() > 7 ) && ( sLineName.indexOf( rtl::OUString::createFromAscii( "msArray" ) ) ) != -1 )
    {
        sal_Int32 nIndex = sLineName.indexOf( rtl::OUString::createFromAscii(" ") );
        rtl::OUString sName = sLineName.copy( 0, nIndex );
        //sal_Int32 nSize = sLineName.copy( nIndex + 1 ).toInt32();
        nLineType = convertLineStartEndNameToArrowheadStyle( sName );
    }
    else
    {
        nLineType = convertLineStartEndNameToArrowheadStyle( sLineName );
    }
    return nLineType;
}

void SAL_CALL 
ScVbaLineFormat::setBeginArrowheadStyle( sal_Int32 _beginarrowheadstyle ) throw (uno::RuntimeException)
{
    rtl::OUString sArrayName = convertArrowheadStyleToLineStartEndName( _beginarrowheadstyle );
    m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "LineStartName" ), uno::makeAny( sArrayName ) );
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getBeginArrowheadLength() throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

void SAL_CALL 
ScVbaLineFormat::setBeginArrowheadLength( sal_Int32 _beginarrowheadlength ) throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getBeginArrowheadWidth() throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

void SAL_CALL 
ScVbaLineFormat::setBeginArrowheadWidth( sal_Int32 _beginarrowheadwidth ) throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getEndArrowheadStylel() throw (uno::RuntimeException)
{
    return 0;
}

void SAL_CALL 
ScVbaLineFormat::setEndArrowheadStylel( sal_Int32 _endarrowheadstylel ) throw (uno::RuntimeException)
{
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getEndArrowheadLength() throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

void SAL_CALL 
ScVbaLineFormat::setEndArrowheadLength( sal_Int32 _endarrowheadlength ) throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getEndArrowheadWidth() throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

void SAL_CALL 
ScVbaLineFormat::setEndArrowheadWidth( sal_Int32 _endarrowheadwidth ) throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString::createFromAscii("Property 'EndArrowheadWidth' is not supported."), uno::Reference< uno::XInterface >() );
}

double SAL_CALL 
ScVbaLineFormat::getWeight() throw (uno::RuntimeException)
{
    sal_Int32 nLineWidth;
    m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "LineWidth") ) >>= nLineWidth;
    double dLineWidth = Millimeter::getInPoints( nLineWidth );
    return dLineWidth;
}

void SAL_CALL 
ScVbaLineFormat::setWeight( double _weight ) throw (uno::RuntimeException)
{
    if( _weight < 0 )
        throw uno::RuntimeException( rtl::OUString::createFromAscii("Parameter: Must be positv."), uno::Reference< uno::XInterface >() );
    if( _weight == 0 )
        _weight = 0.5;
        
    sal_Int32 nLineWidth = Millimeter::getInHundredthsOfOneMillimeter( _weight );
    m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "LineWidth" ), uno::makeAny( nLineWidth ) );
}

sal_Bool SAL_CALL 
ScVbaLineFormat::getVisible() throw (uno::RuntimeException)
{
    return sal_True;
}

void SAL_CALL 
ScVbaLineFormat::setVisible( sal_Bool _visible ) throw (uno::RuntimeException)
{
}

double SAL_CALL 
ScVbaLineFormat::getTransparency() throw (uno::RuntimeException)
{
    return double(0);
}

void SAL_CALL 
ScVbaLineFormat::setTransparency( double _transparency ) throw (uno::RuntimeException)
{
}

sal_Int16 SAL_CALL 
ScVbaLineFormat::getStyle() throw (uno::RuntimeException)
{
    return sal_Int16(0);
}

void SAL_CALL 
ScVbaLineFormat::setStyle( sal_Int16 _style ) throw (uno::RuntimeException)
{
}

sal_Int32 SAL_CALL 
ScVbaLineFormat::getDashStyle() throw (uno::RuntimeException)
{
    sal_Int32 m_nLineDashStyle;
    enum drawing::LineStyle eLineStyle;
    //LineStyle integer in Xray
    m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "LineStyle" ) ) >>= eLineStyle;
    if( eLineStyle = drawing::LineStyle_SOLID )
        m_nLineDashStyle = office::MsoLineDashStyle::msoLineSolid;
    else
    {
        drawing::LineDash aLineDash;
        m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "LineDash" ) ) >>= aLineDash;
        if( aLineDash.Dots == 0 )
        {
            //LineDash
            //LineLongDash
            m_nLineDashStyle = office::MsoLineDashStyle::msoLineDash;
            if( aLineDash.Distance > 0 && ( aLineDash.DashLen / aLineDash.Distance > 1 ) )
            {
                m_nLineDashStyle = office::MsoLineDashStyle::msoLineLongDash;
            }
        }
        else if( aLineDash.Dots == 1 )
        {
            // LineDashDot
            // LineLongDashDot
            // LineSquareDot
            // LineRoundDot ! not supported
            m_nLineDashStyle = office::MsoLineDashStyle::msoLineDashDot;
            if( aLineDash.Dashes == 0 )
            {
                m_nLineDashStyle = office::MsoLineDashStyle::msoLineSquareDot;
            }
            else
            {
                if( aLineDash.Distance > 0 && ( aLineDash.DashLen / aLineDash.Distance > 1 ) )
                {
                    m_nLineDashStyle = office::MsoLineDashStyle::msoLineLongDashDot;
                }
            }
        }
        else if( aLineDash.Dots == 2 )
        {
            // LineDashDotDot
            m_nLineDashStyle = office::MsoLineDashStyle::msoLineDashDotDot;
        }
    }

    return m_nLineDashStyle;
}

void SAL_CALL 
ScVbaLineFormat::setDashStyle( sal_Int32 _dashstyle ) throw (uno::RuntimeException)
{
    m_nLineDashStyle = _dashstyle;
    if( _dashstyle == office::MsoLineDashStyle::msoLineSolid )
    {
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "LineStyle" ), uno::makeAny( drawing::LineStyle_SOLID  ));
    }
    else
    {
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "LineStyle" ), uno::makeAny( drawing::LineStyle_DASH ) );
    }
}


rtl::OUString& 
ScVbaLineFormat::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaLineFormat") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaLineFormat::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.LineFormat" ) );
	}
	return aServiceNames;
}


