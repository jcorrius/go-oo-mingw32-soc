#include <org/openoffice/office/MsoArrowheadStyle.hpp>
#include <org/openoffice/office/MsoArrowheadLength.hpp>
#include <org/openoffice/office/MsoArrowheadWidth.hpp>
#include "vbalineformat.hxx"

using namespace org::openoffice;
using namespace com::sun::star;

ScVbaLineFormat::ScVbaLineFormat( const uno::Reference< oo::vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape ) : ScVbaLineFormat_BASE( xParent, xContext ), m_xShape( xShape ) 
{
    m_xPropertySet.set( xShape, uno::UNO_QUERY_THROW );
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


