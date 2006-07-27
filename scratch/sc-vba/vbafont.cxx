#include <com/sun/star/beans/XProperty.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <org/openoffice/vba/Excel/Constants.hpp>
#include <org/openoffice/vba/Excel/XlUnderlineStyle.hpp>
#include "vbafont.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

// use local constants there is no need to expose these constants
// externally. Looking at the Format->Character dialog it seem that 
// these may infact be even be calculated. Leave hardcoded for now
// #FIXEME #TBD investigate the code for dialog mentioned above

// The font baseline is not specified.
const short NORMAL = 0;

// specifies a superscripted.
const short SUPERSCRIPT = 33;

// specifies a subscripted.
const short SUBSCRIPT = -33;

// specifies a hight of superscripted font 
 const sal_Int8 SUPERSCRIPTHEIGHT = 58; 

// specifies a hight of subscripted font 
const sal_Int8 SUBSCRIPTHEIGHT = 58; 

// specifies a hight of normal font 
const short NORMALHEIGHT = 100; 

void 
ScVbaFont::setSuperscript( sal_Bool bValue ) throw ( uno::RuntimeException )
{
	sal_Int16 nValue = NORMAL;
	sal_Int8 nValue2 = NORMALHEIGHT;

        if( bValue ) 
	{
		nValue = SUPERSCRIPT;
	        nValue2 = SUPERSCRIPTHEIGHT;
	}
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharEscapement" ) ), ( uno::Any )nValue );
 	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharEscapementHeight" ) ), ( uno::Any )nValue2 );
}

sal_Bool
ScVbaFont::getSuperscript() throw ( uno::RuntimeException )
{
	short nValue = 0;
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharEscapement" ) ) ) >>= nValue;
	return( nValue == SUPERSCRIPT );
}

void 
ScVbaFont::setSubscript( sal_Bool bValue ) throw ( uno::RuntimeException )
{
	sal_Int16 nValue = NORMAL;
	sal_Int8 nValue2 = NORMALHEIGHT;

        if( bValue ) 
	{
		nValue= SUBSCRIPT;
	        nValue2 = SUBSCRIPTHEIGHT;
	}

 	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharEscapementHeight" ) ), ( uno::Any )nValue2 );
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharEscapement" ) ), ( uno::Any )nValue );

}

sal_Bool
ScVbaFont::getSubscript() throw ( uno::RuntimeException )
{
	short nValue = NORMAL;
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharEscapement" ) ) ) >>= nValue;
	return( nValue == SUBSCRIPT );
}

void 
ScVbaFont::setSize( float fValue ) throw( uno::RuntimeException )
{
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharHeight" ) ), ( uno::Any )fValue );
} 
	
float
ScVbaFont::getSize() throw ( uno::RuntimeException )
{
        float fValue = 0.0;
        mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharHeight" ) ) ) >>= fValue;
        return fValue;
}

void
ScVbaFont::setColorIndex( sal_Int32 lValue ) throw( uno::RuntimeException )
{
	// #TODO #FIXME is behavior random or just default, maybe it depends
	// on the parent object ( which we don't really take care of right now
	if ( lValue == vba::Excel::Constants::xlColorIndexAutomatic )
		lValue = 1;
	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharColor" ) ), ( uno::Any )mPalette.getPalette()->getByIndex( --lValue ) );
} 
	

sal_Int32
ScVbaFont::getColorIndex() throw ( uno::RuntimeException )
{
//XXX
        sal_Int32 nColor = -1;
        uno::Any aValue= mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharColor" ) ) );
	aValue >>= nColor;
	uno::Reference< container::XIndexAccess > xIndex = mPalette.getPalette();
	sal_Int32 nElems = xIndex->getCount();
	sal_Int32 nIndex = -1;
	for ( sal_Int32 count=0; count<nElems; ++count )
       	{
		sal_Int32 nPaletteColor;
		xIndex->getByIndex( count ) >>= nPaletteColor;
		if ( nPaletteColor == nColor )
		{
			nIndex = count + 1; // 1 based
			break;
		}
	} 
	return nIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////
void 
ScVbaFont::setStandardFontSize( float fValue ) throw( uno::RuntimeException )
{
//XXX
	//mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharSize" ) ), ( uno::Any )fValue );
	throw uno::RuntimeException(
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("setStandardFontSize not supported") ), uno::Reference< uno::XInterface >() );
} 
	

