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

typedef ::cppu::WeakImplHelper1< oo::vba::XRange > ScVbaRange_BASE;

class ScVbaRange : public ScVbaRange_BASE
    ,public ::comphelper::OMutexAndBroadcastHelper
    ,public ::comphelper::OPropertyContainer

{
	css::uno::Reference< css::table::XCellRange > mxRange;
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	sal_Bool mbIsRows;
	sal_Bool mbIsColumns;
	rtl::OUString msDftPropName;
	css::uno::Reference< oo::vba::XRange > getEntireColumnOrRow( bool bEntireRow = true ) throw( css::uno::RuntimeException );

public:
	ScVbaRange( css::uno::Reference< css::uno::XComponentContext >& xContext, css::uno::Reference< css::table::XCellRange > xRange, sal_Bool bIsRows = false, sal_Bool bIsColumns = false ) throw ( css::lang::IllegalArgumentException );

	virtual ~ScVbaRange() {}

    // Attributes
	virtual css::uno::Any SAL_CALL getValue() throw (css::uno::RuntimeException);
	virtual void   SAL_CALL setValue( const css::uno::Any& aValue ) throw ( css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFormula() throw (css::uno::RuntimeException);
	virtual void   SAL_CALL setFormula( const ::rtl::OUString &rFormula ) throw (css::uno::RuntimeException);
	virtual double SAL_CALL getCount() throw (css::uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getRow() throw (css::uno::RuntimeException);
	virtual ::sal_Int32 SAL_CALL getColumn() throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getText() throw (css::uno::RuntimeException);
	virtual void   SAL_CALL setText( const ::rtl::OUString &rString ) throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFormulaArray() throw (css::uno::RuntimeException);
	virtual void   SAL_CALL setFormulaArray(const ::rtl::OUString &rFormula) throw (css::uno::RuntimeException);
	virtual void SAL_CALL setNumberFormat( const ::rtl::OUString &rNumberFormat ) throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getNumberFormat() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setMergeCells( sal_Bool bMerge ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getMergeCells() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setWrapText( sal_Bool bIsWrapped ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getWrapText() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL getEntireRow() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL getEntireColumn() throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getHidden() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setHidden( const css::uno::Any& _hidden ) throw (css::uno::RuntimeException);
	// Methods
	sal_Bool IsRows() { return mbIsRows; }
	sal_Bool IsColumns() { return mbIsColumns; }
	virtual void SAL_CALL Clear() throw (css::uno::RuntimeException);
	virtual void SAL_CALL ClearComments() throw (css::uno::RuntimeException);
	virtual void SAL_CALL ClearContents() throw (css::uno::RuntimeException);
	virtual void SAL_CALL ClearFormats() throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL HasFormula() throw (css::uno::RuntimeException);
	virtual void SAL_CALL FillLeft() throw (css::uno::RuntimeException);
	virtual void SAL_CALL FillRight() throw (css::uno::RuntimeException);
	virtual void SAL_CALL FillUp() throw (css::uno::RuntimeException);
	virtual void SAL_CALL FillDown() throw (css::uno::RuntimeException);
	virtual	css::uno::Reference< oo::vba::XRange > SAL_CALL Offset( const css::uno::Any &nRowOffset, const css::uno::Any &nColOffset )
														   throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL CurrentRegion() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL CurrentArray() throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL Characters( const css::uno::Any& nIndex, const css::uno::Any& nCount ) 
												 throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL Address() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL Cells( const css::uno::Any &nRow, const css::uno::Any &nCol ) 
														  throw (css::uno::RuntimeException);
	virtual void SAL_CALL Select() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange >  SAL_CALL Rows( const css::uno::Any& nIndex ) throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange >  SAL_CALL Columns( const css::uno::Any &nIndex ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Copy( const css::uno::Any& Destination ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Cut( const css::uno::Any& Destination ) throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL Resize( const css::uno::Any& RowSize, const css::uno::Any& ColumnSize ) 
														   throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XFont > SAL_CALL Font() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XInterior > SAL_CALL Interior(  ) throw (css::uno::RuntimeException) ;
	virtual css::uno::Reference< oo::vba::XRange > SAL_CALL Range( const css::uno::Any &Cell1, const css::uno::Any &Cell2 )
															throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getCellRange(  ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL PasteSpecial( sal_Int16 Paste, sal_Int16 Operation, ::sal_Bool SkipBlanks, ::sal_Bool Transpose ) throw (css::uno::RuntimeException);
	// XPropertySet

	virtual css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (css::uno::RuntimeException);  

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()
protected:
	// OPropertySetHelper
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

	virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;

};

#endif /* SC_VBA_RANGE_HXX */

