#include "vbawindow.hxx"
#include "vbaworksheets.hxx"
#include "vbaworksheet.hxx"
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <docsh.hxx>
#include <tabvwsh.hxx>
#include <docuno.hxx>
#include <sc.hrc>
#include <hash_map>

using namespace ::com::sun::star;
using namespace ::org::openoffice;

typedef  std::hash_map< rtl::OUString,
SCTAB, ::rtl::OUStringHash,
::std::equal_to< ::rtl::OUString > > NameIndexHash;

typedef std::vector < uno::Reference< sheet::XSpreadsheet > > Sheets;

typedef ::cppu::WeakImplHelper1< container::XEnumeration

> Enumeration_BASE;

typedef ::cppu::WeakImplHelper3< container::XEnumerationAccess 
	, com::sun::star::container::XIndexAccess
	, com::sun::star::container::XNameAccess
	> SelectedSheets_BASE;


class SelectedSheetsEnum : public Enumeration_BASE
{
public:
	uno::Reference< uno::XComponentContext > m_xContext;
	uno::Reference< frame::XModel > m_xModel;
	Sheets m_sheets;
	Sheets::const_iterator m_it;

	SelectedSheetsEnum( const uno::Reference< uno::XComponentContext >& xContext, const Sheets& sheets, const uno::Reference< frame::XModel >& xModel ) throw ( uno::RuntimeException ) :  m_xContext( xContext ), m_sheets( sheets ), m_xModel( xModel )
	{
		m_it = m_sheets.begin();
	}
	// XEnumeration
	virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException) 
	{ 
		return m_it != m_sheets.end();
	}
	virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		if ( !hasMoreElements() )
		{
			throw container::NoSuchElementException();
		}
		return makeAny( uno::Reference< vba::XWorksheet > ( new ScVbaWorksheet( m_xContext, *(m_it++), m_xModel ) ) );
	}


};

class SelectedSheetsEnumAccess : public SelectedSheets_BASE
{
	uno::Reference< uno::XComponentContext > m_xContext;
	NameIndexHash namesToIndices;
	Sheets sheets;
	uno::Reference< frame::XModel > m_xModel; 
public:
	SelectedSheetsEnumAccess( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< frame::XModel >& xModel ):m_xContext( xContext ), m_xModel( xModel )
	{
		ScModelObj* pModel = static_cast< ScModelObj* >( m_xModel.get() );
		if ( !pModel )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot obtain current document" ) ), uno::Reference< uno::XInterface >() );
		ScDocShell* pDocShell = (ScDocShell*)pModel->GetEmbeddedObject();
		if ( !pDocShell )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot obtain docshell" ) ), uno::Reference< uno::XInterface >() );
		ScTabViewShell* pViewShell = pDocShell->GetBestViewShell();
		if ( !pViewShell )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot obtain view shell" ) ), uno::Reference< uno::XInterface >() );
			
		SCTAB nTabCount = pDocShell->GetDocument()->GetTableCount();
		uno::Sequence<sal_Int32> aSheets( nTabCount );
		sal_Int32 nIndex = 0;
		const ScMarkData& rMarkData = pViewShell->GetViewData()->GetMarkData();
		sheets.reserve( nTabCount );
		uno::Reference <sheet::XSpreadsheetDocument> xSpreadSheet( m_xModel, uno::UNO_QUERY_THROW );
		uno::Reference <container::XIndexAccess> xIndex( xSpreadSheet->getSheets(), uno::UNO_QUERY_THROW );
		for ( SCTAB nTab=0; nTab<nTabCount; nTab++ )
		{
			if ( rMarkData.GetTableSelect(nTab) )
			{
				uno::Reference< sheet::XSpreadsheet > xSheet( xIndex->getByIndex( nTab ), uno::UNO_QUERY_THROW );
				uno::Reference< container::XNamed > xNamed( xSheet, uno::UNO_QUERY_THROW );
				sheets.push_back( xSheet );
				namesToIndices[ xNamed->getName() ] = nIndex++;
			}
		}

	}
	
