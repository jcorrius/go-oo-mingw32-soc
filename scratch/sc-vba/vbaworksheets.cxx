#include <cppuhelper/queryinterface.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <sfx2/objsh.hxx>

#include "vbaglobals.hxx"
#include "vbaworksheet.hxx"
#include "vbaworksheets.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XInterface implementation
uno::Any ScVbaWorksheets::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XWorksheets * >( this ) ),
			 static_cast< vba::XWorksheets * >( this ),
			 static_cast< container::XEnumerationAccess * >( this ) );
}


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
	return uno::Reference< container::XEnumeration >( NULL );
}

// XCollection
uno::Any
ScVbaWorksheets::getParent() throw (uno::RuntimeException)
{
	return uno::Any( ScVbaGlobals::get()->getApplication() );
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
	return ScVbaGlobals::get()->getApplication();
}
::sal_Int32
ScVbaWorksheets::getCount() throw (uno::RuntimeException)
{

        SfxObjectShell* pObjSh = SfxObjectShell::Current();
        if ( pObjSh )
        {
            uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( pObjSh->GetModel(), uno::UNO_QUERY );
            if ( xSpreadDoc.is() )
            {
                uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		if (xSheets.is())
		{
			uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
			if ( xIndex.is() )
			{
				return xIndex->getCount();
			}
		}
            }
        }

	return 0;
}

uno::Any
ScVbaWorksheets::Item( ::sal_Int32 nIndex ) throw (uno::RuntimeException)
{
	return uno::Any( uno::Reference< vba::XWorksheet >(NULL) );
}

// XWorksheets
uno::Any
ScVbaWorksheets::Add( const uno::Any& Before, const uno::Any& After,
					 const uno::Any& Count, const uno::Any& Type ) throw (uno::RuntimeException)
{
	rtl::OUString aStringSheet;
	Before >>= aStringSheet;
	sal_Int32 nSheetIndex = -1;
	sal_Int32 nNewSheets;
	Count >>= nNewSheets; 
	sal_Int32 nCount = 0;

	//FIXME: Handle 'After' 
	//FIXME: Need to see if it makes sense to get the current shell
        SfxObjectShell* pObjSh = SfxObjectShell::Current();
        if ( pObjSh )
        {
            uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( pObjSh->GetModel(), uno::UNO_QUERY );
            if ( xSpreadDoc.is() )
            {
                uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
		if (xSheets.is())
		{
			uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
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
		}
		if(nSheetIndex != -1)
		{
			//FIXME: Default index case, if Before/After not specified
			fprintf(stderr, "nSheetIndex:: %d\n",  nSheetIndex);
			for (sal_Int32 i=0; i < nNewSheets; i++)
			{
		                String aStringName( RTL_CONSTASCII_USTRINGPARAM("Sheet") );
				aStringName += String::CreateFromInt32(nCount + i + 1L);
				fprintf(stderr, "aStringName :: %s\n",  rtl::OUStringToOString(aStringName, RTL_TEXTENCODING_UTF8).getStr());
				xSheets->insertNewByName(aStringName, nSheetIndex + i);
			}
		}
            }
        }
	return uno::Any();
}

void
ScVbaWorksheets::Delete() throw (uno::RuntimeException)
{
	SC_VBA_STUB();
}
