#include <comphelper/processfactory.hxx>
#include <sfx2/objsh.hxx>

#include <com/sun/star/sheet/XSheetOperation.hpp>
#include <com/sun/star/sheet/CellFlags.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/sheet/XCellAddressable.hpp>
#include <com/sun/star/table/CellContentType.hpp>
#include <com/sun/star/sheet/XCellSeries.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSheetCellCursor.hpp>
#include <com/sun/star/sheet/XArrayFormulaRange.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/sheet/XFunctionAccess.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/table/XCellCursor.hpp>
#include <com/sun/star/table/XTableRows.hpp>
#include <com/sun/star/table/XTableColumns.hpp>
#include <com/sun/star/util/XMergeable.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/util/XNumberFormats.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/util/XReplaceable.hpp>
#include <com/sun/star/sheet/XCellRangeMovement.hpp>
#include <com/sun/star/sheet/XCellRangeData.hpp>

#include <org/openoffice/vba/xlPasteSpecialOperation.hpp>
#include <org/openoffice/vba/xlPasteType.hpp>
#include <org/openoffice/vba/Excel/Constants.hpp>
#include <org/openoffice/vba/Excel/XlFindLookIn.hpp>
#include <org/openoffice/vba/Excel/XlLookAt.hpp>
#include <org/openoffice/vba/Excel/XlSearchOrder.hpp>

#include "vbarange.hxx"
#include "vbafont.hxx"
#include "vbacomment.hxx"
#include "vbainterior.hxx"
#include "vbaarraywrapper.hxx"

#include <comphelper/anytostring.hxx>

#include "global.hxx"
#include <vector>

using namespace ::org::openoffice;
using namespace ::com::sun::star;


class NumFormatHelper
{
	uno::Reference< util::XNumberFormatsSupplier > mxSupplier;
	uno::Reference< beans::XPropertySet > mxRangeProps;
	uno::Reference< util::XNumberFormats > mxFormats;
public:
	NumFormatHelper( const uno::Reference< table::XCellRange >& xRange )
	{
		mxSupplier.set( getCurrentDocument(), uno::UNO_QUERY_THROW );
		mxRangeProps.set( xRange, uno::UNO_QUERY_THROW);
		mxFormats = mxSupplier->getNumberFormats();
	}
	uno::Reference< beans::XPropertySet > getNumberProps()
	{	
		long nIndexKey;
		uno::Any aValue = mxRangeProps->getPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("NumberFormat")));
		aValue >>= nIndexKey;

		if ( mxFormats.is() )
			return  mxFormats->getByKey( nIndexKey );
		return	uno::Reference< beans::XPropertySet > ();
	}

	bool isBooleanType()
	{
	
		if ( getNumberFormat() & util::NumberFormat::LOGICAL )
			return true;
		return false;
	}

	rtl::OUString getNumberFormatString()
	{
		uno::Reference< beans::XPropertySet > xNumberProps( getNumberProps(), uno::UNO_QUERY_THROW );
		::rtl::OUString aFormatString;
		uno::Any aString = xNumberProps->getPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("FormatString")));
		aString >>= aFormatString;
		return aFormatString;
	}

	sal_Int16 getNumberFormat()
	{
		uno::Reference< beans::XPropertySet > xNumberProps = getNumberProps(); 	
		sal_Int16 nType = ::comphelper::getINT16(
        	xNumberProps->getPropertyValue( ::rtl::OUString::createFromAscii( "Type" ) ) );
		return nType;
	}

	bool setNumberFormat( sal_Int16 nType )
	{
		uno::Reference< beans::XPropertySet > xNumberProps = getNumberProps(); 	
		lang::Locale aLocale;
		xNumberProps->getPropertyValue( ::rtl::OUString::createFromAscii( "Locale" ) ) >>= aLocale;
		uno::Reference<util::XNumberFormatTypes> xTypes( mxFormats, uno::UNO_QUERY );
		if ( xTypes.is() )
		{
			sal_Int32 nNewIndex = xTypes->getStandardFormat( nType, aLocale );
       		mxRangeProps->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("NumberFormat") ), uno::makeAny( nNewIndex ) );				
			return true;
		}
		return false;
	}

};

struct CellPos
{
	CellPos():m_nRow(-1), m_nCol(-1) {};
	CellPos( sal_Int32 nRow, sal_Int32 nCol ):m_nRow(nRow), m_nCol(nCol) {};
sal_Int32 m_nRow;
sal_Int32 m_nCol;
};

typedef ::cppu::WeakImplHelper1< container::XEnumeration > CellsEnumeration_BASE;
typedef vector< CellPos > vCellPos;

class CellsEnumeration : public CellsEnumeration_BASE
{
	uno::Reference< table::XCellRange > m_xRange;
	uno::Reference< uno::XComponentContext > m_xContext;
	vCellPos m_CellPositions;	
	vCellPos::const_iterator m_it; 
public:
	CellsEnumeration( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< table::XCellRange >& xRange ): m_xContext( xContext ), m_xRange( xRange )
	{
		uno::Reference< table::XColumnRowRange > xColumnRowRange(m_xRange, uno::UNO_QUERY_THROW );
		sal_Int32 nRowCount =  xColumnRowRange->getRows()->getCount();
		sal_Int32 nColCount = xColumnRowRange->getColumns()->getCount();
		for ( sal_Int32 i=0; i<nRowCount; ++i )
			for ( sal_Int32 j=0; j<nColCount; ++j )
				m_CellPositions.push_back( CellPos( i,j ) );
		m_it = m_CellPositions.begin();
	}
	virtual ::sal_Bool SAL_CALL hasMoreElements() throw (::uno::RuntimeException){ return m_it != m_CellPositions.end(); }

	virtual uno::Any SAL_CALL nextElement() throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
	{
		if ( !hasMoreElements() )
			throw container::NoSuchElementException();
		CellPos aPos = *(m_it)++;
		uno::Reference< table::XCellRange > xCellRange( m_xRange->getCellByPosition(  aPos.m_nCol, aPos.m_nRow ), uno::UNO_QUERY_THROW );
		return makeAny( uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xCellRange ) ) );
	}
};


const sal_Int32 RANGE_PROPERTY_ID_DFLT=1;
// name is not defineable in IDL so no chance of a false detection of the
// another property/method of the same name
const ::rtl::OUString RANGE_PROPERTY_DFLT( RTL_CONSTASCII_USTRINGPARAM( "_$DefaultProp" ) );
const ::rtl::OUString ISVISIBLE(  RTL_CONSTASCII_USTRINGPARAM( "IsVisible"));

