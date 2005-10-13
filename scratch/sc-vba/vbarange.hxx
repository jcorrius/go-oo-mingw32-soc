#ifndef SC_VBA_RANGE_HXX
#define SC_VBA_RANGE_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XRange.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <org/openoffice/vba/XFont.hpp>
#include "org/openoffice/vba/xlPasteType.hdl"
#include "org/openoffice/vba/xlPasteSpecialOperation.hdl"

#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>

#include "vbahelper.hxx"

class ScTableSheetsObj;

typedef ::cppu::WeakImplHelper1< org::openoffice::vba::XRange > ScVbaRange_BASE;

const sal_Int32 RANGE_PROPERTY_ID_DFLT=1;
// name is not defineable in IDL so no chance of a false detection of the 
// another property/method of the same name
const ::rtl::OUString RANGE_PROPERTY_DFLT( RTL_CONSTASCII_USTRINGPARAM( "_$DefaultProp" ) );

class ScVbaRange : public ScVbaRange_BASE
//    ,public ::comphelper::OMutexAndBroadcastHelper
//    ,public ::comphelper::OPropertyContainer
//    ,public ::comphelper::OPropertyArrayUsageHelper< ScVbaRange >

{
	uno::Reference< table::XCellRange > mxRange;
	uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
	sal_Bool mbIsRows;
	sal_Bool mbIsColumns;
	rtl::OUString msDftPropName;

public:
	ScVbaRange( uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext, uno::Reference< table::XCellRange > xRange, sal_Bool bIsRows = false, sal_Bool bIsColumns = false ) 
//:  OPropertyContainer(GetBroadcastHelper())
:mxRange( xRange ),
		m_xContext(xContext),
		mbIsRows( bIsRows ),
		mbIsColumns( bIsColumns )
{
	msDftPropName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Value" ) );
//	registerProperty( RANGE_PROPERTY_DFLT, RANGE_PROPERTY_ID_DFLT,
//        beans::PropertyAttribute::TRANSIENT | beans::PropertyAttribute::BOUND, &msDftPropName, ::getCppuType( &msDftPropName ) );

}
	virtual ~ScVbaRange() {}

    // Attributes
	virtual uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
	virtual void   SAL_CALL setValue( const uno::Any& aValue ) throw ( uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFormula() throw (uno::RuntimeException);
	virtual void   SAL_CALL setFormula( const ::rtl::OUString &rFormula ) throw (uno::RuntimeException);
	virtual double SAL_CALL getCount() throw (uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getRow() throw (uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getColumn() throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getText() throw (uno::RuntimeException);
	virtual void   SAL_CALL setText( const ::rtl::OUString &rString ) throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFormulaArray() throw (uno::RuntimeException);
	virtual void   SAL_CALL setFormulaArray(const ::rtl::OUString &rFormula) throw (uno::RuntimeException);
	virtual void SAL_CALL setNumberFormat( const ::rtl::OUString &rNumberFormat ) throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getNumberFormat() throw (uno::RuntimeException);
	virtual void SAL_CALL setMergeCells( sal_Bool bMerge ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getMergeCells() throw (uno::RuntimeException);
	virtual void SAL_CALL setWrapText( sal_Bool bIsWrapped ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getWrapText() throw (uno::RuntimeException);

	// Methods
	sal_Bool IsRows() { return mbIsRows; }
	sal_Bool IsColumns() { return mbIsColumns; }
	virtual void SAL_CALL Clear() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearComments() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearContents() throw (uno::RuntimeException);
	virtual void SAL_CALL ClearFormats() throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL HasFormula() throw (uno::RuntimeException);
	virtual void SAL_CALL FillLeft() throw (uno::RuntimeException);
	virtual void SAL_CALL FillRight() throw (uno::RuntimeException);
	virtual void SAL_CALL FillUp() throw (uno::RuntimeException);
	virtual void SAL_CALL FillDown() throw (uno::RuntimeException);
	virtual	uno::Reference< vba::XRange > SAL_CALL Offset( const ::uno::Any &nRowOffset, const ::uno::Any &nColOffset )
														   throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL CurrentRegion() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL CurrentArray() throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL Characters( const ::uno::Any& nIndex, const ::uno::Any& nCount ) 
												 throw (uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL Address() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL Cells( const ::uno::Any &nRow, const ::uno::Any &nCol ) 
														  throw (uno::RuntimeException);
	virtual void SAL_CALL Select() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange >  SAL_CALL Rows( const uno::Any& nIndex ) throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange >  SAL_CALL Columns( const uno::Any &nIndex ) throw (uno::RuntimeException);
	virtual void SAL_CALL Copy( const ::uno::Any& Destination ) throw (uno::RuntimeException);
	virtual void SAL_CALL Cut( const ::uno::Any& Destination ) throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL Resize( const ::uno::Any& RowSize, const ::uno::Any& ColumnSize ) 
														   throw (uno::RuntimeException);
	virtual uno::Reference< vba::XFont > SAL_CALL Font() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XInterior > SAL_CALL Interior(  ) throw (::com::sun::star::uno::RuntimeException) ;
	virtual uno::Reference< vba::XRange > SAL_CALL Range( const uno::Any &Cell1, const uno::Any &Cell2 )
															throw (uno::RuntimeException);
	virtual ::com::sun::star::uno::Any SAL_CALL getCellRange(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL PasteSpecial( sal_Int16 Paste, sal_Int16 Operation, ::sal_Bool SkipBlanks, ::sal_Bool Transpose ) throw (::com::sun::star::uno::RuntimeException);
	// XPropertySet
/*
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (::com::sun::star::uno::RuntimeException);  

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()
protected:
	// OPropertySetHelper
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

	// OPropertyArrayUsageHelper
	virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;
*/
};

#endif /* SC_VBA_RANGE_HXX */

