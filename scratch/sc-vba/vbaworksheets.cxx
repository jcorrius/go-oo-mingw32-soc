#include "vbaworksheets.hxx"
#include <comphelper/processfactory.hxx>
#include <cppuhelper/implbase3.hxx>

#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <org/openoffice/vba/XApplication.hpp>

#include <tools/string.hxx>

#include "vbaglobals.hxx"
#include "vbaworksheet.hxx"
#include "vbaworkbook.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

typedef ::cppu::WeakImplHelper1< com::sun::star::container::XEnumeration 
> Enumeration_BASE;

class EnumerationImpl : public Enumeration_BASE
{
public:
	uno::Reference< uno::XComponentContext > m_xContext;
	uno::Reference< frame::XModel > m_xModel;
	uno::Reference< container::XEnumeration > m_xEnumeration;

	EnumerationImpl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< frame::XModel >& xModel ) throw ( uno::RuntimeException ) : m_xModel( xModel ), m_xContext( xContext )
	{
		uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY_THROW );
		uno::Reference<sheet::XSpreadsheets > xSheets( xSpreadDoc->getSheets(), uno::UNO_QUERY_THROW );
		uno::Reference< container::XEnumerationAccess > xEnumAccess( xSheets, uno::UNO_QUERY_THROW );
		m_xEnumeration.set( xEnumAccess->createEnumeration(), uno::UNO_QUERY_THROW );
	}
	// XEnumeration
	virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException) { return m_xEnumeration->hasMoreElements(); }
	virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		uno::Reference< sheet::XSpreadsheet > xSheet( m_xEnumeration->nextElement(), uno::UNO_QUERY_THROW );
		return makeAny( uno::Reference< vba::XWorksheet > ( new ScVbaWorksheet( m_xContext, xSheet, m_xModel ) ) );
	}
};

ScVbaWorksheets::ScVbaWorksheets( const uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, const uno::Reference< frame::XModel >& xModel ):
		mxModel( xModel ), m_xContext( xContext ) 
{ 
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( mxModel, uno::UNO_QUERY_THROW );
	mxEnumAccess.set( xSpreadDoc->getSheets(), uno::UNO_QUERY_THROW );
}

ScVbaWorksheets::ScVbaWorksheets( const uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, const uno::Reference< container::XEnumerationAccess >& xEnumAccess, const uno::Reference< frame::XModel >& xModel  ): mxEnumAccess( xEnumAccess ), m_xContext( xContext ), mxModel(xModel) {}

// XEnumerationAccess
uno::Type 
ScVbaWorksheets::getElementType() throw (uno::RuntimeException)
{
	return vba::XWorksheet::static_type(0);
}
::sal_Bool
ScVbaWorksheets::hasElements() throw (uno::RuntimeException)
{
	return sal_True;
}
uno::Reference< container::XEnumeration >
ScVbaWorksheets::createEnumeration() throw (uno::RuntimeException)
{
	css::uno::Reference< css::sheet::XSpreadsheets > xSheets( mxEnumAccess, uno::UNO_QUERY );
	if ( xSheets.is() )
		return new EnumerationImpl( m_xContext, mxModel );
	return mxEnumAccess->createEnumeration();
}

// XCollection
uno::Any
ScVbaWorksheets::getParent() throw (uno::RuntimeException)
{
	OSL_TRACE("In ScVbaWorksheets::getParent()");
	uno::Reference< vba::XApplication > xApplication =
		ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
	uno::Reference< vba::XWorkbook > xWorkbook;
	if ( xApplication.is() )
	{
		xWorkbook = xApplication->getActiveWorkbook();
	}
	if ( !xWorkbook.is() )
	{
		//throw uno::RuntimeException( rtl::OUString::createFromAscii(
		//	"ScVbaWorksheets::getParent - No Parent" ), uno::Reference< uno::XInterface >() );
	}
	
	OSL_TRACE("In ScVbaWorksheets::getParent() returning workbook");
	return uno::Any( xWorkbook );
}
::sal_Int32
ScVbaWorksheets::getCreator() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return 0;
}
uno::Reference< vba::XApplication >
ScVbaWorksheets::getApplication() throw (uno::RuntimeException)
{
	return ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
}