class CellValueSetter : public ArrayVisitor
{
protected:
	uno::Any maValue;
	uno::TypeClass mTypeClass;
	bool processValue( const uno::Any& aValue,  const uno::Reference< table::XCell >& xCell );
public:
	CellValueSetter( const uno::Any& aValue );
	virtual void visitNode( sal_Int32 x, sal_Int32 y, const uno::Reference< table::XCell >& xCell );
		
};

CellValueSetter::CellValueSetter( const uno::Any& aValue ): maValue( aValue ), mTypeClass( aValue.getValueTypeClass() ) {}

void
CellValueSetter::visitNode( sal_Int32 i, sal_Int32 j, const uno::Reference< table::XCell >& xCell )
{
	bool result = processValue( maValue, xCell );
}

bool
CellValueSetter::processValue( const uno::Any& aValue, const uno::Reference< table::XCell >& xCell )
{

	bool isExtracted = false;
	switch ( aValue.getValueTypeClass() )
	{
		case  uno::TypeClass_BOOLEAN:
		{
			sal_Bool bState;
			if ( aValue >>= bState 	 )
			{
				uno::Reference< table::XCellRange > xRange( xCell, uno::UNO_QUERY_THROW );
				if ( bState )
					xCell->setValue( (double) 1 );
				else
					xCell->setValue( (double) 0 );
				NumFormatHelper cellNumFormat( xRange );
				cellNumFormat.setNumberFormat( util::NumberFormat::LOGICAL );
			}
			break;
		}
		case uno::TypeClass_STRING:
		{
			rtl::OUString aString;
			if ( aValue >>= aString )
			{
				uno::Reference< text::XTextRange > xTextRange( xCell, uno::UNO_QUERY_THROW );
				xTextRange->setString( aString );
			}
			else
				isExtracted = false;	
			break;
		}
		default:
		{
			double nDouble;
			if ( aValue >>= nDouble )
				xCell->setValue( nDouble );
			else
				isExtracted = false;	
			break;
		}
	}
	return isExtracted;
		
}

class CellValueGetter : public ArrayVisitor
{
protected:
	uno::Any maValue;
	uno::TypeClass mTypeClass;
	virtual void processValue( sal_Int32 x, sal_Int32 y, const uno::Any& aValue );
public:
	CellValueGetter() {}
	virtual void visitNode( sal_Int32 x, sal_Int32 y, const uno::Reference< table::XCell >& xCell );
	const uno::Any& getValue() const { return maValue; }
		
};

void
CellValueGetter::processValue(  sal_Int32 x, sal_Int32 y, const uno::Any& aValue )
{
	maValue = aValue;
}
void CellValueGetter::visitNode( sal_Int32 x, sal_Int32 y, const uno::Reference< table::XCell >& xCell )
{
	uno::Any aValue;
	table::CellContentType eType = xCell->getType();
	if( eType == table::CellContentType_VALUE || eType == table::CellContentType_FORMULA )
	{
		if ( eType == table::CellContentType_FORMULA )
				aValue <<= xCell->getValue();
		else
		{
			uno::Reference< table::XCellRange > xRange( xCell, uno::UNO_QUERY_THROW );
			NumFormatHelper cellFormat( xRange );
			if ( cellFormat.isBooleanType() )
				aValue = uno::makeAny( ( xCell->getValue() != 0.0 ) );
			else
				aValue <<= xCell->getValue();
		}
	}
	if( eType == table::CellContentType_TEXT )
	{
		uno::Reference< text::XTextRange > xTextRange(xCell, ::uno::UNO_QUERY_THROW);
		aValue <<= xTextRange->getString();
	}
	processValue( x,y,aValue );
}

class Dim2ArrayValueGetter : public CellValueGetter
{
protected:
	virtual void processValue( sal_Int32 x, sal_Int32 y, const uno::Any& aValue )
	{
		uno::Sequence< uno::Sequence< uno::Any > >& aMatrix = *( uno::Sequence< uno::Sequence< uno::Any > >* )( maValue.getValue() );
		aMatrix[x][y] = aValue;
	}

public:
	Dim2ArrayValueGetter(sal_Int32 nRowCount, sal_Int32 nColCount ):CellValueGetter() 
	{
		uno::Sequence< uno::Sequence< uno::Any > > aMatrix;
		aMatrix.realloc( nRowCount );	
		for ( sal_Int32 index = 0; index < nRowCount; ++index )
			aMatrix[index].realloc( nColCount );
		maValue <<= aMatrix;
	}
};

const static rtl::OUString sNA = rtl::OUString::createFromAscii("#N/A"); 

class Dim1ArrayValueSetter : public CellValueSetter
{
	uno::Sequence< uno::Any > aMatrix;
	sal_Int32 nColCount;
public:
	Dim1ArrayValueSetter( const uno::Any& aValue ) : CellValueSetter( aValue )
	{
		aValue >>= aMatrix;
		nColCount = aMatrix.getLength();
	}
	virtual void visitNode( sal_Int32 x, sal_Int32 y, const uno::Reference< table::XCell >& xCell )
	{
		if ( y < nColCount )
			processValue( aMatrix[ y ], xCell );
		else
			processValue( uno::makeAny( sNA ), xCell );
	}
};



class Dim2ArrayValueSetter : public CellValueSetter
{
	uno::Sequence< uno::Sequence< uno::Any > > aMatrix;
	sal_Int32 nRowCount;
	sal_Int32 nColCount;
public:
	Dim2ArrayValueSetter( const uno::Any& aValue ) : CellValueSetter( aValue )
	{
		aValue >>= aMatrix;
		nRowCount = aMatrix.getLength();
		nColCount = aMatrix[0].getLength();  
	}
	virtual void visitNode( sal_Int32 x, sal_Int32 y, const uno::Reference< table::XCell >& xCell )
	{
		if ( x < nRowCount && y < nColCount )
			processValue( aMatrix[ x ][ y ], xCell );
		else
			processValue( uno::makeAny( sNA ), xCell );
	}
};

class RangeHelper
{
	uno::Reference< table::XCellRange > m_xCellRange;
	
public:
	RangeHelper( const uno::Reference< table::XCellRange >& xCellRange ) throw (uno::RuntimeException) : m_xCellRange( xCellRange ) 
	{
		if ( !m_xCellRange.is() )
			throw uno::RuntimeException();
	}
	RangeHelper( const uno::Any aCellRange ) throw (uno::RuntimeException)
	{
		m_xCellRange.set( aCellRange, uno::UNO_QUERY_THROW );
	}
	uno::Reference< sheet::XSheetCellRange > getSheetCellRange() throw (uno::RuntimeException)
	{
		return uno::Reference< sheet::XSheetCellRange >(m_xCellRange, uno::UNO_QUERY_THROW);
	}
	uno::Reference< sheet::XSpreadsheet >  getSpreadSheet() throw (uno::RuntimeException)
	{
		return getSheetCellRange()->getSpreadsheet();
	}	

