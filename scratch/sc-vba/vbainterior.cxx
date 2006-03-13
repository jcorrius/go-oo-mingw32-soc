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
#include <document.hxx>

#include "vbainterior.hxx"
using namespace ::com::sun::star;
using namespace ::org::openoffice;
static const rtl::OUString BACKCOLOR( RTL_CONSTASCII_USTRINGPARAM( "CellBackColor" ) );
/** Standard EGA colors, bright. */
#define EXC_PALETTE_EGA_COLORS_LIGHT \
            0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF
/** Standard EGA colors, dark. */
#define EXC_PALETTE_EGA_COLORS_DARK \
            0x800000, 0x008000, 0x000080, 0x808000, 0x800080, 0x008080, 0xC0C0C0, 0x808080

static const ColorData spnDefColorTable8[] =
{
/*  8 */    EXC_PALETTE_EGA_COLORS_LIGHT,
/* 16 */    EXC_PALETTE_EGA_COLORS_DARK,
/* 24 */    0x9999FF, 0x993366, 0xFFFFCC, 0xCCFFFF, 0x660066, 0xFF8080, 0x0066CC, 0xCCCCFF,
/* 32 */    0x000080, 0xFF00FF, 0xFFFF00, 0x00FFFF, 0x800080, 0x800000, 0x008080, 0x0000FF,
/* 40 */    0x00CCFF, 0xCCFFFF, 0xCCFFCC, 0xFFFF99, 0x99CCFF, 0xFF99CC, 0xCC99FF, 0xFFCC99,
/* 48 */    0x3366FF, 0x33CCCC, 0x99CC00, 0xFFCC00, 0xFF9900, 0xFF6600, 0x666699, 0x969696,
/* 56 */    0x003366, 0x339966, 0x003300, 0x333300, 0x993300, 0x993366, 0x333399, 0x333333
};

typedef ::cppu::WeakImplHelper1< container::XIndexAccess > XIndexAccess_BASE;

class DefaultPalette : public XIndexAccess_BASE
{
public:
   DefaultPalette(){}

    // Methods XIndexAccess
    virtual ::sal_Int32 SAL_CALL getCount() throw (uno::RuntimeException)
    {
        return sizeof(spnDefColorTable8) / sizeof(spnDefColorTable8[0]);
    }

    virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw (lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException)
    {
	if ( Index < 0 || Index >= getCount() )
		throw lang::IndexOutOfBoundsException();
        return uno::makeAny( sal_Int32( spnDefColorTable8[ Index ] ) );
    }

    // Methods XElementAcess
    virtual uno::Type SAL_CALL getElementType() throw (uno::RuntimeException)
    {
        return ::getCppuType( (sal_Int32*)0 );
    }
    virtual ::sal_Bool SAL_CALL hasElements() throw (uno::RuntimeException)
    {
        return sal_True;
    }

};


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
	aAny = m_xProps->getPropertyValue( BACKCOLOR );
	return aAny;
}
 
void 
ScVbaInterior::setColor( const uno::Any& _color  ) throw (uno::RuntimeException)
{
	m_xProps->setPropertyValue( BACKCOLOR , _color);
}

uno::Reference< container::XIndexAccess >
ScVbaInterior::getPalette()
{
	uno::Reference< container::XIndexAccess > xIndex;
	SfxObjectShell* pShell = m_pScDoc->GetDocumentShell();
	uno::Reference< beans::XPropertySet > xProps;
	if ( pShell )	
		xProps.set( pShell->GetModel(), uno::UNO_QUERY_THROW );
	else
		throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Can't extract palette, no doc shell" ) ), uno::Reference< uno::XInterface >() );
	xIndex.set( xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ColorPalette") ) ), uno::UNO_QUERY );
	if ( !xIndex.is() )
		return new DefaultPalette();
	return xIndex;	
}

void SAL_CALL 
ScVbaInterior::setColorIndex( const css::uno::Any& _colorindex ) throw (css::uno::RuntimeException) 
{
	sal_Int32 nIndex = 0;
	_colorindex >>= nIndex;
	if ( nIndex && ( ( nIndex == vba::Excel::Constants::xlColorIndexAutomatic ) || ( nIndex == vba::Excel::Constants::xlColorIndexNone ) ) )
	{
		setColor( uno::makeAny( (sal_Int32)-1 ) ); // default
		return;
	}
	--nIndex; // OOo indices are zero bases
	uno::Reference< container::XIndexAccess > xIndex = getPalette();
	setColor( xIndex->getByIndex( nIndex ) );
}

uno::Any SAL_CALL 
ScVbaInterior::getColorIndex() throw ( css::uno::RuntimeException )
{
	sal_Int32 nColor = 0;
	getColor() >>= nColor;
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


