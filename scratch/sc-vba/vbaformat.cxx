/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#include "vbaformat.hxx"
#include <org/openoffice/excel/XStyle.hpp>
#include <org/openoffice/excel/XlVAlign.hpp>
#include <org/openoffice/excel/XlHAlign.hpp>
#include <org/openoffice/excel/XlOrientation.hpp>
#include <org/openoffice/excel/Constants.hpp>
#include <com/sun/star/table/CellVertJustify.hpp>
#include <com/sun/star/table/CellHoriJustify.hpp>
#include <com/sun/star/table/CellOrientation.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/text/WritingMode.hpp>
#include <com/sun/star/util/CellProtection.hpp>

#include <rtl/math.hxx>

#include "vbaborders.hxx"
#include "vbapalette.hxx"
#include "vbafont.hxx"
#include "vbainterior.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

//strings
static rtl::OUString VERTJUSTIFY( RTL_CONSTASCII_USTRINGPARAM("VertJustify") );
static rtl::OUString HORIJUSTIFY( RTL_CONSTASCII_USTRINGPARAM("HoriJustify") );
static rtl::OUString ORIENTATION( RTL_CONSTASCII_USTRINGPARAM("Orientation") );
static rtl::OUString ROTATEANGLE( RTL_CONSTASCII_USTRINGPARAM("RotateAngle") );
static rtl::OUString ISTEXTWRAPPED( RTL_CONSTASCII_USTRINGPARAM("IsTextWrapped") );
static rtl::OUString NUMBERFORMAT( RTL_CONSTASCII_USTRINGPARAM("NumberFormat") );
static rtl::OUString FORMATSTRING( RTL_CONSTASCII_USTRINGPARAM("FormatString") );
static rtl::OUString LOCALE( RTL_CONSTASCII_USTRINGPARAM("Locale") );
static rtl::OUString PARAINDENT( RTL_CONSTASCII_USTRINGPARAM("ParaIndent") );
static rtl::OUString CELLPROTECTION( RTL_CONSTASCII_USTRINGPARAM("CellProtection") );
static rtl::OUString SHRINKTOFIT( RTL_CONSTASCII_USTRINGPARAM("ShrinkToFit") );
static rtl::OUString WRITINGMODE( RTL_CONSTASCII_USTRINGPARAM("WritingMode") );