	uno::Reference< table::XCellRange > getCellRangeFromSheet() throw (uno::RuntimeException)
	{
		return uno::Reference< table::XCellRange >(getSpreadSheet(), uno::UNO_QUERY_THROW );
	}

	uno::Reference< sheet::XCellRangeAddressable >  getCellRangeAddressable() throw (uno::RuntimeException)
	{
		return uno::Reference< sheet::XCellRangeAddressable >(m_xCellRange, ::uno::UNO_QUERY_THROW);

	}

	uno::Reference< sheet::XSheetCellCursor > getSheetCellCursor() throw ( uno::RuntimeException )
	{
		return 	uno::Reference< sheet::XSheetCellCursor >( getSpreadSheet()->createCursorByRange( getSheetCellRange() ), uno::UNO_QUERY_THROW );
	}	

	static uno::Reference< vba::XRange > createRangeFromRange( const uno::Reference<uno::XComponentContext >& xContext, const uno::Reference< table::XCellRange >& xRange, const uno::Reference< sheet::XCellRangeAddressable >& xCellRangeAddressable, sal_Int32 nStartColOffset = 0, sal_Int32 nStartRowOffset = 0,
 sal_Int32 nEndColOffset = 0, sal_Int32 nEndRowOffset = 0 )
	{
		return uno::Reference< vba::XRange >( new ScVbaRange( xContext, 
			xRange->getCellRangeByPosition(
				xCellRangeAddressable->getRangeAddress().StartColumn + nStartColOffset,
				xCellRangeAddressable->getRangeAddress().StartRow + nStartRowOffset,
				xCellRangeAddressable->getRangeAddress().EndColumn + nEndColOffset,
				xCellRangeAddressable->getRangeAddress().EndRow + nEndRowOffset ) ) );
	}
	
};

ScVbaRange::ScVbaRange( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< table::XCellRange >& xRange, sal_Bool bIsRows, sal_Bool bIsColumns ) throw( lang::IllegalArgumentException )
:OPropertyContainer(GetBroadcastHelper())
,mxRange( xRange ),
                m_xContext(xContext),
                mbIsRows( bIsRows ),
                mbIsColumns( bIsColumns )
{
	if  ( !xContext.is() )
		throw lang::IllegalArgumentException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "context is not set " ) ), uno::Reference< uno::XInterface >() , 1 );
	if  ( !xRange.is() )
		throw lang::IllegalArgumentException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "range is not set " ) ), uno::Reference< uno::XInterface >() , 1 );
	msDftPropName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Value" ) );
	registerProperty( RANGE_PROPERTY_DFLT, RANGE_PROPERTY_ID_DFLT,
beans::PropertyAttribute::TRANSIENT | beans::PropertyAttribute::BOUND, &msDftPropName, ::getCppuType( &msDftPropName ) );

}

uno::Reference< script::XTypeConverter >
ScVbaRange::getTypeConverter() throw (uno::RuntimeException)
{
	static uno::Reference< script::XTypeConverter > xTypeConv( m_xContext->getServiceManager()->createInstanceWithContext( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.script.Converter") ), m_xContext ), uno::UNO_QUERY_THROW );
	return xTypeConv;
}

void
ScVbaRange::visitArray( ArrayVisitor& visitor )
{
	uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, uno::UNO_QUERY_THROW );
	sal_Int32 nRowCount = xColumnRowRange->getRows()->getCount();
	sal_Int32 nColCount = xColumnRowRange->getColumns()->getCount();
	for ( sal_Int32 i=0; i<nRowCount; ++i )
	{
		for ( sal_Int32 j=0; j<nColCount; ++j )
		{
			uno::Reference< table::XCell > xCell( mxRange->getCellByPosition( j, i ), uno::UNO_QUERY_THROW );

			visitor.visitNode( i, j, xCell );
		}
	}
}




uno::Any SAL_CALL
ScVbaRange::getValue() throw (uno::RuntimeException)
{
	uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, uno::UNO_QUERY_THROW );
	sal_Int32 nRowCount = xColumnRowRange->getRows()->getCount();
	sal_Int32 nColCount = xColumnRowRange->getColumns()->getCount();
	// single cell range
	if ( nRowCount == 1 && nColCount == 1 )
	{
		CellValueGetter getter;
		visitArray( getter );
		return getter.getValue();
	}
	// multi cell range ( return array )
	Dim2ArrayValueGetter getter( nRowCount, nColCount );
	visitArray( getter );
	return makeAny( uno::Reference< vba::XArrayWrapper >( new ScArrayWrapper( getter.getValue(), sal_False ) ) );

}

void SAL_CALL
ScVbaRange::setValue( const uno::Any  &aValue  ) throw (uno::RuntimeException)
{
	uno::TypeClass aClass = aValue.getValueTypeClass();
	if ( aClass == uno::TypeClass_SEQUENCE )
	{
		uno::Reference< script::XTypeConverter > xConverter = getTypeConverter();
		uno::Any aConverted;
		try
		{
			// test for single dimension, could do 
			// with a better test than this	
			if ( aValue.getValueTypeName().indexOf('[') ==  aValue.getValueTypeName().lastIndexOf('[') )
			{
				aConverted = xConverter->convertTo( aValue, getCppuType((uno::Sequence< uno::Any >*)0) );
				Dim1ArrayValueSetter setter( aConverted );
				visitArray( setter );
			}
			else
			{
				aConverted = xConverter->convertTo( aValue, getCppuType((uno::Sequence< uno::Sequence< uno::Any > >*)0) );
				Dim2ArrayValueSetter setter( aConverted );
				visitArray( setter );
			}
		}
		catch ( uno::Exception& e )
		{
			OSL_TRACE("Bahhh, caught exception %s", 
				rtl::OUStringToOString( e.Message,
					RTL_TEXTENCODING_UTF8 ).getStr() );
		}
	}
	else
	{
		CellValueSetter setter( aValue );
		visitArray( setter );
	}
}

void
ScVbaRange::Clear() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY_THROW);
	xSheetOperation->clearContents(sheet::CellFlags::VALUE | sheet::CellFlags::STRING | 
					sheet::CellFlags::HARDATTR | sheet::CellFlags::FORMATTED | sheet::CellFlags::EDITATTR);
}

void
ScVbaRange::ClearComments() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY_THROW);
	//FIXME: STRING is not the correct type; May require special handling; clearNotes?
	xSheetOperation->clearContents(sheet::CellFlags::STRING);
}

void
ScVbaRange::ClearContents() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY_THROW);
	xSheetOperation->clearContents(sheet::CellFlags::VALUE | sheet::CellFlags::STRING );
}

