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

#include "optiondlg.hxx"
#include "unoglobal.hxx"
#include "global.hxx"
#include "listener.hxx"
#include "solver.hxx"

#include "com/sun/star/awt/XCheckBox.hpp"

#include "scsolver.hrc"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace scsolver {

/**
 * This class defines the action to be performed when the OK
 * button in the Option dialog is pressed.
 */
class OptionDlgOKAction : public ActionObject
{
public:
	OptionDlgOKAction()
	{
	}

	virtual ~OptionDlgOKAction() throw()
	{
	}

	virtual void execute( BaseDialog *dlg, const awt::ActionEvent &e )
	{
		OptionDialog* p = static_cast<OptionDialog*>(dlg);
		p->setVisible(false);
		p->getSolverImpl()->getOptionData()->setModelType( p->getModelType() );
	}
};

OptionDialog::OptionDialog( SolverImpl* p ) :
	BaseDialog( p ),
	m_pActionOK( new OptionDlgOKAction )
{
	initialize();
}

OptionDialog::~OptionDialog() throw()
{
}

void OptionDialog::initialize()
{
	int nWidth = 200, nHeight = 150;
	BaseDialog::initialize( 
		static_cast<sal_Int16>(nWidth), static_cast<sal_Int16>(nHeight), 
		ascii_i18n("Options") );

	sal_Int32 nX = 5, nY = 5, nMargin = 5;

	addFixedLine( nX, nY, nWidth-nX-nMargin, 12, ascii("flOptions"), 
				  ascii_i18n("Options") );
	nY += 13;
	addCheckBox( nX, nY+2, nWidth-nX-nMargin, 12, ascii("cbLinear"),
				 ascii_i18n("Assume linear model") );

	addButton( nWidth-110, nHeight-20, 50, 15, ascii("btnOK"), 
			   getResStr(SCSOLVER_STR_BTN_OK) );

	addButton( nWidth-55, nHeight-20, 50, 15, ascii("btnCancel"), 
			   getResStr(SCSOLVER_STR_BTN_CANCEL) );

	registerListeners();
}

void OptionDialog::registerListeners()
{
	m_pOKListener = new ActionListener( this, m_pActionOK.get() );
	registerListener( ascii("btnOK"), m_pOKListener );

	m_pCloseListener = new CloseBtnListener(this);
	registerListener( ascii("btnCancel"), m_pCloseListener );
}

void OptionDialog::unregisterListeners() throw()
{
	unregisterListener( ascii("btnCancel"), m_pCloseListener );
}

bool OptionDialog::doneRangeSelection() const
{
	return false;
}

const rtl::OUString OptionDialog::getDialogName() const
{
	return ascii("OptionDialog");
}

void OptionDialog::setVisible( bool b )
{
	setVisibleDefault( b );
}

OptModelType OptionDialog::getModelType() const
{
	Reference<uno::XInterface> oWgt = getWidgetByName( ascii("cbLinear") );
	Reference<awt::XCheckBox> xCB( oWgt, UNO_QUERY );

	return xCB->getState() ? OPTMODELTYPE_LP : OPTMODELTYPE_NLP;
}

void OptionDialog::setModelType( OptModelType type )
{
	Reference<uno::XInterface> oWgt = getWidgetByName( ascii("cbLinear") );
	Reference<awt::XCheckBox> xCB( oWgt, UNO_QUERY );
	if ( type == OPTMODELTYPE_LP || type == OPTMODELTYPE_MILP )
		xCB->setState(1);
	else
		xCB->setState(0);
}

}
