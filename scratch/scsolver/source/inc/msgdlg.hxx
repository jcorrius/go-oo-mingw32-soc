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

#ifndef _MSGDLG_HXX_
#define _MSGDLG_HXX_

#include <basedlg.hxx>
#include <baselistener.hxx>

namespace scsolver { 


class MessageDialog : public BaseDialog
{
public:
	MessageDialog( SolverImpl*, const rtl::OUString& );
	virtual ~MessageDialog() throw();

	virtual const rtl::OUString getDialogName() const { return ascii( "MessageDialog" ); }
	virtual void setVisible( bool );
	virtual bool doneRangeSelection() const { return true; }

	void setMessage( const rtl::OUString& );
	void initialize();
	void registerListeners();

private:
	rtl::OUString m_sMessage;
};



}


#endif
