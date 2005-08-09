#include <comphelper/processfactory.hxx>

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/beans/PropertyVetoException.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>

#include <sfx2/objsh.hxx>
#include <tools/urlobj.hxx>

#include "vbaglobals.hxx"
#include "vbaworkbook.hxx"
#include "vbaworkbooks.hxx"
#include "vbahelper.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;
static uno::Any
getWorkbook( uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< sheet::XSpreadsheetDocument > &xDoc )
{
	// FIXME: fine as long as ScVbaWorkbook is stateless ...
	uno::Reference< frame::XModel > xModel( xDoc, uno::UNO_QUERY );
	if( !xModel.is() )
		return uno::Any();

	ScVbaWorkbook *pWb = new ScVbaWorkbook( xContext, xModel );
	return uno::Any( uno::Reference< vba::XWorkbook > (pWb) );
}

typedef ::cppu::WeakImplHelper1< com::sun::star::container::XEnumeration > ScVbaWorkbooksIterator_BASE;

class ScVbaWorkbooksIterator : public ScVbaWorkbooksIterator_BASE
{
	uno::Reference< container::XEnumeration > mxComponents;
	uno::Reference< sheet::XSpreadsheetDocument > mxSpreadsheet;
	uno::Reference< uno::XComponentContext > m_xContext;
  public:
	ScVbaWorkbooksIterator( uno::Reference< uno::XComponentContext >& xContext)
		: m_xContext( xContext )
	{
		uno::Reference< lang::XMultiComponentFactory > xSMgr(
			m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );

		uno::Reference< frame::XDesktop > xDesktop
			(xSMgr->createInstanceWithContext(::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop"), m_xContext), uno::UNO_QUERY_THROW );
		mxComponents = xDesktop->getComponents()->createEnumeration();
		nextElem();
	}
	~ScVbaWorkbooksIterator() {}

	virtual sal_Bool hasMoreElements() throw (uno::RuntimeException)
	{
		return mxSpreadsheet.is();
	}
	
	virtual ::com::sun::star::uno::Any nextElement() throw (container::NoSuchElementException,
															lang::WrappedTargetException,
															uno::RuntimeException)
	{
		uno::Reference< sheet::XSpreadsheetDocument > xNext = nextElem();
		if( !xNext.is() )
			throw container::NoSuchElementException();
		return getWorkbook( m_xContext, xNext );
	}

	uno::Reference< sheet::XSpreadsheetDocument > nextElem() throw (uno::RuntimeException)
	{
		uno::Reference< sheet::XSpreadsheetDocument > xLast = mxSpreadsheet;
		mxSpreadsheet.clear();

		while( !mxSpreadsheet.is() &&
			   mxComponents.is() && mxComponents->hasMoreElements() )
		{
			uno::Any aElem = mxComponents->nextElement();
			mxSpreadsheet = uno::Reference< sheet::XSpreadsheetDocument >( aElem, uno::UNO_QUERY );
		}
		return xLast;
	}
};

// XEnumerationAccess
uno::Type
ScVbaWorkbooks::getElementType() throw (uno::RuntimeException)
{
	return vba::XWorkbook::static_type(0);
}
::sal_Bool
ScVbaWorkbooks::hasElements() throw (uno::RuntimeException)
{
	return sal_True;
}
uno::Reference< container::XEnumeration >
ScVbaWorkbooks::createEnumeration() throw (uno::RuntimeException)
{
	return uno::Reference< container::XEnumeration >( new ScVbaWorkbooksIterator(m_xContext) );
}


// XCollection
uno::Any
ScVbaWorkbooks::getParent() throw (uno::RuntimeException)
{
	uno::Reference< vba::XGlobals > xGlobals = ScVbaGlobals::getGlobalsImpl( m_xContext );
	uno::Reference< vba::XApplication > xApplication = 
		xGlobals->getApplication();
	if ( !xApplication.is() )
	{
		throw uno::RuntimeException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScVbaWorkbooks::getParent: Couldn't access Application object" ) ),
                    uno::Reference< XInterface >() );
	}
	return uno::Any(xApplication);
}

::sal_Int32
ScVbaWorkbooks::getCreator() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return 0;
}
uno::Reference< vba::XApplication >
ScVbaWorkbooks::getApplication() throw (uno::RuntimeException)
{
	uno::Reference< vba::XApplication > xApplication = 
		ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();

	if ( !xApplication.is() )
	{
		throw uno::RuntimeException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScVbaWorkbooks::getParent: Couldn't access Application object" ) ),
                    uno::Reference< XInterface >() );
	}
	return xApplication;
}
::sal_Int32
ScVbaWorkbooks::getCount() throw (uno::RuntimeException)
{
	sal_Int32 nCount;
	ScVbaWorkbooksIterator aIter(m_xContext);

	for( nCount = 0; aIter.hasMoreElements(); nCount++ )
		aIter.nextElem();

	return nCount;
}

uno::Any
ScVbaWorkbooks::getItemByStringIndex( const rtl::OUString& sIndex ) throw (uno::RuntimeException)
{
	if ( !( sIndex.getLength() > 0 ) )
	{
		rtl::OUString message;
		message = rtl::OUString::createFromAscii(
			"index is a blank string");
		throw  lang::IndexOutOfBoundsException( message,
			uno::Reference< uno::XInterface >() );
	}

	ScVbaWorkbooksIterator aIter(m_xContext);
	uno::Reference< sheet::XSpreadsheetDocument > xDoc;

	bool isFound = false;
	// FIXME check if there is a XNameAccess for this, quick look says
	// no, need to check this later. so. atm very inefficient below
	uno::Reference< vba::XWorkbook > xWorkBook;
	while ( aIter.hasMoreElements() )
	{
		xDoc = aIter.nextElem();
		xWorkBook.set( getWorkbook( m_xContext, xDoc ), uno::UNO_QUERY );
		if ( xWorkBook.is() )
		{
			if ( sIndex.equals( xWorkBook->getName() ) )
			{
				isFound = true;
				break;
			}
		}
	}

	if ( !isFound )
	{
		rtl::OUString message;
		message = rtl::OUString::createFromAscii(
			"Element does not exist for index ");
		throw  lang::IndexOutOfBoundsException( message,
			uno::Reference< uno::XInterface >() );
	}

	return uno::Any( xWorkBook );	

}