	//XEnumerationAccess
	virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException)
	{
		return new SelectedSheetsEnum( m_xContext, sheets, m_xModel  ); 
	}
	// XIndexAccess
	virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException) 
	{ 
		return sheets.size();
	}
	virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw ( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		if ( Index < 0 
			|| Index >= sheets.size() ) 
			throw lang::IndexOutOfBoundsException();
		
		//return makeAny( uno::Reference< vba::XWorksheet > ( new ScVbaWorksheet( m_xContext, sheets[ Index ], m_xModel ) ) );
		return makeAny( sheets[ Index ] );
	}

	//XElementAccess
	virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException)
	{ 
		return vba::XWorksheet::static_type(0); 
	}

	virtual ::sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException) 
	{ 
		return (sheets.size() > 0);
	}

	//XNameAccess
	virtual uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
	{ 
		NameIndexHash::const_iterator it = namesToIndices.find( aName );
		if ( it == namesToIndices.end() )
			throw container::NoSuchElementException();
		//return makeAny( uno::Reference< vba::XWorksheet > ( new ScVbaWorksheet( m_xContext, sheets[ it->second ], m_xModel ) ) );
		return makeAny( sheets[ it->second ] );
		
	}

	virtual uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (uno::RuntimeException) 
	{ 
		uno::Sequence< ::rtl::OUString > names( namesToIndices.size() );
		::rtl::OUString* pString = names.getArray();
		NameIndexHash::const_iterator it = namesToIndices.begin();
		NameIndexHash::const_iterator it_end = namesToIndices.end();
		for ( ; it != it_end; ++it, ++pString )
			*pString = it->first;	
		return names;	
	}

	virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (uno::RuntimeException) 
	{ 
		NameIndexHash::const_iterator it = namesToIndices.find( aName );
		return (it != namesToIndices.end());
	}


};


void  
ScVbaWindow::Scroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft, bool bLargeScroll ) throw (uno::RuntimeException)
{
	sal_Int16 down = 0;	
	sal_Int16 up = 0;	
	sal_Int16 toRight = 0;	
	sal_Int16 toLeft = 0;	
	Down >>= down;
	Up >>= up;
	ToRight >>= toRight;
	ToLeft >>= toLeft;
	uno::Sequence< beans::PropertyValue > args1(2);
	args1[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "By" ) );
	args1[1].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Sel" ) );
	args1[1].Value <<= false;
	
	sal_Int16 totalUp = up - down;
	sal_Int16 totalLeft = toLeft - toRight;
	
	if ( totalUp != 0 )
	{
		args1[0].Value <<= totalUp;
		rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:GoUp"));
		if ( bLargeScroll )
			url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:GoUpBlock"));
		uno::Reference< frame::XModel > xModel = getCurrentDocument();
		dispatchRequests( xModel, url, args1 );
	}
	
	if ( totalLeft != 0 )
	{
		args1[0].Value <<= totalLeft;
		rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:GoLeft"));
		if ( bLargeScroll )
			url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "slot:")) + rtl::OUString::valueOf( (sal_Int32)SID_CURSORPAGELEFT_ );
		uno::Reference< frame::XModel > xModel = getCurrentDocument();
		dispatchRequests( xModel, url, args1 );
	}

}
void SAL_CALL 
ScVbaWindow::SmallScroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft ) throw (uno::RuntimeException)
{
	Scroll( Down, Up, ToRight, ToLeft );
}
void SAL_CALL 
ScVbaWindow::LargeScroll( const uno::Any& Down, const uno::Any& Up, const uno::Any& ToRight, const uno::Any& ToLeft ) throw (uno::RuntimeException)
{
	Scroll( Down, Up, ToRight, ToLeft, true );
}

uno::Any SAL_CALL 
ScVbaWindow::SelectedSheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	uno::Reference< container::XEnumerationAccess > xEnumAccess( new SelectedSheetsEnumAccess( m_xContext, getCurrentDocument()  ) );
	return makeAny( uno::Reference< vba::XWorksheets > ( new ScVbaWorksheets( m_xContext, xEnumAccess, getCurrentDocument() ) ) ); 	
}
