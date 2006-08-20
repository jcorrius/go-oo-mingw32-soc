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

#include <baselistener.hxx>

namespace scsolver {

//---------------------------------------------------------------------------
// Class Listener

Listener::~Listener() throw()
{
}

ActionObject::ActionObject()
{
}

ActionObject::~ActionObject() throw()
{
}

ActionListener::ActionListener( BaseDialog* pDlg ) 
	: Listener(pDlg), m_pAction(NULL)
{
}


ActionListener::ActionListener( BaseDialog* pDlg, ActionObject* pAction ) 
	: Listener(pDlg), m_pAction(pAction)
{
}

ActionListener::~ActionListener() throw()
{
}

void SAL_CALL ActionListener::disposing( const lang::EventObject& e )
	throw ( RuntimeException )
{
}

void SAL_CALL ActionListener::actionPerformed( const awt::ActionEvent& e )
	throw ( RuntimeException )
{
	if( m_pAction )
		m_pAction->execute( getDialog(), e );
}

ItemListener::~ItemListener() throw()
{
}

FocusListener::~FocusListener() throw()
{
}

MouseListener::~MouseListener() throw()
{
}

}

