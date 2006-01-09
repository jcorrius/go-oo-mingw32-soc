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

#include <com/sun/star/awt/XFixedText.hpp>
#include <msgdlg.hxx>
#include <memory>

namespace scsolver {

MessageDialog::MessageDialog( SolverImpl* p, const rtl::OUString& sMsg ) 
		: BaseDialog( p ), m_sMessage( sMsg )
{
	initialize();
}

MessageDialog::~MessageDialog() throw()
{
}

void MessageDialog::setVisible( bool bVisible )
{
	if ( bVisible )
		toFront();

	setVisibleDefault( bVisible );
}

void MessageDialog::setMessage( const rtl::OUString& sMsg )
{
	m_sMessage = sMsg;
	Reference< uno::XInterface > oWgt = getWidgetByName( ascii( "ftMessage" ) );
	Reference< awt::XFixedText > xTxt( oWgt, UNO_QUERY );
	xTxt->setText( sMsg );
}

void MessageDialog::initialize()
{
	const sal_Int16 nBtnWidth = 30, nBtnHeight = 14, nMargin = 3;
	sal_Int16 nWidth = 120, nHeight = 40;

	BaseDialog::initialize( nWidth, nHeight, ascii( "Message Dialog" ) );

	sal_Int32 nX, nY;
	addFixedText( 5, 5, nWidth - 10, 10, ascii( "ftMessage" ), m_sMessage );
	addButton( (nWidth - nBtnWidth)/2, nHeight - nBtnHeight - nMargin, nBtnWidth, nBtnHeight, 
			ascii( "btnOk" ), ascii( "OK" ), awt::PushButtonType_OK );

	registerListeners();
}

void MessageDialog::registerListeners()
{
}


}






