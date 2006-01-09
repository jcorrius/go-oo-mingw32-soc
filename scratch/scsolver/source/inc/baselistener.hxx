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

#ifndef _BASELISTENER_HXX_
#define _BASELISTENER_HXX_

#include <unoglobal.hxx>

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/awt/XActionListener.hpp>
#include <com/sun/star/awt/XItemListener.hpp>
#include <com/sun/star/awt/XFocusListener.hpp>
#include <com/sun/star/awt/XMouseListener.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace scsolver {

class BaseDialog;

//--------------------------------------------------------------------------
// Listener Class for Dialog Widgets

// All listener objects keeps a pointer to the dialog they belong to.  Such
// pointer can be retrieved by calling getDialog().
class Listener
{
public:
	Listener( BaseDialog* pDlg ) { m_pDlg = pDlg; }
	virtual ~Listener() throw() = 0;
	
protected:
	BaseDialog* getDialog() const { return m_pDlg; }
	
private:
	BaseDialog* m_pDlg;
};


class ActionListener : public ::cppu::WeakImplHelper1< awt::XActionListener >, public Listener
{
public:
	ActionListener( BaseDialog* pDlg ) : Listener( pDlg ) {}
	virtual ~ActionListener() throw() = 0;
	
	virtual void disposing( const lang::EventObject& ) throw ( RuntimeException ) {}
	virtual void actionPerformed( const awt::ActionEvent& ) throw ( RuntimeException ) {}
};


class ItemListener : public ::cppu::WeakImplHelper1< awt::XItemListener >, public Listener
{
public:
	ItemListener( BaseDialog* pDlg ) : Listener( pDlg ) {}
	virtual ~ItemListener() throw() = 0;

	virtual void disposing( const lang::EventObject& oEvt ) throw ( RuntimeException ) {}
	virtual void itemStateChanged( const awt::ItemEvent& oEvt ) throw ( RuntimeException ) {}
};


class FocusListener : public ::cppu::WeakImplHelper1< awt::XFocusListener >, public Listener
{
public:
	FocusListener( BaseDialog* pDlg ) : Listener( pDlg ) {}
	virtual ~FocusListener() throw() = 0;
	
	virtual void focusGained( const awt::FocusEvent& ) throw( RuntimeException ) {}
	virtual void focusLost( const awt::FocusEvent& ) throw( RuntimeException ) {}
	virtual void disposing( const lang::EventObject& oEvt ) throw ( RuntimeException ) {}
};	

class MouseListener : public ::cppu::WeakImplHelper1< awt::XMouseListener >, public Listener
{
public:
	MouseListener( BaseDialog* pDlg ) : Listener( pDlg ) {}
	virtual ~MouseListener() throw() = 0;
	
	virtual void mousePressed( const awt::MouseEvent& ) throw( RuntimeException ) {}
	virtual void mouseReleased( const awt::MouseEvent& o) throw( RuntimeException ) {}
	virtual void mouseEntered( const awt::MouseEvent& o) throw( RuntimeException ) {}
	virtual void mouseExited( const awt::MouseEvent& o) throw( RuntimeException ) {}
	
	virtual void disposing( const lang::EventObject& oEvt ) throw ( RuntimeException ) {}
};


}

#endif
