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

 
#ifndef _DIALOG_HXX_
#define _DIALOG_HXX_

#include <type.hxx>
#include <basedlg.hxx>
#include <unoglobal.hxx>
#include <numeric/type.hxx>
#include <memory>
#include <com/sun/star/awt/PushButtonType.hpp>
#include <com/sun/star/awt/Rectangle.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using scsolver::numeric::opres::Equality;
using scsolver::numeric::opres::Goal;

namespace com { namespace sun { namespace star { namespace awt {

	class XTextComponent;

}}}}

namespace scsolver {

class SolverImpl;

class Listener;
class RngBtnListener;
class BaseDialog;
class WidgetProperty;
class MessageDialog;

class SolverDialog;
class SolveBtnListener;
class CloseBtnListener;
class SaveBtnListener;
class LoadBtnListener;
class ResetBtnListener;
class MaxRadioBtnListener;
class ConstEditBtnListener;
class WindowFocusListener;
class WindowMouseListener;
class ConstListBoxListener;
	
class OKCancelBtnListener;


//---------------------------------------------------------------------------
// Class ConstEditDialog


class ConstEditDialog : public scsolver::BaseDialog
{
public:
	ConstEditDialog( SolverImpl* );
	virtual ~ConstEditDialog() throw();
	virtual const rtl::OUString getDialogName() const { return ascii( "ConstEditDialog" ); }
	virtual void setVisible( bool );
	virtual bool doneRangeSelection() const;

	void initialize();
	void reset();
	
	sal_uInt32 getConstraintId() const { return m_nConstraintId; }
	rtl::OUString getLeftCellReference();
	rtl::OUString getRightCellReference();
	Equality getEquality();

	void setChangeMode( sal_Bool b ) { m_bIsChangeMode = b; }
	void setConstraintId( sal_uInt32 );
	
	const rtl::OUString getLeftCellReference() const;
	const rtl::OUString getRightCellReference() const;
	void setLeftCellReference( const rtl::OUString& );
	void setRightCellReference( const rtl::OUString& );

	void setEquality( const Equality );
	
	bool isChangeMode() const { return m_bIsChangeMode; }
	bool isCellRangeGeometryEqual() const;
	
protected:
	virtual void registerListeners();
	virtual void unregisterListeners();

private:
	bool m_bIsChangeMode;
	sal_uInt32 m_nConstraintId;

	OKCancelBtnListener* m_pOKListener;
	OKCancelBtnListener* m_pCancelListener;
	RngBtnListener* m_pLeftRngListener;
	RngBtnListener* m_pRightRngListener;
};

//---------------------------------------------------------------------------
// Class SolverDialog

class SolverDialog : public BaseDialog
{

public:
	
	SolverDialog( SolverImpl* );
	virtual ~SolverDialog() throw();
	virtual const rtl::OUString getDialogName() const { return ascii( "SolverDialog" ); }
	virtual void setVisible( bool );
	virtual bool doneRangeSelection() const { return true; }

	void initialize();
	

	ConstEditDialog* getConstEditDialog();

	sal_Int16 getSelectedConstraintPos();
	
	Reference< awt::XTextComponent > getXTextComponentFromWidget( const rtl::OUString& ) const;
	
	rtl::OUString getTargetCellAddress() const;
	void setTargetCellAddress( const rtl::OUString& );
	
	rtl::OUString getVarCellAddress();
	void setVarCellAddress( const rtl::OUString& );
	
	Goal getGoal() const;
	void setGoal( Goal );
	
	void output();
	
	void editConstraint( const sal_uInt32, const rtl::OUString&, const rtl::OUString&, 
			const Equality );
	void getConstraint( const sal_uInt32, rtl::OUString&, rtl::OUString&, Equality& );
	std::vector< ConstraintString > getAllConstraints() const;
	void removeConstraint( const sal_uInt32 );
	void clearConstraints();
	
	void setConstraint( const rtl::OUString&, const rtl::OUString&, const Equality );

	void showSolveError( const rtl::OUString& );
	void showSolutionInfeasible();
	void showSolutionFound();
	
	void reset();
	void saveModelToDocument();
	void loadModelFromDocument();

protected:
	
	virtual void registerListeners();
	virtual void unregisterListeners();
	
private:
	
	void removeConstraintsFromListBox( sal_Int16 = 0, sal_Int16 = 0 );
	void setConstraintImpl( const rtl::OUString&, const rtl::OUString&, const Equality,
			const sal_Bool, const sal_uInt32 = 0 );

	std::auto_ptr<ConstEditDialog> m_pConstEditDialog;
	std::auto_ptr<MessageDialog> m_pSolveErrorDlg;
	std::auto_ptr<MessageDialog> m_pSolInfeasibleDlg;
	std::auto_ptr<MessageDialog> m_pSolFoundDlg;

	// Action Listeners
	RngBtnListener* m_pTargetRngListener;
	RngBtnListener* m_pVarCellsRngListener;
	WindowMouseListener* m_pWindowMouseListener;
	SolveBtnListener* m_pSolveListener;
	CloseBtnListener* m_pOKListener;
	SaveBtnListener* m_pSaveListener;
	LoadBtnListener* m_pLoadListener;
	ResetBtnListener* m_pResetListener;
	MaxRadioBtnListener* m_pMaxListener;
	ConstEditBtnListener* m_pConstAddListener;
	ConstEditBtnListener* m_pConstChangeListener;
	ConstEditBtnListener* m_pConstDeleteListener;
	WindowFocusListener* m_pWindowFocusListener;
	ConstListBoxListener* m_pConstListBoxListener;

	// Constraint
	std::vector< ConstraintString > m_aConstraint;
};

//--------------------------------------------------------------------------


}


#endif //_DIALOG_HXX_