void
ScVbaRange::ClearFormats() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetOperation > xSheetOperation(mxRange, uno::UNO_QUERY_THROW);
	//FIXME: need to check if we need to combine sheet::CellFlags::FORMATTED
	xSheetOperation->clearContents(sheet::CellFlags::HARDATTR | sheet::CellFlags::FORMATTED | sheet::CellFlags::EDITATTR);
}

::rtl::OUString
ScVbaRange::getFormula() throw (::com::sun::star::uno::RuntimeException)
{
	uno::Reference< table::XCell > xCell( mxRange->getCellByPosition( 0, 0 ), uno::UNO_QUERY_THROW );
	return xCell->getFormula();
}

void
ScVbaRange::setFormula(const ::rtl::OUString &rFormula ) throw (uno::RuntimeException)
{
	setValue( uno::makeAny( rFormula ) );
}

double 
ScVbaRange::getCount() throw (uno::RuntimeException)
{
	double rowCount, colCount;
	uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, uno::UNO_QUERY_THROW );
	rowCount = xColumnRowRange->getRows()->getCount();
	colCount = xColumnRowRange->getColumns()->getCount();
	if( IsRows() )
		return rowCount;	
	if( IsColumns() )
		return colCount;
	return rowCount * colCount;
}

sal_Int32 
ScVbaRange::getRow() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellAddressable > xCellAddressable(mxRange->getCellByPosition(0, 0), uno::UNO_QUERY_THROW );
	return xCellAddressable->getCellAddress().Row + 1; // Zero value indexing 
}	
		
sal_Int32 
ScVbaRange::getColumn() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XCellAddressable > xCellAddressable(mxRange->getCellByPosition(0, 0), uno::UNO_QUERY_THROW );
	return xCellAddressable->getCellAddress().Column + 1; // Zero value indexing
}

sal_Bool
ScVbaRange::HasFormula() throw (uno::RuntimeException)
{
	uno::Reference< table::XCell > xCell( mxRange->getCellByPosition(0, 0), uno::UNO_QUERY_THROW );
	return( xCell->getType() == table::CellContentType_FORMULA );
}
void
ScVbaRange::fillSeries( sheet::FillDirection nFillDirection, sheet::FillMode nFillMode, sheet::FillDateMode nFillDateMode, double fStep, double fEndValue ) throw( uno::RuntimeException )
{
	uno::Reference< sheet::XCellSeries > xCellSeries(mxRange, uno::UNO_QUERY_THROW );
}

void 
ScVbaRange::FillLeft() throw (uno::RuntimeException)
{
	fillSeries(sheet::FillDirection_TO_LEFT, 
		sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

void 
ScVbaRange::FillRight() throw (uno::RuntimeException)
{
	fillSeries(sheet::FillDirection_TO_RIGHT, 
		sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

void 
ScVbaRange::FillUp() throw (uno::RuntimeException)
{
	fillSeries(sheet::FillDirection_TO_TOP, 
		sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

void 
ScVbaRange::FillDown() throw (uno::RuntimeException)
{
	fillSeries(sheet::FillDirection_TO_BOTTOM, 
		sheet::FillMode_LINEAR, sheet::FillDateMode_FILL_DATE_DAY, 0, 0x7FFFFFFF);
}

::rtl::OUString
ScVbaRange::getText() throw (uno::RuntimeException)
{
	uno::Reference< text::XTextRange > xTextRange(mxRange, uno::UNO_QUERY_THROW );
	return xTextRange->getString();
}

void 
ScVbaRange::setText( const ::rtl::OUString &rString ) throw (uno::RuntimeException)
{
	uno::Reference< text::XTextRange > xTextRange(mxRange, uno::UNO_QUERY_THROW );
	xTextRange->setString( rString );
}

uno::Reference< vba::XRange >
ScVbaRange::Offset( const ::uno::Any &nRowOff, const uno::Any &nColOff ) throw (uno::RuntimeException)
{
	sal_Int32 nRowOffset = 0, nColOffset = 0;
	sal_Bool bIsRowOffset = ( nRowOff >>= nRowOffset );
	sal_Bool bIsColumnOffset = ( nColOff >>= nColOffset );

	RangeHelper helper( mxRange );

	return RangeHelper::createRangeFromRange( m_xContext, helper.getCellRangeFromSheet(), helper.getCellRangeAddressable(), nColOffset, nRowOffset, nColOffset, nRowOffset );	
}

uno::Reference< vba::XRange >
ScVbaRange::CurrentRegion() throw (uno::RuntimeException)
{
	RangeHelper helper( mxRange );
	uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = 
		helper.getSheetCellCursor();
	xSheetCellCursor->collapseToCurrentRegion();
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xSheetCellCursor, uno::UNO_QUERY_THROW);
	return RangeHelper::createRangeFromRange( m_xContext, helper.getCellRangeFromSheet(), xCellRangeAddressable );	
}

uno::Reference< vba::XRange >
ScVbaRange::CurrentArray() throw (uno::RuntimeException)
{
	RangeHelper helper( mxRange );
	uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor = 
		helper.getSheetCellCursor();
	xSheetCellCursor->collapseToCurrentArray();
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xSheetCellCursor, uno::UNO_QUERY_THROW);
	return RangeHelper::createRangeFromRange( m_xContext, helper.getCellRangeFromSheet(), xCellRangeAddressable );	
}

::rtl::OUString
ScVbaRange::getFormulaArray() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XArrayFormulaRange > xArrayFormulaRange(mxRange, ::uno::UNO_QUERY_THROW );
	return xArrayFormulaRange->getArrayFormula();
}

void 
ScVbaRange::setFormulaArray(const ::rtl::OUString &rFormula) throw (uno::RuntimeException)
{
	uno::Reference< sheet::XArrayFormulaRange > xArrayFormulaRange(mxRange, ::uno::UNO_QUERY_THROW );
	xArrayFormulaRange->setArrayFormula( rFormula );
}

::rtl::OUString
ScVbaRange::Characters(const uno::Any& Start, const uno::Any& Length) throw (uno::RuntimeException)
{
	long nIndex, nCount;
	::rtl::OUString rString;
	uno::Reference< text::XTextRange > xTextRange(mxRange, ::uno::UNO_QUERY_THROW );
	rString = xTextRange->getString();
	if( !( Start >>= nIndex ) && !( Length >>= nCount ) )
		return rString;
	if(!( Start >>= nIndex ) )
		nIndex = 1;
	if(!( Length >>= nCount ) )
		nIndex = rString.getLength();
	return rString.copy( --nIndex, nCount ); // Zero value indexing
}

::rtl::OUString
ScVbaRange::Address() throw (uno::RuntimeException)
{
	::rtl::OUString aStart, aEnd;
	uno::Sequence< uno::Any > aAddrArray1, aAddrArray2;	

	uno::Reference< lang::XMultiComponentFactory > xSMgr( m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );

	uno::Reference< sheet::XFunctionAccess > xFunctionAccess( 
		xSMgr->createInstanceWithContext(::rtl::OUString::createFromAscii(
			"com.sun.star.sheet.FunctionAccess"), m_xContext), 
			::uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable( mxRange, ::uno::UNO_QUERY_THROW );
	if( aAddrArray1.getLength() == 0 )
	{
		aAddrArray1.realloc(2);
		uno::Any* aArray = aAddrArray1.getArray();
		aArray[0] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().StartRow + 1 );
		aArray[1] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().StartColumn + 1 );
	}
	uno::Any aString1 = xFunctionAccess->callFunction(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADDRESS")), aAddrArray1);
	aString1 >>= aStart;

	if( xCellRangeAddressable->getRangeAddress().StartColumn == xCellRangeAddressable->getRangeAddress().EndColumn &&
	xCellRangeAddressable->getRangeAddress().StartRow == xCellRangeAddressable->getRangeAddress().EndRow )
	return aStart;

	String aString(aStart);
	aStart = rtl::OUString(aString.Append((sal_Unicode)':'));
	if( aAddrArray2.getLength() == 0 )
	{
		aAddrArray2.realloc(2);
		uno::Any* aArray = aAddrArray2.getArray();
		aArray[0] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().EndRow + 1 );
		aArray[1] = ( uno::Any )( xCellRangeAddressable->getRangeAddress().EndColumn + 1 );
	}
	uno::Any aString2 = xFunctionAccess->callFunction(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADDRESS")), aAddrArray2);
	aString2 >>= aEnd;
	return aStart.concat( aEnd );
}

uno::Reference < vba::XFont >
ScVbaRange::Font() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps(mxRange, ::uno::UNO_QUERY );
	return uno::Reference< vba::XFont >( new ScVbaFont( xProps ) );
}
                                                                                                                             
uno::Reference< vba::XRange >
ScVbaRange::Cells( const uno::Any &nRowIndex, const uno::Any &nColumnIndex ) throw(uno::RuntimeException)
{
	long nRow = 0, nColumn = 0;
	sal_Bool bIsIndex = nRowIndex >>= nRow, bIsColumnIndex = nColumnIndex >>= nColumn;
                                                                                                                       
	uno::Reference< sheet::XSheetCellRange > xSheetCellRange(mxRange, ::uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
	uno::Reference< table::XCellRange > xRange(xSheet, uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XCellRangeAddressable > xAddressable( xRange, uno::UNO_QUERY_THROW );
	uno::Reference< table::XCellRange > xRangeReference = xRange->getCellRangeByPosition(
																	getColumn()-1, getRow()-1,
																	xAddressable->getRangeAddress().EndColumn,
																	xAddressable->getRangeAddress().EndRow );
	if( !bIsIndex && !bIsColumnIndex ) // .Cells
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange ) );
	if( !nRow )
			throw uno::RuntimeException(
										::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": Invalid RowIndex ") ),
										uno::Reference< XInterface >() );
	if( bIsIndex && !bIsColumnIndex ) // .Cells(n)
	{
		int nIndex = --nRow;
		uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, ::uno::UNO_QUERY_THROW);
		int nColCount = xColumnRowRange->getColumns()->getCount();
		nRow = nIndex / nColCount;
		nColumn = nIndex % nColCount;
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRangeReference->getCellRangeByPosition( 
																	nColumn, nRow, nColumn, nRow ) ) );
	}
	if( !nColumn )
		throw uno::RuntimeException(
					::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": Invalid ColumnIndex" ) ),
					uno::Reference< XInterface >() );
	--nRow, --nColumn;
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xRangeReference->getCellRangeByPosition( nColumn, nRow,                                        nColumn, nRow ) ) );
}