template< typename Ifc1 >
ScVbaFormat< Ifc1 >::ScVbaFormat( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext > & xContext, const uno::Reference< beans::XPropertySet >& _xPropertySet, const uno::Reference< frame::XModel >& xModel, bool bCheckAmbiguoity ) throw ( script::BasicErrorException ) : ScVbaFormat_BASE( xParent, xContext ), m_aDefaultLocale( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("en") ), rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "US") ), rtl::OUString() ), mxPropertySet( _xPropertySet ), mxModel( xModel ), mbCheckAmbiguoity( bCheckAmbiguoity ), mbAddIndent( sal_False )
{
	try
	{
		mxServiceInfo.set( mxPropertySet, uno::UNO_QUERY_THROW );
		if ( !mxModel.is() )
			DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "XModel Interface could not be retrieved") ) );
		mxNumberFormatsSupplier.set( mxModel, uno::UNO_QUERY_THROW ); 
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setVerticalAlignment( const uno::Any& _oAlignment)   throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		sal_Int32 nAlignment = 0;
		if ( !(_oAlignment >>= nAlignment ))
			throw uno::RuntimeException();
		switch (nAlignment) 
		{
			case excel::XlVAlign::xlVAlignBottom :
				mxPropertySet->setPropertyValue(VERTJUSTIFY, uno::makeAny( table::CellVertJustify_BOTTOM ) );
				break;
			case excel::XlVAlign::xlVAlignCenter :
				mxPropertySet->setPropertyValue(VERTJUSTIFY, uno::makeAny( table::CellVertJustify_CENTER ) );
				break;
			case excel::XlVAlign::xlVAlignDistributed:
			case excel::XlVAlign::xlVAlignJustify:
				mxPropertySet->setPropertyValue(VERTJUSTIFY, uno::makeAny( table::CellVertJustify_STANDARD ));
				break;
	
			case excel::XlVAlign::xlVAlignTop:
				mxPropertySet->setPropertyValue(VERTJUSTIFY, uno::makeAny( table::CellVertJustify_TOP) );
				break;
			default:
				mxPropertySet->setPropertyValue(VERTJUSTIFY, uno::makeAny( table::CellVertJustify_STANDARD ));
				break;
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getVerticalAlignment(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aResult = aNULL();
	try
	{
		if (!isAmbiguous(VERTJUSTIFY))
		{
			table::CellVertJustify aAPIAlignment;
			mxPropertySet->getPropertyValue(VERTJUSTIFY) >>= aAPIAlignment;
			switch( aAPIAlignment )
			{
				case table::CellVertJustify_BOTTOM:
					aResult = uno::makeAny( excel::XlVAlign::xlVAlignBottom );
					break;
				case table::CellVertJustify_CENTER:
					aResult = uno::makeAny( excel::XlVAlign::xlVAlignCenter );
					break;
				case table::CellVertJustify_STANDARD:
					aResult = uno::makeAny( excel::XlVAlign::xlVAlignBottom );
					break;
				case table::CellVertJustify_TOP:
					aResult = uno::makeAny( excel::XlVAlign::xlVAlignTop );
					break;
				default:
					break;
			}
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return aResult;
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setHorizontalAlignment( const uno::Any& HorizontalAlignment ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		sal_Int32 nAlignment = 0;
		if ( !( HorizontalAlignment >>= nAlignment ) )
			throw uno::RuntimeException();
		switch ( nAlignment )
		{
			case excel::XlHAlign::xlHAlignJustify:
				mxPropertySet->setPropertyValue(HORIJUSTIFY, uno::makeAny( table::CellHoriJustify_BLOCK) );
				break;
			case excel::XlHAlign::xlHAlignCenter:
				mxPropertySet->setPropertyValue(HORIJUSTIFY, uno::makeAny( table::CellHoriJustify_CENTER ));
				break;
			case excel::XlHAlign::xlHAlignDistributed:
				mxPropertySet->setPropertyValue(HORIJUSTIFY,uno::makeAny( table::CellHoriJustify_BLOCK) );
				break;
			case excel::XlHAlign::xlHAlignLeft:
				mxPropertySet->setPropertyValue( HORIJUSTIFY, uno::makeAny( table::CellHoriJustify_LEFT) );
				break;
			case excel::XlHAlign::xlHAlignRight:
				mxPropertySet->setPropertyValue(HORIJUSTIFY, uno::makeAny( table::CellHoriJustify_RIGHT));
				break;
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}

}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getHorizontalAlignment(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any NRetAlignment = aNULL();
	try
	{
		if (!isAmbiguous(HORIJUSTIFY))
		{
			table::CellHoriJustify aAPIAlignment = table::CellHoriJustify_BLOCK;
		
			if ( mxPropertySet->getPropertyValue(HORIJUSTIFY) >>= aAPIAlignment )
			{
				switch( aAPIAlignment )
				{
					case table::CellHoriJustify_BLOCK:
						NRetAlignment = uno::makeAny( excel::XlHAlign::xlHAlignJustify );
						break;
					case table::CellHoriJustify_CENTER:
						NRetAlignment = uno::makeAny( excel::XlHAlign::xlHAlignCenter );
						break;
					case table::CellHoriJustify_LEFT:
						NRetAlignment = uno::makeAny( excel::XlHAlign::xlHAlignLeft );
						break;
					case table::CellHoriJustify_RIGHT:
						NRetAlignment =  uno::makeAny( excel::XlHAlign::xlHAlignRight );
						break;
					 default: // handle those other cases with a NULL return
						break;
				}
			}
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
	return NRetAlignment;
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setOrientation( const uno::Any& _aOrientation ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		sal_Int32 nOrientation = 0;
		if ( !( _aOrientation >>= nOrientation ) )
			throw uno::RuntimeException();
		switch( nOrientation )
		{
			case excel::XlOrientation::xlDownward:
				mxPropertySet->setPropertyValue(ORIENTATION,uno::makeAny( table::CellOrientation_TOPBOTTOM));
				break;
			case excel::XlOrientation::xlHorizontal:
				mxPropertySet->setPropertyValue(ORIENTATION, uno::makeAny( table::CellOrientation_STANDARD ));
				mxPropertySet->setPropertyValue(ROTATEANGLE, uno::makeAny( sal_Int32(0) ) );
				break;
			case excel::XlOrientation::xlUpward:
				mxPropertySet->setPropertyValue(ORIENTATION, uno::makeAny( table::CellOrientation_BOTTOMTOP));
				break;
			case excel::XlOrientation::xlVertical:
				mxPropertySet->setPropertyValue(ORIENTATION, uno::makeAny( table::CellOrientation_STACKED) );
				break;
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}
template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getOrientation(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any NRetOrientation = aNULL();
	try 
	{
		if (!isAmbiguous(ORIENTATION))
		{
			table::CellOrientation aOrientation = table::CellOrientation_STANDARD;
			if ( !(  mxPropertySet->getPropertyValue(ORIENTATION ) >>= aOrientation ) )
				throw uno::RuntimeException();

			switch(aOrientation)
			{
				case table::CellOrientation_STANDARD:
					NRetOrientation = uno::makeAny( excel::XlOrientation::xlHorizontal );
					break;
				case table::CellOrientation_BOTTOMTOP:
					NRetOrientation = uno::makeAny( excel::XlOrientation::xlUpward );
					break;
				case table::CellOrientation_TOPBOTTOM:
					NRetOrientation = uno::makeAny( excel::XlOrientation::xlDownward );
					break;
				case table::CellOrientation_STACKED:
					NRetOrientation = uno::makeAny( excel::XlOrientation::xlVertical );
					break;                    
				default:
					NRetOrientation = uno::makeAny( excel::XlOrientation::xlHorizontal );
			}
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return NRetOrientation;
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setWrapText( const uno::Any& _aWrapText ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		mxPropertySet->setPropertyValue(ISTEXTWRAPPED, _aWrapText);
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}	
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getWrapText(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aWrap = aNULL();
	try 
	{
		if (!isAmbiguous(ISTEXTWRAPPED))
		{
			aWrap = mxPropertySet->getPropertyValue(ISTEXTWRAPPED);
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
	return aWrap;
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::Borders( const uno::Any& Index ) throw (script::BasicErrorException, uno::RuntimeException )
{
	ScVbaPalette aPalette( getDocShell( mxModel ) );	
	uno::Reference< vba::XCollection > xColl =  new ScVbaBorders( thisHelperIface(), ScVbaFormat_BASE::mxContext, uno::Reference< table::XCellRange >( mxPropertySet, uno::UNO_QUERY_THROW ), aPalette );

	if ( Index.hasValue() )
	{ 
		return xColl->Item( Index, uno::Any() );
	}		
	return uno::makeAny( xColl );
}

template< typename Ifc1 >
uno::Reference< excel::XFont > SAL_CALL 
ScVbaFormat<Ifc1>::Font(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	ScVbaPalette aPalette( getDocShell( mxModel ) );
	return new ScVbaFont( thisHelperIface(), ScVbaFormat_BASE::mxContext, aPalette, mxPropertySet );
}

template< typename Ifc1 >
uno::Reference< excel::XInterior > SAL_CALL 
ScVbaFormat<Ifc1>::Interior(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	return new ScVbaInterior( thisHelperIface(), ScVbaFormat_BASE::mxContext, mxPropertySet );	
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getNumberFormatLocal(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aRet = uno::makeAny( rtl::OUString() );
	try 
	{            
		if (!isAmbiguous(NUMBERFORMAT))
		{
	
			initializeNumberFormats();
	
			sal_Int32 nFormat = 0;
			if ( ! (mxPropertySet->getPropertyValue(NUMBERFORMAT) >>= nFormat ) )
				throw uno::RuntimeException();
	
			rtl::OUString sFormat;
			xNumberFormats->getByKey(nFormat)->getPropertyValue(FORMATSTRING) >>= sFormat;
			aRet = uno::makeAny( sFormat.toAsciiLowerCase() );
		
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return aRet;

}

template< typename Ifc1 >
void 
ScVbaFormat<Ifc1>::setNumberFormat( lang::Locale _aLocale, const rtl::OUString& _sFormatString) throw( script::BasicErrorException )
{
	try
	{
		initializeNumberFormats();
		sal_Int32 nFormat = xNumberFormats->queryKey(_sFormatString, _aLocale , sal_True);
		if (nFormat == -1)
		{
			xNumberFormats->addNew(_sFormatString, _aLocale);
		}
		mxPropertySet->setPropertyValue(NUMBERFORMAT, uno::makeAny( nFormat ) );
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setNumberFormatLocal( const uno::Any& _oLocalFormatString ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		rtl::OUString sLocalFormatString;
		sal_Int32 nFormat = -1;
		if ( !(_oLocalFormatString >>= sLocalFormatString )
		|| !( mxPropertySet->getPropertyValue(NUMBERFORMAT) >>= nFormat ) )
			throw uno::RuntimeException();

		sLocalFormatString = sLocalFormatString.toAsciiUpperCase();
		initializeNumberFormats();
		lang::Locale aRangeLocale;
		xNumberFormats->getByKey(nFormat)->getPropertyValue(LOCALE) >>= aRangeLocale;
		sal_Int32 nNewFormat = xNumberFormats->queryKey(sLocalFormatString, aRangeLocale, sal_True);

		if (nNewFormat == -1)
			nNewFormat = xNumberFormats->addNew(sLocalFormatString, aRangeLocale);
		mxPropertySet->setPropertyValue(NUMBERFORMAT, uno::makeAny( nNewFormat ));
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setNumberFormat( const uno::Any& _oFormatString ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		rtl::OUString sFormatString;
		if ( !( _oFormatString >>= sFormatString ) )
			throw uno::RuntimeException();
		
		sFormatString = sFormatString.toAsciiUpperCase();

		lang::Locale aDefaultLocale = m_aDefaultLocale;
		initializeNumberFormats();
		sal_Int32 nFormat = xNumberFormats->queryKey(sFormatString, aDefaultLocale, sal_True);

		if (nFormat == -1)
			nFormat = xNumberFormats->addNew(sFormatString, aDefaultLocale);

		lang::Locale aRangeLocale;
		xNumberFormats->getByKey(nFormat)->getPropertyValue(LOCALE) >>= aRangeLocale;
		sal_Int32 nNewFormat = xNumberFormatTypes->getFormatForLocale(nFormat, aRangeLocale);
		mxPropertySet->setPropertyValue(NUMBERFORMAT, uno::makeAny( nNewFormat));
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setIndentLevel( const uno::Any& _aLevel ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try 
	{
		sal_Int32 nLevel = 0;
		if ( !(_aLevel >>= nLevel ) )
			throw uno::RuntimeException();
		table::CellHoriJustify aAPIAlignment = table::CellHoriJustify_STANDARD; 
		if ( !( mxPropertySet->getPropertyValue(HORIJUSTIFY) >>= aAPIAlignment ) )
			throw uno::RuntimeException();
		if (aAPIAlignment == table::CellHoriJustify_STANDARD) 
			mxPropertySet->setPropertyValue( HORIJUSTIFY, uno::makeAny( table::CellHoriJustify_LEFT) ) ;
		mxPropertySet->setPropertyValue(PARAINDENT, uno::makeAny( sal_Int16(nLevel * 352.8) ) );
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getIndentLevel(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any NRetIndentLevel = aNULL();
	try 
	{
		if (!isAmbiguous(PARAINDENT))
		{
			sal_Int16 IndentLevel = 0;
			if ( ( mxPropertySet->getPropertyValue(PARAINDENT) >>= IndentLevel  ) )
				NRetIndentLevel = uno::makeAny( sal_Int32( rtl::math::round(static_cast<double>( IndentLevel ) / 352.8)) );
			else 
				NRetIndentLevel = uno::makeAny( sal_Int32(0) );
		}	    
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return NRetIndentLevel;
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setLocked( const uno::Any& _aLocked ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try
	{
		sal_Bool bIsLocked = sal_False;
		if ( !( _aLocked >>= bIsLocked ) )
			throw uno::RuntimeException();
		util::CellProtection aCellProtection;
		mxPropertySet->getPropertyValue(CELLPROTECTION) >>= aCellProtection;
		aCellProtection.IsLocked = bIsLocked;
		mxPropertySet->setPropertyValue(CELLPROTECTION, uno::makeAny( aCellProtection ) );
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setFormulaHidden( const uno::Any& FormulaHidden ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try
	{
		sal_Bool bIsFormulaHidden = sal_False;
		FormulaHidden >>= bIsFormulaHidden;
		util::CellProtection aCellProtection;
		mxPropertySet->getPropertyValue(CELLPROTECTION) >>= aCellProtection; 
		aCellProtection.IsFormulaHidden = bIsFormulaHidden;
		mxPropertySet->setPropertyValue(CELLPROTECTION,uno::makeAny(aCellProtection));
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception( SbERR_METHOD_FAILED, rtl::OUString() );                    
	}
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getLocked(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aCellProtection = aNULL();
	try 
	{
		if (!isAmbiguous(CELLPROTECTION))
		{
			util::CellProtection cellProtection;
			mxPropertySet->getPropertyValue(CELLPROTECTION) >>= cellProtection;

			aCellProtection = uno::makeAny( cellProtection.IsLocked );
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());                    
	}
	return aCellProtection;
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getFormulaHidden(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aBoolRet = aNULL();
	try
	{
		if (!isAmbiguous(CELLPROTECTION))
		{
			util::CellProtection aCellProtection;
			mxPropertySet->getPropertyValue(CELLPROTECTION) >>= aCellProtection; 
			aBoolRet = uno::makeAny( aCellProtection.IsFormulaHidden );    
		}
	}
	catch (uno::Exception e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}    
	return aBoolRet;
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setShrinkToFit( const uno::Any& ShrinkToFit ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try
	{
		mxPropertySet->setPropertyValue(SHRINKTOFIT, ShrinkToFit);
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString() );
	}

}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getShrinkToFit(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aRet = aNULL();
	try
	{
		if (!isAmbiguous(SHRINKTOFIT))
			aRet = mxPropertySet->getPropertyValue(SHRINKTOFIT);
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());                    
	}
	return aRet;
}

template< typename Ifc1 >
void SAL_CALL 
ScVbaFormat<Ifc1>::setReadingOrder( const uno::Any& ReadingOrder ) throw (script::BasicErrorException, uno::RuntimeException)
{
	try
	{
		sal_Int32 nReadingOrder = 0;
		if ( !(ReadingOrder >>= nReadingOrder ))
			throw uno::RuntimeException();
		switch(nReadingOrder)
		{
			case excel::Constants::xlLTR:
				mxPropertySet->setPropertyValue(WRITINGMODE, uno::makeAny( text::WritingMode_LR_TB ));
				break;
			case excel::Constants::xlRTL:
				mxPropertySet->setPropertyValue(WRITINGMODE, uno::makeAny( text::WritingMode_RL_TB ));
				break;
			case excel::Constants::xlContext:
				DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());                    
				break;
			default:
				DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());                    
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());                    
	}

}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat<Ifc1>::getReadingOrder(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any NRetReadingOrder = aNULL();
	try
	{
		if (!isAmbiguous(WRITINGMODE))
		{
			text::WritingMode aWritingMode = text::WritingMode_LR_TB;
			if ( ( mxPropertySet->getPropertyValue(WRITINGMODE) ) >>= aWritingMode ) 
			switch (aWritingMode){
				case text::WritingMode_LR_TB:
					NRetReadingOrder = uno::makeAny(excel::Constants::xlLTR);
					break;
				case text::WritingMode_RL_TB:
					NRetReadingOrder = uno::makeAny(excel::Constants::xlRTL);
					break;
				default:
					NRetReadingOrder = uno::makeAny(excel::Constants::xlRTL);
			}
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());
	}
	return NRetReadingOrder;
	
}

template< typename Ifc1 >
uno::Any SAL_CALL 
ScVbaFormat< Ifc1 >::getNumberFormat(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
	uno::Any aFormat = aNULL();
	try 
	{
		sal_Int32 nFormat = -1;
		if (!isAmbiguous(NUMBERFORMAT) && 
			( mxPropertySet->getPropertyValue(NUMBERFORMAT) >>= nFormat) )
		{
			initializeNumberFormats();

			sal_Int32 nNewFormat = xNumberFormatTypes->getFormatForLocale(nFormat, getDefaultLocale() );
			rtl::OUString sFormat;
			xNumberFormats->getByKey(nNewFormat)->getPropertyValue(FORMATSTRING) >>= sFormat;
			aFormat = uno::makeAny( sFormat );
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return aFormat;
}

template< typename Ifc1 >
bool
ScVbaFormat<Ifc1>::isAmbiguous(const rtl::OUString& _sPropertyName) throw ( script::BasicErrorException )
{
	bool bResult = false;
	try
	{
		if (mbCheckAmbiguoity)
			bResult = ( getXPropertyState()->getPropertyState(_sPropertyName) == beans::PropertyState_AMBIGUOUS_VALUE );
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}	
	return bResult;
}

template< typename Ifc1 >
void 
ScVbaFormat<Ifc1>::initializeNumberFormats() throw ( script::BasicErrorException )
{
	if ( !xNumberFormats.is() )
	{
		xNumberFormats = mxNumberFormatsSupplier->getNumberFormats();
		xNumberFormatTypes.set( xNumberFormats, uno::UNO_QUERY ); // _THROW?
	}
}

template< typename Ifc1 >
uno::Reference< beans::XPropertyState > 
ScVbaFormat<Ifc1>::getXPropertyState() throw ( uno::RuntimeException )
{
	if ( !xPropertyState.is() )
		xPropertyState.set( mxPropertySet, uno::UNO_QUERY_THROW );
	return xPropertyState;
}

template< typename Ifc1 >
rtl::OUString&
ScVbaFormat<Ifc1>::getServiceImplName()
{
        static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaFormat") );
        return sImplName;
}

template< typename Ifc1 >
uno::Sequence< rtl::OUString >
ScVbaFormat<Ifc1>::getServiceNames()
{
        static uno::Sequence< rtl::OUString > aServiceNames;
        if ( aServiceNames.getLength() == 0 )
        {
                aServiceNames.realloc( 1 );
                aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.excel.Format" ) );
        }
        return aServiceNames;
}

template class ScVbaFormat< excel::XStyle >;
template class ScVbaFormat< excel::XRange >;


