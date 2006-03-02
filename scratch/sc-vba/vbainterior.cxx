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
ScVbaInterior::setColor( const uno::Any& _color ) throw (uno::RuntimeException) 
{
	m_xProps->setPropertyValue( BACKCOLOR , _color);
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
	
	//XColorTable* pColorTable = m_pScDoc ? m_pScDoc->GetColorTable() : XColorTable::GetStdColorTable();	

	XColorTable* pColorTable = NULL;

	if ( m_pScDoc )
	{
		pColorTable = m_pScDoc->GetColorTable() ;
		OSL_TRACE("Using documents color table, num entries is %d",
			pColorTable->Count() );
	}
	else
		pColorTable = XColorTable::GetStdColorTable();	
	XColorEntry* pEntry = NULL;

	if ( nIndex < 0 || !pColorTable || ( nIndex >= pColorTable->Count() ) 
	|| !( pEntry = pColorTable->Get( nIndex ) ) )
		throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Interior::setColorIndex ( Illegal index )" ) ), uno::Reference< uno::XInterface >()  );

	setColor( uno::makeAny( pEntry->GetColor().GetRGBColor() ) );
}

uno::Any SAL_CALL 
ScVbaInterior::getColorIndex() throw ( css::uno::RuntimeException )
{
	return uno::Any();
}