void
ScVbaRange::Select() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
	uno::Reference< view::XSelectionSupplier > xSelection( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
	xSelection->select( ( uno::Any )mxRange );
}

uno::Reference< vba::XRange >
ScVbaRange::Rows(const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	sal_Int32 nValue;
	if( !aIndex.hasValue() )
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange, true ) );
	if( aIndex >>= nValue )
	{
		uno::Reference< sheet::XCellRangeAddressable > xAddressable( mxRange, uno::UNO_QUERY );
		--nValue;
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange->getCellRangeByPosition(
						xAddressable->getRangeAddress().StartColumn, nValue,
						xAddressable->getRangeAddress().EndColumn, nValue ), true ) ); 	
	}
	// Questionable return, I'm just copying the invalid Any::value path
	// above. Would seem to me that this is an internal error and 
	// warrants an exception thrown
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange, true ) );
}	

uno::Reference< vba::XRange >
ScVbaRange::Columns( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	sal_Int32 nValue;
	if( !aIndex.hasValue() )
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange, false, true ) );
	if( aIndex >>= nValue )
	{
		uno::Reference< sheet::XCellRangeAddressable > xAddressable( mxRange, uno::UNO_QUERY_THROW );
		--nValue;
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange->getCellRangeByPosition(
							nValue, xAddressable->getRangeAddress().StartRow,
							nValue, xAddressable->getRangeAddress().EndRow ), false, true ) ); 
	}
	// Questionable return, I'm just copying the invalid Any::value path
	// above. Would seem to me that this is an internal error and 
	// warrants an exception thrown
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, mxRange, false, true ) );
}

void
ScVbaRange::setMergeCells( sal_Bool bIsMerged ) throw (uno::RuntimeException)
{
	uno::Reference< util::XMergeable > xMerge( mxRange, ::uno::UNO_QUERY_THROW );
	//FIXME need to check whether all the cell contents are retained or lost by popping up a dialog 
	xMerge->merge( bIsMerged );
}
                                                                                                                             
sal_Bool
ScVbaRange::getMergeCells() throw (uno::RuntimeException)
{
	uno::Reference< util::XMergeable > xMerge( mxRange, ::uno::UNO_QUERY_THROW );
	return xMerge->getIsMerged();
}
                                                                                                                             
void
ScVbaRange::Copy(const ::uno::Any& Destination) throw (uno::RuntimeException)
{
	if ( Destination.hasValue() )
	{
		uno::Reference< vba::XRange > xRange( Destination, uno::UNO_QUERY_THROW );
		uno::Any aRange = xRange->getCellRange();
		uno::Reference< table::XCellRange > xCellRange;
		aRange >>= xCellRange;
		uno::Reference< sheet::XSheetCellRange > xSheetCellRange(xCellRange, ::uno::UNO_QUERY_THROW);
		uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
		uno::Reference< table::XCellRange > xDest( xSheet, uno::UNO_QUERY_THROW );
		uno::Reference< sheet::XCellRangeMovement > xMover( xSheet, uno::UNO_QUERY_THROW);
		uno::Reference< sheet::XCellAddressable > xDestination( xDest->getCellByPosition(
												xRange->getColumn()-1,xRange->getRow()-1), uno::UNO_QUERY_THROW );
		uno::Reference< sheet::XCellRangeAddressable > xSource( mxRange, uno::UNO_QUERY);
		xMover->copyRange( xDestination->getCellAddress(), xSource->getRangeAddress() );
	}
	else
	{
		Select();
		implnCopy();
	}
}

