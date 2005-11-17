#include "vbachart.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/document/XEmbeddedObjectSupplier.hpp>


#include "vbachartobjects.hxx"
#include "vbachartobject.hxx"
#include "vbaglobals.hxx"

using namespace ::com::sun::star;
using namespace ::org::openoffice;

typedef ::cppu::WeakImplHelper1< container::XEnumeration 
> ChartObjectEnumeration_BASE;

class ChartObjectEnumerationImpl : public ChartObjectEnumeration_BASE
{
public:
	uno::Reference< uno::XComponentContext > m_xContext;
	uno::Reference< container::XEnumeration > m_xEnumeration;

	ChartObjectEnumerationImpl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< container::XEnumeration >& xEnumeration ) throw ( uno::RuntimeException ) : m_xEnumeration( xEnumeration ), m_xContext( xContext ) { }
	virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException) { return m_xEnumeration->hasMoreElements(); }
	virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		uno::Reference< table::XTableChart > xTableChart( m_xEnumeration->nextElement(), uno::UNO_QUERY_THROW );
		return makeAny( uno::Reference< vba::XChartObject > ( new ScVbaChartObject( m_xContext, xTableChart ) ) );
	}
};



// #TODO #FIXME This is pretty similar to the collection implementation of 
// XWorksheets ( also XWorkbooks? ) there should be some opportunity for
// code sharing!! 
// XCollection

uno::Any
ScVbaChartObjects::getParent() throw (uno::RuntimeException)
{
	uno::Reference< vba::XApplication > xApplication =
		getApplication();
	uno::Reference< vba::XWorkbook > xWorkbook;
	if ( xApplication.is() )
	{
		xWorkbook = xApplication->getActiveWorkbook();
	}
	if ( !xWorkbook.is() )
	{
		// #FIXME #TODO if no workbook should we throw?
		// what happens in vba?
		//throw uno::RuntimeException( rtl::OUString::createFromAscii(
		//	"ScVbaChartObjects::getParent - No Parent" ), uno::Reference< uno::XInterface >() );
	}
	
	return uno::Any( xWorkbook );
}
::sal_Int32
ScVbaChartObjects::getCreator() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return 0;
}
uno::Reference< vba::XApplication >
ScVbaChartObjects::getApplication() throw (uno::RuntimeException)
{
	return ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
}
::sal_Int32
ScVbaChartObjects::getCount() throw (uno::RuntimeException)
{
	uno::Reference <container::XIndexAccess> xIndex( m_xTableCharts, uno::UNO_QUERY );
	if ( xIndex.is() )
	{
		return xIndex->getCount();
	}
	return 0;
}

uno::Any
ScVbaChartObjects::Item( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	if ( aIndex.getValueTypeClass() != uno::TypeClass_STRING )
	{
		sal_Int32 nIndex = 0;

		if ( ( aIndex >>= nIndex ) != sal_True )
		{
			rtl::OUString message;
			message = rtl::OUString::createFromAscii(
				"Couldn't convert index to Int32");
			throw  lang::IndexOutOfBoundsException( message,
				uno::Reference< uno::XInterface >() );
		}
		return 	getItemByIntIndex( nIndex );
	}
	rtl::OUString aStringSheet;

	aIndex >>= aStringSheet;
	return getItemByStringIndex( aStringSheet );

}

uno::Any
ScVbaChartObjects::getItemByIntIndex( const sal_Int32 nIndex ) throw (uno::RuntimeException)
{
	if ( nIndex <= 0 )
	{
		throw  lang::IndexOutOfBoundsException( 
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			"index is 0 or negative" ) ), 
			uno::Reference< uno::XInterface >() );
	}

	uno::Reference <container::XIndexAccess> xIndex( m_xTableCharts, uno::UNO_QUERY );

	if ( !xIndex.is() )
	{
		throw uno::RuntimeException(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			": Unable to obtain XIndexAccess to query for chartobject name" ) ), 
			uno::Reference< uno::XInterface >() );
	}
	// need to adjust for vba index ( for which first element is 1 )
	uno::Reference< table::XTableChart > xTableChart( xIndex->getByIndex( nIndex - 1 ), uno::UNO_QUERY );
	return makeAny( uno::Reference< vba::XChartObject > ( new ScVbaChartObject( m_xContext, xTableChart ) ) ); 
}

uno::Any
ScVbaChartObjects::getItemByStringIndex( const rtl::OUString& sIndex ) throw (uno::RuntimeException)
{
	uno::Reference <container::XNameAccess> xName( m_xTableCharts, uno::UNO_QUERY );
	uno::Any result;

	if ( xName.is() )
	{
		uno::Reference< table::XTableChart  > xTableChart( xName->getByName( sIndex ), uno::UNO_QUERY );
		result = makeAny( uno::Reference< vba::XChartObject > ( new ScVbaChartObject( m_xContext, xTableChart ) ) ); 
	}
	else
	{
		throw uno::RuntimeException(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			": Unable to obtain XNameAccess to query for chartobject name" ) ), 
			uno::Reference< uno::XInterface >() );
	}

	return result;
}



// XEnumerationAccess

uno::Reference< container::XEnumeration >
ScVbaChartObjects::createEnumeration() throw (uno::RuntimeException)
{
	css::uno::Reference< container::XEnumerationAccess > xEnumAccess( m_xTableCharts, uno::UNO_QUERY_THROW );
	return new ChartObjectEnumerationImpl( m_xContext, xEnumAccess->createEnumeration() );
}

// XElementAccess

uno::Type 
ScVbaChartObjects::getElementType() throw (uno::RuntimeException)
{
	return vba::XChartObject::static_type(0);
}
::sal_Bool
ScVbaChartObjects::hasElements() throw (uno::RuntimeException)
{
	// #TODO #TOFIX Really?, how can we say that!
	return sal_True;
}




