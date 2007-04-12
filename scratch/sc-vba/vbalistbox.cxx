#include <com/sun/star/form/validation/XValidatableFormComponent.hpp>

#include "vbalistbox.hxx"
#include "vbapropvalue.hxx"
#include <vector>
#include <comphelper/anytostring.hxx>

using namespace com::sun::star;
using namespace org::openoffice;

const static rtl::OUString CONTROLSOURCEPROP( RTL_CONSTASCII_USTRINGPARAM("DataFieldProperty") );
const static rtl::OUString ITEMS( RTL_CONSTASCII_USTRINGPARAM("StringItemList") );

rtl::OUString lcl_getAnyAsString( const uno::Any& pvargItem ) throw ( uno::RuntimeException )
{
	uno::Type aType = pvargItem.getValueType();
	uno::TypeClass eTypeClass = aType.getTypeClass();
	rtl::OUString sString;
	switch ( eTypeClass )
	{
		case uno::TypeClass_BOOLEAN:
		{
			sal_Bool bBool = sal_False;
			pvargItem >>= bBool;
			sString = rtl::OUString::valueOf( bBool );
			break;
		}
		case uno::TypeClass_STRING:
			pvargItem >>= sString;
			break;
		case uno::TypeClass_FLOAT:
			float aFloat;
			pvargItem >>= aFloat;
			sString = rtl::OUString::valueOf( aFloat );
			break;
		case uno::TypeClass_DOUBLE:
			double aDouble;
			pvargItem >>= aDouble;
			sString = rtl::OUString::valueOf( aDouble );
			break;
		case uno::TypeClass_SHORT:
		case uno::TypeClass_LONG:
			sal_Int32 aNum;
			pvargItem >>= aNum;
			sString = rtl::OUString::valueOf( aNum );
			break;

		case uno::TypeClass_HYPER:
			sal_Int64 aHyper;
			pvargItem >>= aHyper;
			sString = rtl::OUString::valueOf( aHyper );
			break;
		default:
       			throw uno::RuntimeException( rtl::OUString::createFromAscii( "Invalid type, can't convert" ), uno::Reference< uno::XInterface >() );
	}
	return sString;
}

ScVbaListBox::ScVbaListBox( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< css::drawing::XControlShape >& xControlShape ) : ListBoxImpl_BASE( xContext, xControlShape )
{
	// grab the default value property name
	m_xProps->getPropertyValue( CONTROLSOURCEPROP ) >>= sSourceName;
}

ScVbaListBox::ScVbaListBox( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< beans::XPropertySet >& xPropSet, const css::uno::Reference< css::drawing::XControlShape > xControlShape ) : ListBoxImpl_BASE( xContext, xPropSet, xControlShape )
{
	m_xProps->getPropertyValue( CONTROLSOURCEPROP ) >>= sSourceName;
}


// Attributes
uno::Any SAL_CALL 
ScVbaListBox::getValue() throw (uno::RuntimeException)
{
    if( getMultiSelect() )
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Attribute use invalid." ), uno::Reference< uno::XInterface >() );
    uno::Reference< form::validation::XValidatableFormComponent > xValidatableFormComponent( m_xProps, uno::UNO_QUERY_THROW );
    return xValidatableFormComponent->getCurrentValue();
}

void SAL_CALL 
ScVbaListBox::setValue( const uno::Any& _value ) throw (uno::RuntimeException)
{
    if( getMultiSelect() )
    {	
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Attribute use invalid." ), uno::Reference< uno::XInterface >() );
    }
    rtl::OUString sValue = lcl_getAnyAsString( _value );
    uno::Sequence< rtl::OUString > sList;
    m_xProps->getPropertyValue( ITEMS ) >>= sList;
    uno::Sequence< sal_Int16 > nList;
    sal_Int32 nLength = sList.getLength();
    sal_Int16 nValue = -1;
    sal_Int32 i = 0;
    for( i = 0; i < nLength; i++ )
    {
        if( sList[i].equals( sValue ) )
        {
            nValue = i; 
            break;
        }
    }
    if( nValue == -1 )
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Attribute use invalid." ), uno::Reference< uno::XInterface >() );

    nList.realloc( 1 );
    nList[0] = nValue;
            
	m_xProps->setPropertyValue( sSourceName, uno::makeAny( nList ) );
}

::rtl::OUString SAL_CALL 
ScVbaListBox::getText() throw (uno::RuntimeException)
{
	rtl::OUString result;
    uno::Reference< form::validation::XValidatableFormComponent > xValidatableFormComponent( m_xProps, uno::UNO_QUERY_THROW );
    xValidatableFormComponent->getCurrentValue() >>= result;
	return result;
}

void SAL_CALL 
ScVbaListBox::setText( const ::rtl::OUString& _text ) throw (uno::RuntimeException)
{
	setValue( uno::makeAny( _text ) ); // seems the same
}

sal_Bool SAL_CALL 
ScVbaListBox::getMultiSelect() throw (css::uno::RuntimeException)
{
    sal_Bool bMultiSelect = sal_False;
    m_xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MultiSelection" ) ) ) >>= bMultiSelect;
    return bMultiSelect;
}

