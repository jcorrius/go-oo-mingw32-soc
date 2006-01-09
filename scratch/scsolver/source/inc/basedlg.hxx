/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

 
#ifndef _BASEDLG_HXX_
#define _BASEDLG_HXX_

#include <unoglobal.hxx>
#include <global.hxx>
#include <memory>
#include <com/sun/star/awt/PushButtonType.hpp>

using namespace com::sun::star;
using uno::Reference;
using uno::XInterface;

namespace com { namespace sun { namespace star {

	namespace awt
	{
		class XControlModel;
	}

}}}

namespace scsolver {

class SolverImpl;
class Listener;
class FocusListener;
class MouseListener;
class ActionListener;
class ItemListener;

class BaseDialogImpl;
class ObjProperty;
class ObjPropertyImpl;
class WidgetProperty;
class WidgetPropertyImpl;

typedef std::auto_ptr<ObjProperty> apObjProp;
typedef std::auto_ptr<WidgetProperty> apWidgetProp;

//--------------------------------------------------------------------------
// BaseDialog

/** Base class for all dialog classes used in scsolver. */
class BaseDialog
{
public:
	BaseDialog( SolverImpl* );
	virtual ~BaseDialog() throw() = 0;
	
	/** Must return a unique name that identifies its dialog type.  Should be
		deprecated and its use in a nested-if or switch statement is discouraged
		because it is not very elegant and non-safe. */
	virtual const rtl::OUString getDialogName() const = 0;
	virtual void setVisible( bool ) = 0;
	
	/** This method is called when a range selection is finished.  If the
		dialog does not contain a range selection widget pair, just leave
		this method empty.
		
		@return false for signaling to the parent function that a selection is
		invalid and should be discarded, or true if the selection is 
		satisfactory.
	 */
	virtual bool doneRangeSelection() const = 0;

	// Widget creation methods
	
	apWidgetProp addButton( sal_Int32, sal_Int32, sal_Int32, sal_Int32,
			const rtl::OUString&, const rtl::OUString&, sal_Int16 = awt::PushButtonType_STANDARD );
		
	apWidgetProp addButtonImage( sal_Int32, sal_Int32, sal_Int32, sal_Int32,
			const rtl::OUString &, const rtl::OUString &, sal_Int16 = awt::PushButtonType_STANDARD );
		
	apWidgetProp addEdit( sal_Int32, sal_Int32, sal_Int32, sal_Int32, const rtl::OUString & );

	apWidgetProp addFixedLine( sal_Int32, sal_Int32, sal_Int32, sal_Int32,
			const rtl::OUString &, const rtl::OUString &, sal_Int32 = 0 );
		
	apWidgetProp addFixedText( sal_Int32, sal_Int32, sal_Int32, sal_Int32,
			const rtl::OUString &, const rtl::OUString & );
		
	apWidgetProp addGroupBox( sal_Int32, sal_Int32, sal_Int32, sal_Int32, const rtl::OUString & );
		
	apWidgetProp addListBox( sal_Int32, sal_Int32, sal_Int32, sal_Int32, const rtl::OUString & );
	
	apWidgetProp addRadioButton( sal_Int32, sal_Int32, sal_Int32, sal_Int32, 
			const rtl::OUString &, const rtl::OUString & );
		
	apWidgetProp addRangeEdit( sal_Int32, sal_Int32, sal_Int32, sal_Int32,
			const rtl::OUString &, const rtl::OUString & );
			
	// Widget registration methods
	
	void registerListener( FocusListener* ) const;
	void registerListener( MouseListener* ) const;
	void registerListener( const rtl::OUString&, ActionListener* ) const;
	void registerListener( const rtl::OUString&, ItemListener* ) const;

	void unregisterListener( FocusListener* ) const;
	void unregisterListener( MouseListener* ) const;
	void unregisterListener( const rtl::OUString&, ActionListener* ) const;
	void unregisterListener( const rtl::OUString&, ItemListener* ) const;

	// Other methods

	SolverImpl* getSolverImpl() const;
	const Reference < XInterface > getWidgetByName( const rtl::OUString& ) const;
	const Reference < awt::XControlModel > getWidgetModelByName( const rtl::OUString& ) const;
	void enableWidget( const rtl::OUString&, sal_Bool = true ) const;
	void toFront() const;
	void execute() const;

protected:

	void initialize( sal_Int16, sal_Int16, const rtl::OUString& ) const;
	void setVisibleDefault( bool ) const;

private:
	std::auto_ptr<BaseDialogImpl> m_pImpl;
};

//--------------------------------------------------------------------------
// ObjProperty

class ObjProperty
{
public:
	ObjProperty( const Reference< XInterface >& );
	virtual ~ObjProperty() throw() = 0;
	
	void setPropertyValueAny( const char*, const uno::Any& );

	template<typename AnyValue>
	void setPropertyValue( const char*, const AnyValue& );
	
private:
	std::auto_ptr<ObjPropertyImpl> m_pImpl;
};

//--------------------------------------------------------------------------
// WidgetProperty

class WidgetProperty : public ObjProperty
{
public:
	WidgetProperty( const Reference< XInterface >& );
	virtual ~WidgetProperty() throw();
	
	void setPositionX( sal_Int32 ) const;
	void setPositionY( sal_Int32 ) const;
	void setWidth( sal_Int32 ) const;
	void setHeight( sal_Int32 ) const;
	void setName( const rtl::OUString& ) const;
	void setLabel( const rtl::OUString& ) const;
	
private:
	std::auto_ptr<WidgetPropertyImpl> m_pImpl;
};


// Helper function

const rtl::OUString getTextByWidget( BaseDialog*, const rtl::OUString& );
void setTextByWidget( BaseDialog*, const rtl::OUString&, const rtl::OUString& );


}

#endif // _BASEDLG_HXX_
















