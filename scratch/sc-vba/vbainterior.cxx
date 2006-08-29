#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/beans/XIntrospection.hpp>
#include <com/sun/star/beans/XIntrospectionAccess.hpp>
#include <com/sun/star/reflection/XIdlMethod.hpp>
#include <com/sun/star/beans/MethodConcept.hpp>

#include <org/openoffice/vba/Excel/Constants.hpp>

#include <comphelper/processfactory.hxx>
#include <cppuhelper/queryinterface.hxx>

#include <svx/xtable.hxx>

#include "vbainterior.hxx"
#include "vbapalette.hxx"
using namespace ::com::sun::star;
using namespace ::org::openoffice;
static const rtl::OUString BACKCOLOR( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) );

ScVbaInterior::ScVbaInterior( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< beans::XPropertySet >&  xProps, ScDocument* pScDoc ) throw ( lang::IllegalArgumentException) : m_xContext(xContext),m_xProps(xProps), m_pScDoc( pScDoc )
{
	if ( !xContext.is() )
		throw lang::IllegalArgumentException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "context not set" ) ), uno::Reference< uno::XInterface >(), 1 ); 
	if ( !m_xProps.is() )
		throw lang::IllegalArgumentException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "properties") ), uno::Reference< uno::XInterface >(), 2 ); 
}

uno::Any
ScVbaInterior::getColor() throw (uno::RuntimeException) 
{
	uno::Any aAny;
	aAny = OORGBToXLRGB( m_xProps->getPropertyValue( BACKCOLOR ) );
	return aAny;
}
 
void 
ScVbaInterior::setColor( const uno::Any& _color  ) throw (uno::RuntimeException)
{
	m_xProps->setPropertyValue( BACKCOLOR , XLRGBToOORGB(_color));
}

uno::Reference< container::XIndexAccess >
ScVbaInterior::getPalette()
{
	SfxObjectShell* pShell = m_pScDoc->GetDocumentShell();
	ScVbaPalette aPalette( pShell );	
	return aPalette.getPalette();	
}

void SAL_CALL 
ScVbaInterior::setColorIndex( const css::uno::Any& _colorindex ) throw (css::uno::RuntimeException) 
{
	sal_Int32 nIndex = 0;
	_colorindex >>= nIndex;
	// #FIXME  xlColorIndexAutomatic & xlColorIndexNone are not really
	// handled properly here
	
	if ( !nIndex || ( nIndex == vba::Excel::Constants::xlColorIndexAutomatic ) || ( nIndex == vba::Excel::Constants::xlColorIndexNone )  )
		nIndex = 1;
	--nIndex; // OOo indices are zero bases
	uno::Reference< container::XIndexAccess > xIndex = getPalette();
	// setColor expects colors in XL RGB values
	// #FIXME this is daft we convert OO RGB val to XL RGB val and
	// then back again to OO RGB value
	setColor( OORGBToXLRGB(xIndex->getByIndex( nIndex )) );
}

uno::Any SAL_CALL 
ScVbaInterior::getColorIndex() throw ( css::uno::RuntimeException )
{
	sal_Int32 nColor = 0;
	// getColor returns Xl ColorValue, need to convert it to OO val
	// as the palette deals with OO RGB values 
	// #FIXME this is daft in getColor we convert OO RGB val to XL RGB val 
	// and then back again to OO RGB value
	XLRGBToOORGB( getColor() ) >>= nColor; 
	uno::Reference< container::XIndexAccess > xIndex = getPalette();
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
	return uno::makeAny( nIndex );
}