void
ScVbaRange::Cut(const ::uno::Any& Destination) throw (uno::RuntimeException)
{
	if (Destination.hasValue())
	{
		uno::Reference< vba::XRange > xRange( Destination, uno::UNO_QUERY_THROW );
		uno::Reference< table::XCellRange > xCellRange( xRange->getCellRange(), uno::UNO_QUERY_THROW );
		uno::Reference< sheet::XSheetCellRange > xSheetCellRange(xCellRange, ::uno::UNO_QUERY_THROW );
		uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
		uno::Reference< table::XCellRange > xDest( xSheet, uno::UNO_QUERY_THROW );
		uno::Reference< sheet::XCellRangeMovement > xMover( xSheet, uno::UNO_QUERY_THROW);
		uno::Reference< sheet::XCellAddressable > xDestination( xDest->getCellByPosition(
												xRange->getColumn()-1,xRange->getRow()-1), uno::UNO_QUERY);
		uno::Reference< sheet::XCellRangeAddressable > xSource( mxRange, uno::UNO_QUERY);
		xMover->moveRange( xDestination->getCellAddress(), xSource->getRangeAddress() );
	}
	{
		Select();
		implnCut();
	}
}
                                                                                                                             
void
ScVbaRange::setNumberFormat( const ::rtl::OUString &rFormat ) throw (uno::RuntimeException)
{
	uno::Reference< util::XNumberFormatsSupplier > xSupplier(getCurrentDocument(), uno::UNO_QUERY_THROW);
	uno::Reference< util::XNumberFormats > xFormats( xSupplier->getNumberFormats(), uno::UNO_QUERY_THROW );
	uno::Reference< beans::XPropertySet > xRangeProps( mxRange, uno::UNO_QUERY_THROW );
	//FIXME behaviour with different locales
	lang::Locale pLocale = ::com::sun::star::lang::Locale();
	long nIndexKey = xFormats->queryKey( rFormat, pLocale, false );
	if( nIndexKey != -1 )
		xRangeProps->setPropertyValue(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("NumberFormat")), (uno::Any) nIndexKey );
}
                                                                                                                             
::rtl::OUString
ScVbaRange::getNumberFormat() throw (uno::RuntimeException)
{
	NumFormatHelper numFormat( mxRange );
	return numFormat.getNumberFormatString();
}

uno::Reference< vba::XRange >
ScVbaRange::Resize( const uno::Any &RowSize, const uno::Any &ColumnSize ) throw (uno::RuntimeException)
{
	long nRowSize, nColumnSize;
	sal_Bool bIsRowChanged = ( RowSize >>= nRowSize ), bIsColumnChanged = ( ColumnSize >>= nColumnSize );
	uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, ::uno::UNO_QUERY_THROW);
	uno::Reference< sheet::XSheetCellRange > xSheetRange(mxRange, ::uno::UNO_QUERY_THROW);
	uno::Reference< sheet::XSheetCellCursor > xCursor( xSheetRange->getSpreadsheet()->createCursorByRange(xSheetRange), ::uno::UNO_QUERY_THROW );
	if( !nRowSize || !nColumnSize )
		throw lang::IndexOutOfBoundsException();
	if( !bIsRowChanged )
		nRowSize = xColumnRowRange->getRows()->getCount();
	if( !bIsColumnChanged )
		nColumnSize = xColumnRowRange->getColumns()->getCount();
	xCursor->collapseToSize( nColumnSize, nRowSize );
	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xCursor, ::uno::UNO_QUERY_THROW );
	uno::Reference< table::XCellRange > xRange( xSheetRange->getSpreadsheet(), ::uno::UNO_QUERY_THROW );
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext,xRange->getCellRangeByPosition(
										xCellRangeAddressable->getRangeAddress().StartColumn,
										xCellRangeAddressable->getRangeAddress().StartRow,
										xCellRangeAddressable->getRangeAddress().EndColumn,
										xCellRangeAddressable->getRangeAddress().EndRow ) ) );
}
                                                                                                                             
void
ScVbaRange::setWrapText( sal_Bool bIsWrapped ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps(mxRange, ::uno::UNO_QUERY_THROW );
	xProps->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsTextWrapped" ) ), ( uno::Any )bIsWrapped );
}
                                                                                                                             
sal_Bool
ScVbaRange::getWrapText() throw (uno::RuntimeException)
{
	sal_Bool bWrapped = false;
	uno::Reference< beans::XPropertySet > xProps(mxRange, ::uno::UNO_QUERY_THROW );
	uno::Any aValue = xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsTextWrapped" ) ) );
	aValue >>= bWrapped;
	return bWrapped;
}

uno::Reference< vba::XInterior > ScVbaRange::Interior( ) throw (uno::RuntimeException)
{
        return uno::Reference<vba::XInterior> (new ScVbaInterior ( m_xContext, mxRange));
}                                                                                                                             
uno::Reference< vba::XRange >
ScVbaRange::Range( const uno::Any &Cell1, const uno::Any &Cell2 ) throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetCellRange > xSheetCellRange( mxRange, uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XSpreadsheet > xSheet = xSheetCellRange->getSpreadsheet();
	uno::Reference< table::XCellRange > xRanges( xSheet, uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XCellRangeAddressable > xAddressable( xRanges, uno::UNO_QUERY_THROW );
	uno::Reference< table::XCellRange > xReferrer = xRanges->getCellRangeByPosition( 
																				getColumn()-1, getRow()-1,
																				xAddressable->getRangeAddress().EndColumn,
																				xAddressable->getRangeAddress().EndRow );
	rtl::OUString rString;
	uno::Reference< vba::XRange > aRange1, aRange2;
	sal_Bool bIsCell1 = Cell1 >>= aRange1, bIsCell2 = Cell2 >>= aRange2;
	
	if( !Cell1.hasValue() )
		throw uno::RuntimeException(
			rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( " Invalid Argument " ) ),
			uno::Reference< XInterface >() );
	if( ( Cell1 >>= rString ) && !Cell2.hasValue() )
	{
		uno::Reference< table::XCellRange > xRange = xRanges->getCellRangeByName( rString );
		uno::Reference< sheet::XCellRangeAddressable > xRefAddress( xRange, uno::UNO_QUERY_THROW );
		return uno::Reference< vba::XRange > ( new ScVbaRange( m_xContext, xReferrer->getCellRangeByPosition(
							xRefAddress->getRangeAddress().StartColumn,
							xRefAddress->getRangeAddress().StartRow,
							xRefAddress->getRangeAddress().EndColumn,
							xRefAddress->getRangeAddress().EndRow ) ) );
	}
	if( bIsCell1 && bIsCell2 )
		return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xReferrer->getCellRangeByPosition(
							aRange1->getColumn() - 1, aRange1->getRow() - 1,
							aRange2->getColumn() - 1, aRange2->getRow() - 1 ) ) );
	if( bIsCell1 && !Cell2.hasValue() )
		return uno::Reference< vba::XRange > ( new ScVbaRange( m_xContext, xReferrer->getCellRangeByPosition(
							aRange1->getColumn() - 1, aRange1->getRow() - 1,
							aRange1->getColumn() - 1, aRange1->getRow() - 1 ) ) );
	return NULL;
}

