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


#ifndef _LISTENER_HXX_
#define _LISTENER_HXX_

#include <global.hxx>
#include <unoglobal.hxx>
#include <type.hxx>
#include <baselistener.hxx>

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/sheet/XRangeSelectionListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>

#include <memory>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace com { namespace sun { namespace star {

	namespace sheet
	{
		class RangeSelectionEvent;
		class XRangeSelection;
	}

}}}



namespace scsolver {

class RngBtnListener;
class BaseDialog;
class Listener;
class FocusListener;
class ItemListener;
class ActionListener;

class MessageDialog;

class SolverDialog;
class SolveBtnListener;
class CloseBtnListener;
class MaxRadioBtnListener;
class ConstEditBtnListener;
class WindowFocusListener;
class WindowMouseListener;
class ConstListBoxListener;
class OKCancelBtnListener;

//--------------------------------------------------------------------------
// Class RngSelListener

class RngSelListener : public ::cppu::WeakImplHelper1< sheet::XRangeSelectionListener >
{

public:
	
	RngSelListener( BaseDialog*, RngBtnListener*, const rtl::OUString& );
	~RngSelListener() throw();
	
	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException );
	virtual void SAL_CALL done( const sheet::RangeSelectionEvent& ) throw ( RuntimeException );
	virtual void SAL_CALL aborted( const sheet::RangeSelectionEvent& ) throw ( RuntimeException );
	
private:
	BaseDialog* m_pDlg;
	RngBtnListener* m_pBtn;
	rtl::OUString m_sEditName;
	
};


//---------------------------------------------------------------------------
// Class RngBtnListener

class RngBtnListener : public ActionListener
{
public:
	
	RngBtnListener( BaseDialog* pDlg, Reference< sheet::XRangeSelection >, const rtl::OUString& );
	virtual ~RngBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) throw ( RuntimeException );

	void registerRngSelListener();	
	void setEventOwner( bool b ) { m_bEventOwner = b; }
	void setSingleCell( bool b ) { m_bSingleCell = b; }
	bool isEventOwner() { return m_bEventOwner; }
	
private:

	Reference< sheet::XRangeSelection > m_xRngSel;
	RngSelListener* m_pRngSelListener;
	rtl::OUString m_sEditName;
	bool m_bEventOwner;
	bool m_bSingleCell;
};


class SolveBtnListener : public ::scsolver::ActionListener
{

public:
	
	SolveBtnListener( SolverDialog* );
	virtual ~SolveBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) throw ( RuntimeException );

};

/**
 * Generic close button listener.  You can use this class for
 * any dialog that needs a simple "close" action.  Just pass a
 * pointer to the target dialog to the constructor when
 * instantiating an object, and register it with the "close"
 * button.
 */
class CloseBtnListener : public ActionListener
{
public:
	CloseBtnListener( BaseDialog* pDlg );
	virtual ~CloseBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt )
		throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& )
		throw ( RuntimeException );
};


class SaveBtnListener : public ::scsolver::ActionListener
{

public:
	
	SaveBtnListener( SolverDialog* pDlg );
	virtual ~SaveBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) 
		throw ( RuntimeException );
};


class LoadBtnListener : public ::scsolver::ActionListener
{

public:
	
	LoadBtnListener( SolverDialog* pDlg );
	virtual ~LoadBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) 
		throw ( RuntimeException );
};


class ResetBtnListener : public ::scsolver::ActionListener
{

public:
	
	ResetBtnListener( SolverDialog* pDlg );
	virtual ~ResetBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) 
		throw ( RuntimeException );

private:
	std::auto_ptr<MessageDialog> m_pDlg;	
};


class OptionBtnListener : public ::scsolver::ActionListener
{
public:
	OptionBtnListener( SolverDialog* pDlg );
	virtual ~OptionBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& oEvt ) 
		throw ( RuntimeException );

private:
	std::auto_ptr<MessageDialog> m_pDlg;
};


class ConstEditBtnListener : public ::scsolver::ActionListener
{
	
public:

	ConstEditBtnListener( SolverDialog*, ConstButtonType );
	virtual ~ConstEditBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& oEvt ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) throw ( RuntimeException );

private:

	ConstButtonType m_eBtnType;
	ConstButtonType getButtonType() const { return m_eBtnType; }

};


class ConstListBoxListener : public ItemListener
{
public:
	ConstListBoxListener( SolverDialog* );
	virtual ~ConstListBoxListener() throw() { Debug( "ConstListBoxListener d'tor" ); }

	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException ) {}
	
	virtual void SAL_CALL itemStateChanged( const awt::ItemEvent& ) throw ( RuntimeException );
		
private:

};


class MaxRadioBtnListener : public ItemListener
{
private:
	
public:
	
	MaxRadioBtnListener( SolverDialog* pDlg );
	virtual ~MaxRadioBtnListener() throw();
	
	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException );
	virtual void SAL_CALL itemStateChanged( const awt::ItemEvent& ) throw ( RuntimeException );
};


class WindowFocusListener : public FocusListener
{
public:

	WindowFocusListener( BaseDialog* );
	virtual ~WindowFocusListener() throw() {}
	
	virtual rtl::OUString getListenerType() const { return ascii( "FocusListener" ); }
	
	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException ) {}
	virtual void SAL_CALL focusGained( const awt::FocusEvent& ) throw( RuntimeException );
	virtual void SAL_CALL focusLost( const awt::FocusEvent& ) throw( RuntimeException );
	
};


class WindowMouseListener : public MouseListener
{
public:

	WindowMouseListener( BaseDialog* );
	virtual ~WindowMouseListener() throw();
	
	virtual void SAL_CALL mousePressed( const awt::MouseEvent& ) throw( RuntimeException );
	virtual void SAL_CALL mouseReleased( const awt::MouseEvent& ) throw( RuntimeException ) {}
	virtual void SAL_CALL mouseEntered( const awt::MouseEvent& ) throw( RuntimeException ) {}
	virtual void SAL_CALL mouseExited( const awt::MouseEvent& ) throw( RuntimeException ) {}
	
	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException ) {}
};

//--------------------------------------------------------------------------

class OKCancelBtnListener : public ::scsolver::ActionListener
{
public:
	
	OKCancelBtnListener( BaseDialog*, const rtl::OUString& );
	virtual ~OKCancelBtnListener() throw();

	// XEventListener
	virtual void SAL_CALL disposing( const lang::EventObject& ) throw ( RuntimeException );

	// XActionListener
	virtual void SAL_CALL actionPerformed( const awt::ActionEvent& ) throw ( RuntimeException );
		
private:

	rtl::OUString m_sMode;
};

}


#endif // _LISTENER_HXX