uno::Any
ScVbaWorkbooks::getItemByIntIndex( const sal_Int32 nIndex ) throw (uno::RuntimeException)
{

	if ( !( nIndex > 0 ) )
	{
		rtl::OUString message;
		message = rtl::OUString::createFromAscii(
			"index is 0 or negative");
		throw  lang::IndexOutOfBoundsException( message,
			uno::Reference< uno::XInterface >() );
	}

	ScVbaWorkbooksIterator aIter(m_xContext);
	uno::Reference< sheet::XSpreadsheetDocument > xDoc;

	sal_Int32 i = 0;
	for( ; aIter.hasMoreElements() && i < nIndex; ++i )
	{
		xDoc = aIter.nextElem();
	}

	if ( i != nIndex )
	{
		rtl::OUString message;
		message = rtl::OUString::createFromAscii(
			"Element does not exist for index ");
		throw  lang::IndexOutOfBoundsException( message,
			uno::Reference< uno::XInterface >() );
	}

	if( !xDoc.is() )
		return uno::Any( uno::Reference< vba::XWorkbook >(NULL) );

	return uno::Any( getWorkbook( m_xContext, xDoc ) );
}

uno::Any
ScVbaWorkbooks::Item( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	if ( aIndex.getValueTypeClass() ==  uno::TypeClass_STRING )
	{
		rtl::OUString sIndex;
		if ( ( aIndex >>= sIndex ) != sal_True )
		{
			rtl::OUString message;
			message = rtl::OUString::createFromAscii(
				"Couldn't convert index to string");
			throw uno::RuntimeException( message, uno::Reference< uno::XInterface >() );
		}
		return getItemByStringIndex( sIndex );

	}

	sal_Int32 nIndex = 0;

	if ( ( aIndex >>= nIndex ) != sal_True )
	{
		rtl::OUString message;
		message = rtl::OUString::createFromAscii(
			"Couldn't convert index to Int32");
		throw uno::RuntimeException( message, uno::Reference< uno::XInterface >() );
	}
	return getItemByIntIndex( nIndex );
}

uno::Any 
ScVbaWorkbooks::Add() throw (uno::RuntimeException)
{
	uno::Reference< lang::XMultiComponentFactory > xSMgr(
        m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );

	 uno::Reference< frame::XComponentLoader > xLoader(
        xSMgr->createInstanceWithContext(
            ::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop"),
                m_xContext), uno::UNO_QUERY_THROW );
	uno::Reference< lang::XComponent > xComponent = xLoader->loadComponentFromURL(
									   rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("private:factory/scalc") ),
									   rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("_blank") ), 0, 
									   uno::Sequence< beans::PropertyValue >(0) );			   
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xComponent, uno::UNO_QUERY_THROW );
    uno::Reference<frame::XModel> xNewModel(xSpreadDoc,uno::UNO_QUERY_THROW);
                                                                                                                             
    if( xSpreadDoc.is() )
        return uno::Any( uno::Reference< vba::XWorkbook >( new ScVbaWorkbook( m_xContext, xNewModel ) ) );
    return uno::Any();
}

void
ScVbaWorkbooks::Close() throw (uno::RuntimeException)
{
	uno::Reference< lang::XMultiComponentFactory > xSMgr(
		m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );
	uno::Reference< frame::XDesktop > xDesktop
		(xSMgr->createInstanceWithContext(
		::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop"),
		m_xContext), uno::UNO_QUERY_THROW );
	uno::Reference< lang::XComponent > xDoc = xDesktop->getCurrentComponent();
	uno::Reference< frame::XModel > xModel( xDoc, uno::UNO_QUERY);
	rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:CloseDoc"));
	dispatchRequests(xModel,url);
}

uno::Any
ScVbaWorkbooks::Open( const uno::Any &aFileName ) throw (uno::RuntimeException)
{
	rtl::OUString aURL(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("file://") ));
	rtl::OUString aTempName;
	aFileName >>= aTempName;
	aURL += aTempName;

	uno::Reference< lang::XMultiComponentFactory > xSMgr(
        	m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );

	uno::Reference< frame::XDesktop > xDesktop
		(xSMgr->createInstanceWithContext(::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop")                    , m_xContext),
		uno::UNO_QUERY_THROW );
	uno::Reference< frame::XComponentLoader > xLoader(
		xSMgr->createInstanceWithContext(
		::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop"),
		m_xContext),
		uno::UNO_QUERY_THROW );
	uno::Reference< lang::XComponent > xComponent = xLoader->loadComponentFromURL( aURL,
		rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("_default") ),
		frame::FrameSearchFlag::CREATE,
		uno::Sequence< beans::PropertyValue >(0));
	uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xComponent, uno::UNO_QUERY_THROW );
	uno::Reference<frame::XModel> xNewModel(xSpreadDoc,uno::UNO_QUERY_THROW);

	return uno::Any( uno::Reference< vba::XWorkbook >( new ScVbaWorkbook( m_xContext, xNewModel ) ) );
}