::com::sun::star::uno::Any SAL_CALL
ScVbaRange::getCellRange(  ) throw (::com::sun::star::uno::RuntimeException)
{
	uno::Any aAny;
	aAny <<= mxRange;
	return aAny;
}

static USHORT 
getPasteFlags (sal_Int16 Paste)
{
	USHORT nFlags = IDF_NONE;	
	switch (Paste) {
        case vba::xlPasteType::xlPasteComments: 
		nFlags = IDF_NOTE;break;
        case vba::xlPasteType::xlPasteFormats: 
		nFlags = IDF_ATTRIB;break;
        case vba::xlPasteType::xlPasteFormulas: 
		nFlags = IDF_FORMULA;break;
        case vba::xlPasteType::xlPasteFormulasAndNumberFormats : 
		nFlags = IDF_FORMULA | IDF_VALUE ;break;
        case vba::Excel::XlFindLookIn::xlValues: 
        case vba::xlPasteType::xlPasteValues: 
		nFlags = IDF_VALUE | IDF_SPECIAL_BOOLEAN ;break;
        case vba::xlPasteType::xlPasteValuesAndNumberFormats:
		nFlags = IDF_VALUE | IDF_ATTRIB; break;
        case vba::xlPasteType::xlPasteColumnWidths:
        case vba::xlPasteType::xlPasteValidation: 
		nFlags = IDF_NONE;break;
	case vba::xlPasteType::xlPasteAll: 
        case vba::xlPasteType::xlPasteAllExceptBorders: 
	default:
		nFlags = IDF_ALL;break;
	};
return nFlags;
}

static USHORT 
getPasteFormulaBits( sal_Int16 Operation)
{
	USHORT nFormulaBits = PASTE_NOFUNC ;
	switch (Operation)
	{
	case vba::xlPasteSpecialOperation::xlPasteSpecialOperationAdd: 
		nFormulaBits = PASTE_ADD;break;
	case vba::xlPasteSpecialOperation::xlPasteSpecialOperationSubtract: 
		nFormulaBits = PASTE_SUB;break;
	case vba::xlPasteSpecialOperation::xlPasteSpecialOperationMultiply: 
		nFormulaBits = PASTE_MUL;break;
	case vba::xlPasteSpecialOperation::xlPasteSpecialOperationDivide:
		nFormulaBits = PASTE_DIV;break;

	case vba::xlPasteSpecialOperation::xlPasteSpecialOperationNone: 
	case vba::Excel::Constants::xlNone:
	default:
		nFormulaBits = PASTE_NOFUNC; break;
	};
	
return nFormulaBits;
}
void SAL_CALL 
ScVbaRange::PasteSpecial(sal_Int16 Paste, sal_Int16 Operation, ::sal_Bool SkipBlanks, ::sal_Bool Transpose ) throw (::com::sun::star::uno::RuntimeException) 
{
	USHORT nFlags = getPasteFlags(Paste);
	USHORT nFormulaBits = getPasteFormulaBits(Operation);
	implnPasteSpecial(nFlags,nFormulaBits,SkipBlanks,Transpose);

}

uno::Reference< vba::XRange > 
ScVbaRange::getEntireColumnOrRow( bool bEntireRow ) throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSheetCellRange > xSheetCellRange(mxRange, uno::UNO_QUERY_THROW);
	uno::Reference< sheet::XSpreadsheet > xSheet( xSheetCellRange->getSpreadsheet(), uno::UNO_QUERY_THROW );
	uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor( xSheet->createCursorByRange( xSheetCellRange ), uno::UNO_QUERY_THROW );
	if ( bEntireRow ) 
		xSheetCellCursor->expandToEntireRows();
	else
		xSheetCellCursor->expandToEntireColumns();

	uno::Reference< sheet::XCellRangeAddressable > xCellRangeAddressable(xSheetCellCursor, uno::UNO_QUERY_THROW);
	uno::Reference< table::XCellRange > xCellRange( xSheet, uno::UNO_QUERY_THROW);
	return uno::Reference< vba::XRange >( new ScVbaRange( m_xContext, xCellRange->getCellRangeByPosition( xCellRangeAddressable->getRangeAddress().StartColumn, xCellRangeAddressable->getRangeAddress().StartRow, xCellRangeAddressable->getRangeAddress().EndColumn, xCellRangeAddressable->getRangeAddress().EndRow ) , bEntireRow, !bEntireRow ) );
}

uno::Reference< vba::XRange > SAL_CALL 
ScVbaRange::getEntireRow() throw (uno::RuntimeException)
{
	return getEntireColumnOrRow();
}

uno::Reference< vba::XRange > SAL_CALL 
ScVbaRange::getEntireColumn() throw (uno::RuntimeException)
{
	return getEntireColumnOrRow( false );
}

uno::Reference< vba::XComment > SAL_CALL
ScVbaRange::getComment() throw (css::uno::RuntimeException)
{
	// intentional behavior to return a null object if no
	// comment defined
	uno::Reference< vba::XComment > xComment( new ScVbaComment( m_xContext, mxRange ) );
	if ( !xComment->Text( uno::Any(), uno::Any(), uno::Any() ).getLength() )
		return NULL;
	return xComment;
	
}

uno::Reference< beans::XPropertySet >
getRowOrColumnProps( const uno::Reference< table::XCellRange >& xCellRange, bool bRows ) throw ( uno::RuntimeException )
{
	uno::Reference< table::XColumnRowRange > xColRow( xCellRange, uno::UNO_QUERY_THROW );
	uno::Reference< beans::XPropertySet > xProps;
	if ( bRows )
		xProps.set( xColRow->getRows(), uno::UNO_QUERY_THROW );
	else
		xProps.set( xColRow->getColumns(), uno::UNO_QUERY_THROW );
	return xProps;	
}

