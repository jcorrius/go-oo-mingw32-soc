#include <sfx2/objsh.hxx>

#include<com/sun/star/sheet/XCellRangeAddressable.hpp>
#include<com/sun/star/sheet/XCellAddressable.hpp>
#include<com/sun/star/table/XCell.hpp>
#include<com/sun/star/table/XCellRange.hpp>
#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>
#include<com/sun/star/sheet/XViewPane.hpp>
#include<com/sun/star/table/CellRangeAddress.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaApplication::ScVbaApplication()
{
}

ScVbaApplication::~ScVbaApplication()
{
}


uno::Reference< vba::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
	SfxObjectShell* pDoc = SfxObjectShell::Current();
	uno::Reference< frame::XModel > xModel;

	if( pDoc )
		xModel = pDoc->GetModel();

	if( xModel.is() )
		return uno::Reference< vba::XWorkbook >(new ScVbaWorkbook( xModel ) );
	else
		return uno::Reference< vba::XWorkbook >( NULL );
}

uno::Reference< vba::XRange >
ScVbaApplication::getSelection() throw (uno::RuntimeException)
{
	SfxObjectShell* pDoc = SfxObjectShell::Current();
    uno::Reference< frame::XModel > xModel;
                                                                                                                             
    if( pDoc )
        xModel = pDoc->GetModel();
                                                                                                                             
    uno::Reference< table::XCellRange > xRange( xModel->getCurrentSelection(), ::uno::UNO_QUERY);

	return uno::Reference< vba::XRange >( new ScVbaRange( xRange ) );
}

uno::Reference< vba::XWorkbooks >
ScVbaApplication::getWorkbooks() throw (uno::RuntimeException)
{
	return uno::Reference< vba::XWorkbooks >( new ScVbaWorkbooks() );
}