void SAL_CALL 
ScVbaListBox::setMultiSelect( sal_Bool _multiselect ) throw (css::uno::RuntimeException)
{
    m_xProps->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MultiSelection" ) ), uno::makeAny( _multiselect ) );
}
/*
css::uno::Sequence< sal_Bool > SAL_CALL 
ScVbaListBox::getSelected() throw (css::uno::RuntimeException)
{
    uno::Sequence< rtl::OUString > sList;
    uno::Sequence< sal_Int16 > nList;
    m_xProps->getPropertyValue( ITEMS ) >>= sList;
    uno::Sequence< sal_Bool > bSelected( sList.getLength() );
    for( sal_Int16 i = 0; i < bSelected.getLength(); i++ )
        bSelected[i] = sal_False;
    m_xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SelectedItems" ) ) ) >>= nList;
    for( sal_Int16 i = 0; i < nList.getLength(); i++ )
    {
        bSelected[nList[i]] = sal_True;
    }
    return bSelected;
}

void SAL_CALL 
ScVbaListBox::setSelected( const css::uno::Sequence< sal_Bool >& _selected ) throw (css::uno::RuntimeException)
{
}
*/
css::uno::Any SAL_CALL 
ScVbaListBox::Selected( sal_Int32 index ) throw (css::uno::RuntimeException)
{
    uno::Sequence< rtl::OUString > sList;
    m_xProps->getPropertyValue( ITEMS ) >>= sList;
    sal_Int32 nLength = sList.getLength();
    sal_Int32 nIndex = m_nIndex;
    if( 0 > nIndex && nLength < nIndex )
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Error Number." ), uno::Reference< uno::XInterface >() );
    m_nIndex = index;
    return uno::makeAny( uno::Reference< vba::XPropValue > ( new ScVbaPropValue( this ) ) );
}

// Methods
void SAL_CALL 
ScVbaListBox::AddItem( const uno::Any& pvargItem, const uno::Any& pvargIndex ) throw (uno::RuntimeException)
{
	if ( pvargItem.hasValue()  )
	{
		uno::Sequence< rtl::OUString > sList;
		m_xProps->getPropertyValue( ITEMS ) >>= sList;

		sal_Int32 nIndex = sList.getLength();

		if ( pvargIndex.hasValue() )
			pvargIndex >>= nIndex;

		rtl::OUString sString = lcl_getAnyAsString( pvargItem );

		// if no index specified or item is to be appended to end of 
		// list just realloc the array and set the last item
		if ( nIndex  == sList.getLength() )
		{
			sal_Int32 nOldSize = sList.getLength();
			sList.realloc( nOldSize + 1 );
			sList[ nOldSize ] = sString; 
		}
		else
		{
			// just copy those elements above the one to be inserted
			std::vector< rtl::OUString > sVec;
			// reserve just the amount we need to copy
			sVec.reserve( sList.getLength() - nIndex );

			// point at first element to copy
			rtl::OUString* pString = sList.getArray() + nIndex;
			const rtl::OUString* pEndString = sList.getArray() + sList.getLength();
			// insert the new element
			sVec.push_back( sString );
			// copy elements	
			for ( ; pString != pEndString; ++pString )
				sVec.push_back( *pString );
		
			sList.realloc(  sList.getLength() + 1 );

			// point at first element to be overwritten
			pString = sList.getArray() + nIndex;
			pEndString = sList.getArray() + sList.getLength();
			std::vector< rtl::OUString >::iterator it = sVec.begin();
			for ( ; pString != pEndString; ++pString, ++it)
				*pString = *it;	
			//
		}

		m_xProps->setPropertyValue( ITEMS, uno::makeAny( sList ) );
		
	}
}

void SAL_CALL 
ScVbaListBox::Clear(  ) throw (uno::RuntimeException)
{
	// urk, setValue doesn't seem to work !!
	//setValue( uno::makeAny( sal_Int16() ) );
	m_xProps->setPropertyValue( ITEMS, uno::makeAny( uno::Sequence< rtl::OUString >() ) );
}
//PropListener
void 
ScVbaListBox::setValueEvent( const uno::Any& value )
{
    sal_Bool bValue;
    if( !(value >>= bValue) )
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Invalid type\n. need boolean." ), uno::Reference< uno::XInterface >() );
    uno::Sequence< sal_Int16 > nList;
    m_xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SelectedItems" ) ) ) >>= nList;
    sal_Int32 nLength = nList.getLength();
    sal_Int32 nIndex = m_nIndex;
    for( sal_Int32 i = 0; i < nLength; i++ )
    {
        if( nList[i] == nIndex )
        {
            if( bValue )
                return;
            else
            {
                for( ; i < nLength - 1; i++ )
                {
                    nList[i] = nList[i + 1];
                }
                nList.realloc( nLength - 1 );
                m_xProps->setPropertyValue( sSourceName, uno::makeAny( nList ) );
                return;
            }
        }
    }
    if( bValue )
    {
        if( getMultiSelect() )
        {
            nList.realloc( nLength + 1 );
            nList[nLength] = nIndex;
        }
        else
        {
            nList.realloc( 1 );
            nList[0] = nIndex;
        }
        m_xProps->setPropertyValue( sSourceName, uno::makeAny( nList ) );
    }
}

css::uno::Any 
ScVbaListBox::getValueEvent()
{
    uno::Sequence< sal_Int16 > nList;
    m_xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SelectedItems" ) ) ) >>= nList;
    sal_Int32 nLength = nList.getLength();
    sal_Int32 nIndex = m_nIndex;
    
    for( sal_Int32 i = 0; i < nLength; i++ )
    {
        if( nList[i] == nIndex )
            return uno::makeAny( sal_True );
    }

    return uno::makeAny( sal_False );
}