uno::Any SAL_CALL 
ScVbaRange::getHidden() throw (uno::RuntimeException)
{
	bool bIsVisible = false;
	try
	{
		uno::Reference< beans::XPropertySet > xProps = getRowOrColumnProps( mxRange, mbIsRows );
		if ( !( xProps->getPropertyValue( ISVISIBLE ) >>= bIsVisible ) )
			throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Failed to get IsVisible property")), uno::Reference< uno::XInterface >() );
	}
	catch( uno::Exception& e )
	{
		throw uno::RuntimeException( e.Message, uno::Reference< uno::XInterface >() );
	}
	return uno::makeAny( !bIsVisible ); 
}

void SAL_CALL 
ScVbaRange::setHidden( const uno::Any& _hidden ) throw (uno::RuntimeException)
{
	sal_Bool bHidden;
	if ( !(_hidden >>= bHidden) )
		throw uno::RuntimeException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Failed to extract param for Hidden property" ) ), uno::Reference< uno::XInterface >() ); 

	try
	{
		uno::Reference< beans::XPropertySet > xProps = getRowOrColumnProps( mxRange, mbIsRows );
		xProps->setPropertyValue( ISVISIBLE, uno::makeAny( !bHidden ) );
	}
	catch( uno::Exception& e )
	{
		throw uno::RuntimeException( e.Message, uno::Reference< uno::XInterface >() );
	}	
}

::sal_Bool SAL_CALL 
ScVbaRange::Replace( const ::rtl::OUString& What, const ::rtl::OUString& Replacement, const uno::Any& LookAt, const uno::Any& SearchOrder, const uno::Any& MatchCase, const uno::Any& MatchByte ) throw (uno::RuntimeException)
{
	// sanity check required params
	if ( !What.getLength() || !Replacement.getLength() )
		throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Range::Replace, missing params" )) , uno::Reference< uno::XInterface >() );

	// #TODO
	// also this method should take/set the defaults from the find dialog
	// but.... really thats probably not important for us

	// Set up sensible defaults
	sal_Int16 nLook =  vba::Excel::XlLookAt::xlPart;
	sal_Int16 nSearchOrder = vba::Excel::XlSearchOrder::xlByRows;
	sal_Bool bMatchCase = sal_False;

	uno::Reference< util::XReplaceable > xReplace( mxRange, uno::UNO_QUERY );
	if ( xReplace.is() )
	{
		uno::Reference< util::XReplaceDescriptor > xDescriptor =
			xReplace->createReplaceDescriptor();
		
		xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SearchString" ) ), uno::makeAny(What) ); 		
		xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ReplacesString" ) ), uno::makeAny(Replacement) ); 		
		if ( LookAt.hasValue() )
		{
			// sets SearchWords ( true is Cell match )
			nLook =  ::comphelper::getINT16( LookAt );
			if ( nLook == vba::Excel::XlLookAt::xlPart )
				xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SearchWords" ) ), uno::makeAny( sal_False ) ); 	
			else if ( nLook == vba::Excel::XlLookAt::xlWhole )
				xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SearchWords" ) ), uno::makeAny( sal_True ) ); 	
			else
				throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Range::Replace, illegal value for LookAt" )) , uno::Reference< uno::XInterface >() );

		}
		// sets SearchByRow ( true for Rows )
		if ( SearchOrder.hasValue() )
		{
			nSearchOrder =  ::comphelper::getINT16( SearchOrder );
			if ( nSearchOrder == vba::Excel::XlSearchOrder::xlByColumns )
				xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SearchByRows" ) ), uno::makeAny( sal_False ) ); 	
			else if ( nSearchOrder == vba::Excel::XlSearchOrder::xlByRows )
				xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SearchByRows" ) ), uno::makeAny( sal_True ) ); 	
			else
				throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Range::Replace, illegal value for SearchOrder" )) , uno::Reference< uno::XInterface >() );
			
		}			
		if ( MatchCase.hasValue() )
		{
			// SearchCaseSensitive
			MatchCase >>= bMatchCase;	
			xDescriptor->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SearchCaseSensitive" ) ), uno::makeAny( bMatchCase ) ); 	
		}			
		// ignore MatchByte for the moment, its not supported in
		// OOo.org afaik

		uno::Reference< util::XSearchDescriptor > xSearch( xDescriptor, uno::UNO_QUERY );
		xReplace->replaceAll( xSearch );
	}
	return sal_True; // always
}

void SAL_CALL
ScVbaRange::Sort( const uno::Any& Key1, ::sal_Int16 Order1, const uno::Any& Key2, const uno::Any& Type, ::sal_Int16 Order2, const uno::Any& Key3, ::sal_Int16 Order3, ::sal_Int16 header, const uno::Any& OrderCustom, const uno::Any& MatchCase, ::sal_Int16 Orientation, ::sal_Int16 SortMethod ) throw (uno::RuntimeException)
{
}

//XElementAccess
sal_Bool SAL_CALL 
ScVbaRange::hasElements() throw (uno::RuntimeException)
{
	uno::Reference< table::XColumnRowRange > xColumnRowRange(mxRange, uno::UNO_QUERY );
	if ( xColumnRowRange.is() )
		if ( xColumnRowRange->getRows()->getCount() ||
			xColumnRowRange->getColumns()->getCount() )
			return sal_True;
	return sal_False;
}

// XEnumerationAccess
uno::Reference< container::XEnumeration > SAL_CALL 
ScVbaRange::createEnumeration() throw (uno::RuntimeException)
{
	return new CellsEnumeration( m_xContext, mxRange );
}

// XInterface

IMPLEMENT_FORWARD_XINTERFACE2( ScVbaRange, ScVbaRange_BASE, OPropertyContainer )

// XTypeProvider

IMPLEMENT_FORWARD_XTYPEPROVIDER2( ScVbaRange, ScVbaRange_BASE, OPropertyContainer )

// OPropertySetHelper

::cppu::IPropertyArrayHelper& 
ScVbaRange::getInfoHelper(  )
{
    static ::cppu::IPropertyArrayHelper* sProps = 0;
    if ( !sProps )
        sProps = createArrayHelper();
    return *sProps;
}


::cppu::IPropertyArrayHelper* 
ScVbaRange::createArrayHelper(  ) const
{
    uno::Sequence< beans::Property > aProps;
    describeProperties( aProps );
    return new ::cppu::OPropertyArrayHelper( aProps );
}

// XPropertySet
uno::Reference< beans::XPropertySetInfo > 
ScVbaRange::getPropertySetInfo(  ) throw (uno::RuntimeException)
{
    uno::Reference< beans::XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}