float
ScVbaFont::getStandardFontSize() throw ( uno::RuntimeException )
{
//XXX
	throw uno::RuntimeException(
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("getStandardFontSize not supported") ), uno::Reference< uno::XInterface >() );
}


void 
ScVbaFont::setStandardFont( const ::rtl::OUString &rString ) throw( uno::RuntimeException )
{
//XXX
	//mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharSize" ) ), ( uno::Any )aValue );
	throw uno::RuntimeException(
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("setStandardFont not supported") ), uno::Reference< uno::XInterface >() );
} 
	

::rtl::OUString 
ScVbaFont::getStandardFont() throw ( uno::RuntimeException )
{
//XXX
	throw uno::RuntimeException(
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("getStandardFont not supported") ), uno::Reference< uno::XInterface >() );
}

void 
ScVbaFont::setFontStyle( const ::rtl::OUString &rString ) throw( uno::RuntimeException )
{
//XXX
	//mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharSize" ) ), ( uno::Any )aValue );
	throw uno::RuntimeException(
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("setFontStyle not supported") ), uno::Reference< uno::XInterface >() );
} 
	

::rtl::OUString 
ScVbaFont::getFontStyle() throw ( uno::RuntimeException )
{
//XXX
	throw uno::RuntimeException(
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("getFontStyle not supported") ), uno::Reference< uno::XInterface >() );
}

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
	 mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharWeight" ) ) ) >>= fValue;
	return( fValue == awt::FontWeight::BOLD );
}

void 
ScVbaFont::setUnderline( const uno::Any& aValue ) throw ( uno::RuntimeException )
{
	// default
	sal_Int32 nValue = vba::Excel::XlUnderlineStyle::xlUnderlineStyleNone;
	aValue >>= nValue;
	switch ( nValue )
	{
// NOTE:: #TODO #FIMXE 
// xlUnderlineStyleDoubleAccounting & xlUnderlineStyleSingleAccounting
// don't seem to be supported in Openoffice. 
// The import filter converts them to single or double underlines as appropriate
// So, here at the moment we are similarly silently converting 
// xlUnderlineStyleSingleAccounting to xlUnderlineStyleSingle.

		case vba::Excel::XlUnderlineStyle::xlUnderlineStyleNone:
			nValue = awt::FontUnderline::NONE;
			break;
		case vba::Excel::XlUnderlineStyle::xlUnderlineStyleSingle:
		case vba::Excel::XlUnderlineStyle::xlUnderlineStyleSingleAccounting:
			nValue = awt::FontUnderline::SINGLE;
			break;
		case vba::Excel::XlUnderlineStyle::xlUnderlineStyleDouble:
		case vba::Excel::XlUnderlineStyle::xlUnderlineStyleDoubleAccounting:
			nValue = awt::FontUnderline::DOUBLE;
			break;
		default:
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Unknown value for Underline")), uno::Reference< uno::XInterface >() );
	}

	mxFont->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharUnderline" ) ), ( uno::Any )nValue );
	
}

uno::Any
ScVbaFont::getUnderline() throw ( uno::RuntimeException )
{
	sal_Int32 nValue = awt::FontUnderline::NONE;
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharUnderline" ) ) ) >>= nValue;
	switch ( nValue )
	{
		case  awt::FontUnderline::DOUBLE:
			nValue = vba::Excel::XlUnderlineStyle::xlUnderlineStyleDouble;
			break;
		case  awt::FontUnderline::SINGLE:
			nValue = vba::Excel::XlUnderlineStyle::xlUnderlineStyleSingle;
			break;
		case  awt::FontUnderline::NONE:
			nValue = vba::Excel::XlUnderlineStyle::xlUnderlineStyleNone;
			break;
		default:
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Unknown value retrieved for Underline") ), uno::Reference< uno::XInterface >() );
		
	}
	return uno::makeAny( nValue );
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
	short nValue = 0;
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharStrikeout" ) ) ) >>= nValue;
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
	sal_Bool bValue = false;
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharShadowed" ) ) ) >>= bValue;
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
	short nValue = 0;  
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharPosture" ) ) ) >>= nValue;
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
	mxFont->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharFontName" ) ) ) >>= rString;
	return rString;
}