::sal_Int32
ScVbaWorksheets::getCount() throw (uno::RuntimeException)
{
	uno::Reference <container::XIndexAccess> xIndex( mxEnumAccess, uno::UNO_QUERY );
	if ( xIndex.is() )
	{
		return xIndex->getCount();
	}
	return 0;
}

uno::Any
ScVbaWorksheets::getItemByStringIndex( const rtl::OUString& sIndex ) throw (uno::RuntimeException)
{
	uno::Reference <container::XNameAccess> xName( mxEnumAccess, uno::UNO_QUERY );
	uno::Any result;

	if ( xName.is() )
	{
		uno::Reference< sheet::XSpreadsheet > xSheet( xName->getByName( sIndex ), uno::UNO_QUERY );
		result = makeAny( uno::Reference< vba::XWorksheet > ( new ScVbaWorksheet( m_xContext, xSheet, mxModel ) ) ); 
	}
	else
	{
		throw uno::RuntimeException(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			": Unable to obtain XNameAccess to query for spreadsheet name" ) ), 
			uno::Reference< uno::XInterface >() );
	}

	return result;
}

uno::Any
ScVbaWorksheets::getItemByIntIndex( const sal_Int32 nIndex ) throw (uno::RuntimeException)
{
	if ( nIndex <= 0 )
	{
		throw  lang::IndexOutOfBoundsException( 
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			"index is 0 or negative" ) ), 
			uno::Reference< uno::XInterface >() );
	}

	uno::Reference <container::XIndexAccess> xIndex( mxEnumAccess, uno::UNO_QUERY );

	if ( !xIndex.is() )
	{
		throw uno::RuntimeException(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			": Unable to obtain XIndexAccess to query for spreadsheet name" ) ), 
			uno::Reference< uno::XInterface >() );
	}
	// need to adjust for vba index ( for which first element is 1 )
	uno::Reference< sheet::XSpreadsheet > xSheet( xIndex->getByIndex( nIndex - 1 ), uno::UNO_QUERY );
	return makeAny( uno::Reference< vba::XWorksheet > ( new ScVbaWorksheet( m_xContext, xSheet, mxModel ) ) ); 
}

uno::Any
ScVbaWorksheets::Item( const uno::Any& aIndex ) throw (uno::RuntimeException)
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

