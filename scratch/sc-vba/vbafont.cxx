#include <com/sun/star/beans/XProperty.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include "vbafont.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

void 
ScVbaFont::setBold( sal_Bool bValue ) throw( uno::RuntimeException )
{
	double fBoldValue = awt::FontWeight::NORMAL;
	if( bValue )
		fBoldValue = awt::FontWeight::BOLD;
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharWeight" ) ), ( uno::Any )fBoldValue );
	
} 
	
sal_Bool
ScVbaFont::getBold() throw ( uno::RuntimeException )
{
	double fValue;
	uno::Any aValue = mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharWeight" ) ) );
	aValue >>= fValue;
	return( fValue == awt::FontWeight::BOLD );
}

void 
ScVbaFont::setUnderline( sal_Bool bValue ) throw ( uno::RuntimeException )
{
	short nValue = awt::FontUnderline::NONE;
	if( bValue )
		nValue = awt::FontUnderline::SINGLE;
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharUnderline" ) ), ( uno::Any )nValue );
}

sal_Bool
ScVbaFont::getUnderline() throw ( uno::RuntimeException )
{
	short nValue;
	uno::Any aValue = mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharUnderline" ) ) );
	aValue >>= nValue;
	return( nValue == awt::FontUnderline::SINGLE );
}
		
void
ScVbaFont::setStrikethrough( sal_Bool bValue ) throw ( uno::RuntimeException )
{
	short nValue = awt::FontStrikeout::NONE;
	if( bValue )
		nValue = awt::FontStrikeout::SINGLE;
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharStrikeout" ) ), ( uno::Any )nValue );
}

sal_Bool
ScVbaFont::getStrikethrough() throw ( uno::RuntimeException )
{
	short nValue;
    uno::Any aValue = mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharStrikeout" ) ) );
    aValue >>= nValue;
    return( nValue == awt::FontStrikeout::SINGLE );
}

void 
ScVbaFont::setShadow( sal_Bool bValue ) throw ( uno::RuntimeException )
{
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharShadowed" ) ), ( uno::Any )bValue );
}

sal_Bool
ScVbaFont::getShadow() throw (uno::RuntimeException)
{
	sal_Bool bValue;
	uno::Any aValue = mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharShadowed" ) ) );
	aValue >>= bValue;
	return bValue;
}

void 
ScVbaFont::setItalic( sal_Bool bValue ) throw ( uno::RuntimeException )
{
	short nValue = awt::FontSlant_NONE;
	if( bValue )
		nValue = awt::FontSlant_ITALIC;
    mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharPosture" ) ), ( uno::Any )nValue );
}

sal_Bool
ScVbaFont::getItalic() throw ( uno::RuntimeException )
{
	short nValue;
	uno::Any aValue = mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharPosture" ) ) );
	aValue >>= nValue;
	return( nValue == awt::FontSlant_ITALIC );
}

void 
ScVbaFont::setName( const ::rtl::OUString &rString ) throw ( uno::RuntimeException )
{
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharFontName" ) ), ( uno::Any )rString );	
}

::rtl::OUString
ScVbaFont::getName() throw ( uno::RuntimeException )
{
	::rtl::OUString rString;
	uno::Any aValue = mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharFontName" ) ) );
	aValue >>= rString;
	return rString;
}
