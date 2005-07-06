#include <comphelper/processfactory.hxx>

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>

#include <sfx2/objsh.hxx>

#include "vbaglobals.hxx"
#include "vbaworkbook.hxx"
#include "vbaworkbooks.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

static uno::Any
getWorkbook( const uno::Reference< sheet::XSpreadsheetDocument > &xDoc )
{
	// FIXME: fine as long as ScVbaWorkbook is stateless ...
	uno::Reference< frame::XModel > xModel( xDoc, uno::UNO_QUERY );
	if( !xModel.is() )
		return uno::Any();

	ScVbaWorkbook *pWb = new ScVbaWorkbook( xModel );
	return uno::Any( uno::Reference< vba::XWorkbook > (pWb) );
}

typedef ::cppu::WeakImplHelper1< com::sun::star::container::XEnumeration > ScVbaWorkbooksIterator_BASE;

class ScVbaWorkbooksIterator : public ScVbaWorkbooksIterator_BASE
{
	uno::Reference< container::XEnumeration > mxComponents;
	uno::Reference< sheet::XSpreadsheetDocument > mxSpreadsheet;
  public:
	ScVbaWorkbooksIterator()
	{
		uno::Reference< lang::XMultiServiceFactory > xSMgr = ::comphelper::getProcessServiceFactory();
		uno::Reference< frame::XDesktop > xDesktop
			(xSMgr->createInstance(::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop")), uno::UNO_QUERY_THROW );
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
		return getWorkbook( xNext );
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
	return uno::Reference< container::XEnumeration >( new ScVbaWorkbooksIterator() );
}

// XCollection
uno::Any
ScVbaWorkbooks::getParent() throw (uno::RuntimeException)
{
	return uno::Any( ScVbaGlobals::get()->getApplication() );
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
	return ScVbaGlobals::get()->getApplication();
}
::sal_Int32
ScVbaWorkbooks::getCount() throw (uno::RuntimeException)
{
	sal_Int32 nCount;
	ScVbaWorkbooksIterator aIter;

	for( nCount = 0; aIter.hasMoreElements(); nCount++ )
		aIter.nextElem();

	return nCount;
}

uno::Any
ScVbaWorkbooks::Item( ::sal_Int32 nIndex ) throw (uno::RuntimeException)
{
	ScVbaWorkbooksIterator aIter;
	uno::Reference< sheet::XSpreadsheetDocument > xDoc;

	for( sal_Int32 i = 0; i <= nIndex; i++ )
		xDoc = aIter.nextElem();

	if( !xDoc.is() )
		return uno::Any( uno::Reference< vba::XWorkbook >(NULL) );
	else
		return uno::Any( getWorkbook( xDoc ) );
}

// XWorkbooks
uno::Any
ScVbaWorkbooks::Add( const uno::Any& Before, const uno::Any& After,
					 const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
	return uno::Any();
}
void
ScVbaWorkbooks::Copy( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
void
ScVbaWorkbooks::Move( const uno::Any& Before, const uno::Any& After ) throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
void
ScVbaWorkbooks::Delete() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