// XWorksheets
uno::Any
ScVbaWorksheets::Add( const uno::Any& Before, const uno::Any& After,
					 const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException)
{
	rtl::OUString aStringSheet;
	sal_Bool bBefore(sal_True);
	sal_Int32 nSheetIndex = 0;
	sal_Int32 nNewSheets = 1, nType = 0;
	Count >>= nNewSheets;
	Type >>= nType;
	sal_Int32 nCount = 0;
	Before >>= aStringSheet;
	if (!aStringSheet.getLength())
	{
		After >>= aStringSheet;
		bBefore = sal_False;
	}
	if (!aStringSheet.getLength())
	{
		aStringSheet = ScVbaGlobals::getGlobalsImpl( 
			m_xContext )->getApplication()->getActiveWorkbook()->getActiveSheet()->getName();
		bBefore = sal_True;
	}
	uno::Reference <container::XIndexAccess> xIndex( mxEnumAccess, uno::UNO_QUERY );
	if ( xIndex.is() )
	{
		nCount = xIndex->getCount();
		for (sal_Int32 i=0; i < nCount; i++)
		{
			uno::Reference< sheet::XSpreadsheet > xSheet(xIndex->getByIndex(i), uno::UNO_QUERY);
			uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
			if (xNamed->getName() == aStringSheet)
			{
				nSheetIndex = i;
				break;
			}
		}
	}

	if(!bBefore)
		nSheetIndex++;

	uno::Reference< container::XNameAccess > xNameAccess( mxEnumAccess, uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XSpreadsheets > xSheets( mxEnumAccess, uno::UNO_QUERY_THROW );
	sal_Int32 nSheetName = nCount + 1L;
	String aStringBase( RTL_CONSTASCII_USTRINGPARAM("Sheet") );				
	for (sal_Int32 i=0; i < nNewSheets; i++, nSheetName++)
	{
		String aStringName = aStringBase;
		aStringName += String::CreateFromInt32(nSheetName);
		while (xNameAccess->hasByName(aStringName))
		{
			nSheetName++;
			aStringName = aStringBase;
			aStringName += String::CreateFromInt32(nSheetName);
		}
		xSheets->insertNewByName(aStringName, nSheetIndex + i);
	}
	return uno::Any();
}

void
ScVbaWorksheets::Delete() throw (uno::RuntimeException)
{
	//SC_VBA_STUB();
}

bool 
ScVbaWorksheets::isSelectedSheets()
{
	uno::Reference< sheet::XSpreadsheets > xSheets( mxEnumAccess, uno::UNO_QUERY );
	return !xSheets.is();
}
void SAL_CALL 
ScVbaWorksheets::PrintOut( const uno::Any& From, const uno::Any& To, const uno::Any& Copies, const uno::Any& Preview, const uno::Any& ActivePrinter, const uno::Any& PrintToFile, const uno::Any& Collate, const uno::Any& PrToFileName ) throw (uno::RuntimeException)
{
	
	sal_Int32 nArgs = 3;
	sal_Int32 nTo = 0;
	sal_Int32 nFrom = 0;
	sal_Int16 nCopies = 1;
	sal_Bool bCollate = sal_False;
	sal_Bool bSelection = sal_False;
	From >>= nFrom;
	To >>= nTo;
	Copies >>= nCopies;
	if ( nCopies > 1 ) // Collate only useful when more that 1 copy
		Collate >>= bCollate;

	rtl::OUString sRange(  RTL_CONSTASCII_USTRINGPARAM( "-" ) );
	rtl::OUString sFileName;
	
	if (( nFrom || nTo ) )
	{
		if ( nFrom )
			sRange = ( ::rtl::OUString::valueOf( nFrom ) + sRange );
		if ( nTo )
			sRange += ::rtl::OUString::valueOf( nTo );
		nArgs++;
	}
	else if ( isSelectedSheets() )
	{
		bSelection = sal_True;
		nArgs++;
	}

	if (  PrToFileName.getValue() )
	{
		PrToFileName >>= sFileName;
		nArgs++;
	}
	
	uno::Sequence< beans::PropertyValue > args1( nArgs );
	sal_Int32 nIndex = 0;
	args1[nIndex].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("CopyCount") );
	args1[nIndex++].Value <<= nCopies;
	args1[nIndex].Name =  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Collate"));
	args1[nIndex++].Value <<= bCollate;
	
	if ( sRange.getLength() > 1 )
	{
		args1[nIndex].Name =  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("RangeText"));
		args1[nIndex++].Value <<= sRange;
	}
	if ( sFileName.getLength() )
	{
		args1[nIndex].Name =  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("FileName"));
		args1[nIndex++].Value <<= sFileName;
	}
	if ( bSelection )
	{
		args1[nIndex].Name =  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Selection"));
		args1[nIndex++].Value <<= (sal_Bool)sal_True;
		
	}
	// #FIXME #TODO
	// 1 Preview ( does such a thing in OO.org? )
	// 2 ActivePrinter ( how/can we switch a printer via API? )
	// 3 PrintToFile ( ms behaviour if this option is specified but no 
	//   filename supplied 'PrToFileName' then the user will be prompted )
	// 4 Need to check behaviour of Selected sheets with range ( e.g. From & To values ) in oOO these options are mutually exclusive
	uno::Reference< frame::XModel > xModel = getCurrentDocument();	
	static rtl::OUString sUrl(RTL_CONSTASCII_USTRINGPARAM( ".uno:Print" ) );
	dispatchRequests ( xModel, sUrl, args1 );
}

