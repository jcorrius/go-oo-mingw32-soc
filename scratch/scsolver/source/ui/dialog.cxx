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


#include <dialog.hxx>
#include <global.hxx>
#include <unoglobal.hxx>
#include <solver.hxx>
#include <listener.hxx>
#include <xcalc.hxx>
#include <unohelper.hxx>
#include <msgdlg.hxx>
#include <rtl/ustrbuf.hxx>

#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XListBox.hpp>
#include <com/sun/star/awt/XRadioButton.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XWindow.hpp>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XServiceName.hpp>

#include <com/sun/star/sheet/XRangeSelection.hpp>

#include <com/sun/star/uno/XComponentContext.hpp>

#include <iostream>

using namespace std;
using namespace ::scsolver::numeric;
using namespace ::scsolver::numeric::opres;
using ::com::sun::star::uno::Reference;

namespace scsolver {

//---------------------------------------------------------------------------
// class ConstDialog Definitions

ConstEditDialog::ConstEditDialog( SolverImpl* p )
	: BaseDialog( p ),
	m_bIsChangeMode( false ),
	m_nConstraintId( 0 ),
	m_pOKListener( NULL ),
	m_pCancelListener( NULL ),
	m_pLeftRngListener( NULL ),
	m_pRightRngListener( NULL )
{
	initialize();
}

ConstEditDialog::~ConstEditDialog() throw()
{	
	unregisterListeners();
}

void ConstEditDialog::registerListeners()
{
	CalcInterface* pCalc = getSolverImpl()->getCalcInterface();

	m_pOKListener = new OKCancelBtnListener( this, ascii( "OK" ) );
	registerListener( ascii( "btnOK" ), m_pOKListener );
	
	m_pCancelListener = new OKCancelBtnListener( this, ascii( "Cancel" ) );
	registerListener( ascii( "btnCancel" ), m_pCancelListener );
	
	Reference< sheet::XRangeSelection > xRngSel = pCalc->getXRangeSelection();
	assert( xRngSel != NULL );
	
	m_pLeftRngListener = new RngBtnListener( this, xRngSel, ascii( "editLeft" ) );
	m_pLeftRngListener->setSingleCell( true );
	registerListener( ascii( "btnLeft" ), m_pLeftRngListener );
		
	m_pRightRngListener = new RngBtnListener( this, xRngSel, ascii( "editRight" ) );
	m_pRightRngListener->setSingleCell( true );
	registerListener( ascii( "btnRight" ), m_pRightRngListener );
}

void ConstEditDialog::unregisterListeners()
{
	unregisterListener( ascii( "btnOK" ), m_pOKListener );
	unregisterListener( ascii( "btnCancel" ), m_pCancelListener );
	unregisterListener( ascii( "btnLeft" ), m_pLeftRngListener );
	unregisterListener( ascii( "btnRight" ), m_pRightRngListener );
}

void ConstEditDialog::initialize()
{
	BaseDialog::initialize( static_cast<sal_Int16>(180), static_cast<sal_Int16>(55), 
		ascii( "Constraint" ) );
	
	sal_Int32 nX, nY;
	
	nX = 5;
	nY = 5;
	addFixedText( nX, nY, 100, 10, ascii( "ftLeft" ), ascii( "Cell Reference" ) );
	nY += 10;
	addRangeEdit( nX, nY, 70, 12, ascii( "editLeft" ), ascii( "btnLeft" ) );
	nX += 72;
	apWidgetProp p = addListBox( nX, nY, 25, 12, ascii( "lbEqual" ) );

	uno::Any aDropdown, aItems, aMultiSel, aSelItems;
	uno::Sequence< rtl::OUString > sItems( 3 );
	sItems[0] = ascii( "<=" );
	sItems[1] = ascii( "=" );
	sItems[2] = ascii( ">=" );
	aItems <<= sItems;
	uno::Sequence< sal_Int16 > sSelItems( 1 );
	sSelItems[0] = 0;
	aSelItems <<= sSelItems;
	aDropdown <<= static_cast<sal_Bool>(true);
	aMultiSel <<= static_cast<sal_Bool>(false);
	p->setPropertyValue( "Dropdown", aDropdown );
	p->setPropertyValue( "StringItemList", aItems );
	p->setPropertyValue( "MultiSelection", aMultiSel );
	p->setPropertyValue( "SelectedItems", aSelItems );
	
	nX += 27;
	addFixedText( nX, 5, 100, 10, ascii( "ftRight" ), ascii( "Constraint" ) );
	addRangeEdit( nX, nY, 70, 12, ascii( "editRight" ), ascii( "btnRight" ) );
	
	nX = 5;
	nY = 35;
	addButton( nX, nY, 50, 15, ascii( "btnOK" ), ascii( "OK" ) );	
	addButton( nX+55, nY, 50, 15, ascii( "btnCancel" ), ascii( "Cancel" ) );	

	registerListeners();
}

void ConstEditDialog::reset()
{
	Reference< uno::XInterface > oEditLeft = getWidgetByName( ascii( "editLeft" ) );
	assert( oEditLeft != NULL );
	Reference< awt::XTextComponent > xCompL( oEditLeft, UNO_QUERY );
	xCompL->setText( ascii( "" ) );
	
	Reference< uno::XInterface > oEditRight = getWidgetByName( ascii( "editRight" ) );
	assert( oEditRight != NULL );
	Reference< awt::XTextComponent > xCompR( oEditRight, UNO_QUERY );
	xCompR->setText( ascii( "" ) );
	
	Reference< uno::XInterface > oEq = getWidgetByName( ascii( "lbEqual" ) );
	assert( oEq != NULL );
	Reference< awt::XListBox > xEq( oEq, UNO_QUERY );
	xEq->selectItemPos( 0, true );
}

void ConstEditDialog::setVisible( bool bVisible )
{
	if ( bVisible )
		toFront();

	setVisibleDefault( bVisible );
}

bool ConstEditDialog::doneRangeSelection() const
{
	SolverDialog* pDlg = getSolverImpl()->getMainDialog();
	pDlg->setVisible( true );

	if ( !isCellRangeGeometryEqual() )
	{
		pDlg->showSolveError( ascii( "The reference and constraint ranges do not match." ) );
		return false;
	}

	return true;
}

rtl::OUString ConstEditDialog::getLeftCellReference()
{
	Reference< uno::XInterface > oEdit = getWidgetByName( ascii( "editLeft" ) );
	Reference< awt::XTextComponent > xComp( oEdit, UNO_QUERY );
	return xComp->getText();
}

rtl::OUString ConstEditDialog::getRightCellReference()
{
	Reference< uno::XInterface > oEdit = getWidgetByName( ascii( "editRight" ) );
	Reference< awt::XTextComponent > xComp( oEdit, UNO_QUERY );
	return xComp->getText();
}

Equality ConstEditDialog::getEquality()
{
	Reference< uno::XInterface > oEq = getWidgetByName( ascii( "lbEqual" ) );
	Reference< awt::XListBox > xEq( oEq, UNO_QUERY );
	if ( xEq != NULL )
	{
		sal_Int16 nPos = xEq->getSelectedItemPos();
		switch( nPos )
		{
			case 0:
				return LESS_THAN_EQUAL;
				break;
			case 1:
				return EQUAL;
				break;
			case 2:
				return GREATER_THAN_EQUAL;
				break;
			default:
				assert( !"Unexpected (in)equal sign" );
		}
	}
	else
		assert( !"xEq == NULL" );
	
	return EQUAL;
}

void ConstEditDialog::setConstraintId( sal_uInt32 nId )
{
	m_nConstraintId = nId;
}

const rtl::OUString ConstEditDialog::getLeftCellReference() const
{
	Reference< uno::XInterface > oEdit = getWidgetByName( ascii( "editLeft" ) );
	Reference< awt::XTextComponent > xComp( oEdit, UNO_QUERY );
	return xComp->getText();
}

const rtl::OUString ConstEditDialog::getRightCellReference() const
{
	Reference< uno::XInterface > oEdit = getWidgetByName( ascii( "editRight" ) );
	Reference< awt::XTextComponent > xComp( oEdit, UNO_QUERY );
	return xComp->getText();
}

void ConstEditDialog::setLeftCellReference( const rtl::OUString& s )
{
	Reference< uno::XInterface > oEdit = getWidgetByName( ascii( "editLeft" ) );
	Reference< awt::XTextComponent > xComp( oEdit, UNO_QUERY );	
	xComp->setText( s );
}

void ConstEditDialog::setRightCellReference( const rtl::OUString& s )
{
	Reference< uno::XInterface > oEdit = getWidgetByName( ascii( "editRight" ) );
	Reference< awt::XTextComponent > xComp( oEdit, UNO_QUERY );	
	xComp->setText( s );
}

void ConstEditDialog::setEquality( const Equality e )
{
	Reference< uno::XInterface > oEq = getWidgetByName( ascii( "lbEqual" ) );
	Reference< awt::XListBox > xEq( oEq, UNO_QUERY );
	if ( xEq != NULL )
	{
		switch( e )
		{
			case LESS_THAN_EQUAL:
				xEq->selectItemPos( 0, true );
				break;
			case EQUAL:
				xEq->selectItemPos( 1, true );
				break;
			case GREATER_THAN_EQUAL:
				xEq->selectItemPos( 2, true );
				break;
			default:
				assert( !"Unexpected Equality" );
		}
	}
	else
		assert( !"xEq == NULL" );
}

/** Checks if given two cell range references in the constraint dialog have 
	identical geometry. 
	
	@return true if the two cell ranges have identical geometry or at least
	one cell range is empty, or false otherwise.
 */
bool ConstEditDialog::isCellRangeGeometryEqual() const
{
	using table::CellRangeAddress;
	
	rtl::OUString sLeftRef = getLeftCellReference();
	rtl::OUString sRightRef = getRightCellReference();
	if ( sLeftRef.getLength() > 0 && sRightRef.getLength() > 0 )
	{
		CalcInterface* pCalc = getSolverImpl()->getCalcInterface();

		CellRangeAddress aLAddr = pCalc->getCellRangeAddress( sLeftRef );
		CellRangeAddress aRAddr = pCalc->getCellRangeAddress( sRightRef );
		sal_Int32 nLCol = aLAddr.EndColumn - aLAddr.StartColumn;
		sal_Int32 nRCol = aRAddr.EndColumn - aRAddr.StartColumn;
		sal_Int32 nLRow = aLAddr.EndRow - aLAddr.StartRow;
		sal_Int32 nRRow = aRAddr.EndRow - aRAddr.StartRow;

		return nLCol == nRCol && nLRow == nRRow;
	}

	return true;
}


//---------------------------------------------------------------------------
// class SolverDialog Definitions

SolverDialog::SolverDialog( SolverImpl* p )
	: BaseDialog( p ),

	m_pConstEditDialog( NULL ),
	m_pSolveErrorDlg( NULL ),
	m_pSolInfeasibleDlg( NULL ),
	m_pSolFoundDlg( NULL ),
	m_pTargetRngListener( NULL ),
	m_pVarCellsRngListener( NULL ),
	m_pWindowMouseListener( NULL ),
	m_pSolveListener( NULL ),
	m_pOKListener( NULL ),
	m_pSaveListener( NULL ),
	m_pLoadListener( NULL ),
	m_pResetListener( NULL ),
	m_pMaxListener( NULL ),
	m_pConstAddListener( NULL ),
	m_pConstChangeListener( NULL ),
	m_pConstDeleteListener( NULL ),
	m_pWindowFocusListener( NULL ),
	m_pConstListBoxListener( NULL )
{
	initialize();
}

SolverDialog::~SolverDialog() throw()
{	
	unregisterListeners();
}

void SolverDialog::initialize()
{
	BaseDialog::initialize( static_cast<sal_Int16>(260), static_cast<sal_Int16>(200), 
		ascii( "Optimization Solver" ) );

	// START CREATING WIDGETS
	sal_Int32 nX, nY;
	
	nY = 5;
	addFixedLine( 5, nY, 195, 12, ascii( "flModel" ), ascii( "Define Model" ) );
	
	nY += 13;
	addFixedText( 10, nY+2, 60, 12, ascii( "ftTargetCell" ), ascii( "Set target cell" ) );
	addRangeEdit( 70, nY, 80, 12, ascii( "editTargetCell" ), ascii( "btnTargetCellRange" ) );

	nY += 18;
	addFixedText( 10, nY, 35, 12, ascii( "ftObj" ), ascii( "Goal" ) );
	addRadioButton( 50, nY, 40, 12, ascii( "rbMax" ), ascii( "Maximize" ) );
	addRadioButton( 95, nY, 40, 12, ascii( "rbMin" ), ascii( "Minimize" ) );
	
	nY += 15;
	addFixedText( 10, nY+2, 60, 12, ascii( "ftDecVars" ), ascii( "By changing cells" ) );
	addRangeEdit( 70, nY, 115, 12, ascii( "editVarCells" ), ascii( "btnRangeSelect" ) );
	
	nY += 20;
	addFixedLine( 5, nY, 195, 12, ascii( "flConstraints" ), ascii( "Subject to the Constraints" ) );
	
	nY += 13;
	nX = 10;
	addListBox( nX, nY, 130, 80, ascii( "lbConstraint" ) );
	nX += 135;

	addButton( nX, nY,    50, 15, ascii( "btnConstAdd" ), ascii( "Add" ) );

	uno::Any aBool;
	aBool <<= static_cast<sal_Bool>(false);

	apWidgetProp p = addButton( 
		nX, nY+20, 50, 15, ascii( "btnConstChange" ), ascii( "Change" ) );

	p->setPropertyValue( "Enabled", aBool );
	
	p = addButton( 
		nX, nY+40, 50, 15, ascii( "btnConstDelete" ), ascii( "Delete" ) );

	p->setPropertyValue( "Enabled", aBool );
	
	addButton( 205, 10, 50, 15, ascii( "btnSolve" ), ascii( "Solve" ) );
	addButton( 205, 30, 50, 15, ascii( "btnReset" ), ascii( "Reset" ) );
	addButton( 205, 50, 50, 15, ascii( "btnOptions" ), ascii( "Options" ) );

	addButton( 205, 90, 50, 15, ascii( "btnSave" ), ascii( "Save" ) );
	addButton( 205, 110, 50, 15, ascii( "btnLoad" ), ascii( "Load" ) );

	addButton( 205, 180, 50, 15, ascii( "btnClose" ), ascii( "Close" ) );

	registerListeners();
}

void SolverDialog::registerListeners()
{
	CalcInterface* pCalc = getSolverImpl()->getCalcInterface();

	Reference< sheet::XRangeSelection > xRngSel = pCalc->getXRangeSelection();
	assert( xRngSel != NULL );

	m_pVarCellsRngListener = new RngBtnListener( this, xRngSel, ascii( "editVarCells" ) );
	m_pVarCellsRngListener->setSingleCell( false );
	registerListener( ascii( "btnRangeSelect" ), m_pVarCellsRngListener );

	m_pTargetRngListener = new RngBtnListener( this, xRngSel, ascii( "editTargetCell" ) );
	m_pTargetRngListener->setSingleCell( true );
	registerListener( ascii( "btnTargetCellRange" ), m_pTargetRngListener );
	
	m_pSolveListener = new SolveBtnListener( this );
	registerListener( ascii( "btnSolve" ), m_pSolveListener );
	
	m_pOKListener = new CloseBtnListener( this );
	registerListener( ascii( "btnClose" ), m_pOKListener );
	
	m_pSaveListener = new SaveBtnListener( this );
	registerListener( ascii( "btnSave" ), m_pSaveListener );
	m_pLoadListener = new LoadBtnListener( this );
	registerListener( ascii( "btnLoad" ), m_pLoadListener );
	
	m_pResetListener = new ResetBtnListener( this );
	registerListener( ascii( "btnReset" ), m_pResetListener );

	m_pMaxListener = new MaxRadioBtnListener( this );
	registerListener( ascii( "rbMax" ), m_pMaxListener );
	
	m_pConstAddListener = new ConstEditBtnListener( this, CONST_ADD );
	registerListener( ascii( "btnConstAdd" ), m_pConstAddListener );

	m_pConstChangeListener = new ConstEditBtnListener( this, CONST_CHANGE );
	registerListener( ascii( "btnConstChange" ), m_pConstChangeListener );

	m_pConstDeleteListener = new ConstEditBtnListener( this, CONST_DELETE );
	registerListener( ascii( "btnConstDelete" ), m_pConstDeleteListener );

	m_pConstListBoxListener = new ConstListBoxListener( this );
	registerListener( ascii( "lbConstraint" ), m_pConstListBoxListener );
	
	m_pWindowFocusListener = new WindowFocusListener( this );
	registerListener( m_pWindowFocusListener );

	m_pWindowMouseListener = new WindowMouseListener( this );
	registerListener( m_pWindowMouseListener );
}

void SolverDialog::unregisterListeners()
{
	Debug( "SolverDialog::unregisterListeners" );
	
	// Unregistering a listener object from its associated widget causes it
	// to be automatically destroyed, so that a manual delete of such object 
	// is no longer necessary.
	
	unregisterListener( ascii( "btnRangeSelect" ), m_pVarCellsRngListener );
	unregisterListener( ascii( "btnTargetCellRange" ), m_pTargetRngListener );
	unregisterListener( ascii( "btnClose" ), m_pOKListener );
	unregisterListener( ascii( "rbMax" ), m_pMaxListener );
	unregisterListener( ascii( "btnConstAdd" ), m_pConstAddListener );
	unregisterListener( ascii( "btnConstChange" ), m_pConstChangeListener );
	unregisterListener( ascii( "btnConstDelete" ), m_pConstDeleteListener );
	unregisterListener( ascii( "lbConstraint" ), m_pConstListBoxListener );
	unregisterListener( m_pWindowFocusListener );
	unregisterListener( m_pWindowMouseListener );
}

void SolverDialog::setVisible( bool bVisible )
{
	if ( bVisible )
	{
		// Get currently active sheet (no use as long as the dialog is modeless).
		CalcInterface* pCalc = getSolverImpl()->getCalcInterface();

		Reference< sheet::XSpreadsheet > xSheet = pCalc->getActiveSheet();
		Reference< container::XNamed > xNamed( xSheet, UNO_QUERY );
		printOUStr( ascii( "Current sheet is " ) + xNamed->getName() );
	}
	else
		getConstEditDialog()->setVisible( false );

	setVisibleDefault( bVisible );
}

ConstEditDialog* SolverDialog::getConstEditDialog()
{
	if ( m_pConstEditDialog.get() == NULL )
		m_pConstEditDialog.reset( new ConstEditDialog( getSolverImpl() ) );

	return m_pConstEditDialog.get();
}

sal_Int16 SolverDialog::getSelectedConstraintPos()
{
	Reference< uno::XInterface > oWgt = getWidgetByName( ascii( "lbConstraint" ) );
	assert( oWgt != NULL );
	Reference< awt::XListBox > xLB( oWgt, UNO_QUERY );
	assert( xLB != NULL );
	return xLB->getSelectedItemPos();
}

Reference< awt::XTextComponent > SolverDialog::getXTextComponentFromWidget(
	const rtl::OUString& sName ) const
{
	Reference< uno::XInterface > oRngEdit = getWidgetByName( sName );
	assert( oRngEdit != NULL );
	Reference< awt::XTextComponent > xComp( oRngEdit, UNO_QUERY );
	return xComp;
}

rtl::OUString SolverDialog::getTargetCellAddress() const
{
	Reference< awt::XTextComponent > xComp = getXTextComponentFromWidget( 
			ascii( "editTargetCell" ) );
	return xComp->getText();
}

void SolverDialog::setTargetCellAddress( const rtl::OUString& sAddr )
{
	Reference< awt::XTextComponent > xComp = getXTextComponentFromWidget( 
			ascii( "editTargetCell" ) );
	xComp->setText( sAddr );
}

rtl::OUString SolverDialog::getVarCellAddress()
{
	Reference< awt::XTextComponent > xComp = getXTextComponentFromWidget( 
			ascii( "editVarCells" ) );
	return xComp->getText();
}

void SolverDialog::setVarCellAddress( const rtl::OUString& sAddr )
{
	Reference< awt::XTextComponent > xComp = getXTextComponentFromWidget( 
			ascii( "editVarCells" ) );
	xComp->setText( sAddr );
}

Goal SolverDialog::getGoal() const
{
	Reference< uno::XInterface > xModel = getWidgetModelByName( ascii( "rbMax" ) );
	sal_Int16 nState;
	unohelper::getPropertyValue( xModel, ascii( "State" ), nState );
	if ( nState )
		return GOAL_MAXIMIZE;
	
	xModel = getWidgetModelByName( ascii( "rbMin" ) );
	unohelper::getPropertyValue( xModel, ascii( "State" ), nState );
	if ( nState )
		return GOAL_MINIMIZE;

	return GOAL_UNKNOWN;
}

void SolverDialog::setGoal( Goal eGoal )
{
	rtl::OUString sName = ascii( "" );
	switch ( eGoal )
	{
	case GOAL_MAXIMIZE:
		sName = ascii( "rbMax" );
		break;
	case GOAL_MINIMIZE:
		sName = ascii( "rbMin" );
		break;
	case GOAL_UNKNOWN:
		break;
	}

	if ( sName.getLength() > 0 )
	{
		Reference< uno::XInterface > xModel = getWidgetModelByName( sName );
		sal_Int16 nState = 1;
		uno::Any aVal;
		aVal <<= nState;
		unohelper::setPropertyValue( xModel, ascii( "State" ), aVal );
	}
}

void SolverDialog::output()
{
	cout << "----------------------------------------------" << endl;
	cout << "Subject to the constraints:" << endl;

	vector< ConstraintString >::iterator pos = m_aConstraint.begin();
	while( pos != m_aConstraint.end() )
	{
		ConstraintString aItem = *pos;
		rtl::OUString sLine = ascii( "  " ) + aItem.Left;
		switch ( aItem.Equal )
		{
			case GREATER_THAN_EQUAL:
				sLine += ascii( " >= " );
				break;
			case EQUAL:
				sLine += ascii( " = " );
				break;
			case LESS_THAN_EQUAL:
				sLine += ascii( " <= " );
				break;
			default:
				assert( !"wrong equality type" );
		}
		sLine += aItem.Right;
		printOUStr( sLine );
		++pos;
	}
	cout << "----------------------------------------------" << endl;
}

void SolverDialog::getConstraint( const sal_uInt32 nIdx, 
	rtl::OUString& sLeft, rtl::OUString& sRight, Equality& eEq )
{
	ConstraintString aConstItem = m_aConstraint.at( nIdx );
	sLeft = aConstItem.Left;
	sRight = aConstItem.Right;
	eEq = aConstItem.Equal;
}

vector< ConstraintString > SolverDialog::getAllConstraints() const
{
	return m_aConstraint;
}

void SolverDialog::setConstraint( 
		const rtl::OUString& sLeft, const rtl::OUString& sRight, const Equality eEq )
{
	ConstraintString aConstItem;
	aConstItem.Left = sLeft;
	aConstItem.Right = sRight;
	aConstItem.Equal = eEq;
	m_aConstraint.push_back( aConstItem );
	
	output();
	setConstraintImpl( sLeft, sRight, eEq, false );
}

void SolverDialog::editConstraint( const sal_uInt32 nId,
		const rtl::OUString& sLeft, const rtl::OUString& sRight, const Equality eEqVal )
{
	if ( m_aConstraint.size() <= nId )
		return;
	
	vector< ConstraintString > aConstraint;
	vector< ConstraintString >::iterator pos = m_aConstraint.begin();
	sal_uInt32 nCurId = 0;
	while ( pos != m_aConstraint.end() )
	{
		if ( nCurId == nId )
		{
			ConstraintString aConstItem;
			aConstItem.Left = sLeft;
			aConstItem.Right = sRight;
			aConstItem.Equal = eEqVal;
			m_aConstraint.erase( pos );
			m_aConstraint.insert( pos, aConstItem );
		}
		++pos; ++nCurId;
	}
	
	output();
	setConstraintImpl( sLeft, sRight, eEqVal, true, nId );
}

void SolverDialog::removeConstraint( const sal_uInt32 nIdx )
{	
	if ( m_aConstraint.size() <= nIdx )
		return;
	
	sal_uInt32 nCurId = 0;
	vector< ConstraintString >::iterator pos = m_aConstraint.begin();
	while ( pos != m_aConstraint.end() )
	{
		if ( nCurId == nIdx )
		{
			m_aConstraint.erase( pos );
			break;
		}
		++pos; ++nCurId;
	}

	output();
	
	removeConstraintsFromListBox( nIdx, 1 );
}

/** Remove all constraints. */
void SolverDialog::clearConstraints()
{
	m_aConstraint.clear();
	removeConstraintsFromListBox();
}

/** Remove constraint entries from the list box.

	@param nPos position of the first entry to be removed
	@param nCount number of entires to be removed (all remaining items if nCount = 0)
*/
void SolverDialog::removeConstraintsFromListBox( sal_Int16 nPos, sal_Int16 nCount )
{
	Reference< uno::XInterface > oWgt = getWidgetByName( ascii( "lbConstraint" ) );
	assert( oWgt != NULL );
	Reference< awt::XListBox > xLB( oWgt, UNO_QUERY );
	assert( xLB != NULL );
	if ( nCount == 0 )
	{
		sal_Int16 nItemCount = xLB->getItemCount();
		nCount = nItemCount - nPos;
	}
	xLB->removeItems( nPos, nCount );	
}

/** Set a new constraint or modify an existing one within the dialog.

	Implements displaying of a new constraint or an existing constraint that has
	been modified.  Note that this function takes care of just the view portion
	of the addition/modification, and it does NOT alter the internal constraint 
	data in a model.
*/
void SolverDialog::setConstraintImpl( 
		const rtl::OUString& sLeft, const rtl::OUString& sRight, const Equality eEq,
		const sal_Bool bChangeMode, const sal_uInt32 nId )
{
	rtl::OUString sEq;
	switch( eEq )
	{
		case GREATER_THAN_EQUAL:
			sEq = ascii( ">=" );
			break;
		case EQUAL:
			sEq = ascii( "=" );
			break;
		case LESS_THAN_EQUAL:
			sEq = ascii( "<=" );
			break;
		default:
			assert( !"Unknown equality" );
	}
	
	rtl::OUString sWhole = sLeft + ascii( " " ) + sEq + ascii( " " ) + sRight;
	printOUStr( sWhole );
 	Reference< uno::XInterface > oLB = getWidgetByName( ascii( "lbConstraint" ) );
 	if ( oLB != NULL )
 	{
 		Reference< awt::XListBox > xLB( oLB, UNO_QUERY );
 		if ( xLB != NULL )
		{
			if ( bChangeMode )
			{
				xLB->removeItems( nId, 1 );
				xLB->addItem( sWhole, nId );
			}
			else
			{
				sal_Int16 nCount = xLB->getItemCount();
 				xLB->addItem( sWhole, nCount );
			}
		}
		else
			assert( !"xLB is NULL" );
 	}
 	else
 		assert( !"oLB is NULL" );
}

void SolverDialog::showSolveError( const rtl::OUString& sMsg )
{
	if ( m_pSolveErrorDlg.get() == NULL )
		m_pSolveErrorDlg.reset( new MessageDialog( getSolverImpl(), sMsg ) );
	else
		m_pSolveErrorDlg->setMessage( sMsg );
	m_pSolveErrorDlg->execute();
}

void SolverDialog::showSolutionInfeasible()
{
	if ( m_pSolInfeasibleDlg.get() == NULL )
		m_pSolInfeasibleDlg.reset( new MessageDialog( getSolverImpl(), 
				ascii( "Solution could not be found" ) ) );
	m_pSolInfeasibleDlg->execute();
}

void SolverDialog::showSolutionFound()
{
	if ( m_pSolFoundDlg.get() == NULL )
		m_pSolFoundDlg.reset( new MessageDialog( getSolverImpl(), 
				ascii( "Solution found" ) ) );
	m_pSolFoundDlg->execute();
}

void SolverDialog::reset()
{
	setTextByWidget( this, ascii( "editTargetCell" ), ascii( "" ) );
	setTextByWidget( this, ascii( "editVarCells" ), ascii( "" ) );
	setGoal( GOAL_UNKNOWN );
	clearConstraints();
}

void SolverDialog::saveModelToDocument()
{
	using rtl::OUString;
	using rtl::OUStringBuffer;
	
	OUStringBuffer sb;
	
	sb.append( getTextByWidget( this, ascii( "editTargetCell" ) ) );
	sb.append( Global::STRING_SEPARATOR );
	Goal eGoal = getGoal();
	switch ( eGoal )
	{
	case GOAL_MAXIMIZE:
		sb.append( ascii( "max" ) );
		break;
	case GOAL_MINIMIZE:
		sb.append( ascii( "min" ) );
		break;
	default:
		assert( eGoal == GOAL_UNKNOWN );
		sb.append( ascii( "unknown" ) );
	}
	sb.append( Global::STRING_SEPARATOR );
	sb.append( getTextByWidget( this, ascii( "editVarCells" ) ) );
	
	typedef std::vector< ConstraintString > ConStr;
	ConStr cnConStr = getAllConstraints();
	ConStr::iterator itEnd = cnConStr.end();
	for ( ConStr::iterator it = cnConStr.begin(); it != itEnd; ++it )
	{
		sb.append( Global::STRING_SEPARATOR );
		sb.append( it->Left );
		sb.append( Global::STRING_SEPARATOR );
		switch ( it->Equal )
		{
		case numeric::opres::EQUAL:
			sb.append( ascii( "=" ) );
			break;
		case numeric::opres::GREATER_THAN_EQUAL:
			sb.append( ascii( ">=" ) );
			break;
		case numeric::opres::LESS_THAN_EQUAL:
			sb.append( ascii( "<=" ) );
			break;
		default:
			assert( !"unknown equality sign" );
		}
		sb.append( Global::STRING_SEPARATOR );
		sb.append( it->Right );
	}

	OUString sStr = sb.makeStringAndClear();
	CalcInterface* pCalc = getSolverImpl()->getCalcInterface();
	uno::Any aVal;
	aVal <<= sStr;
	pCalc->setDocumentProperty( Global::MODEL_METADATA_NAME, aVal );
}

void SolverDialog::loadModelFromDocument()
{
	using rtl::OUString;
	typedef std::vector< rtl::OUString > ContStr;

	CalcInterface* pCalc = getSolverImpl()->getCalcInterface();
	uno::Any aVal = pCalc->getDocumentProperty( Global::MODEL_METADATA_NAME );
	OUString sVal;
	aVal >>= sVal;
	OUString sSep = Global::STRING_SEPARATOR;
	ContStr cnVal;
	split( sVal, sSep, cnVal );
	clearConstraints();

	ContStr::iterator it, itBeg = cnVal.begin(), itEnd = cnVal.end();
	for ( it = itBeg; it != itEnd; ++it )
	{
		sal_Int32 nIdx = distance( itBeg, it );
		if ( nIdx == 0 )
			setTextByWidget( this, ascii( "editTargetCell" ), *it );
		else if ( nIdx == 1 )
		{
			OUString sItem = *it;
			if ( sItem.equals( ascii( "min" ) ) )
				setGoal( GOAL_MINIMIZE );
			else if ( sItem.equals( ascii( "max" ) ) )
				setGoal( GOAL_MAXIMIZE );
		}
		else if ( nIdx == 2 )
			setTextByWidget( this, ascii( "editVarCells" ), *it );
		else if ( nIdx > 2 )
		{
			OUString sLeft = *it++;
			assert( it != itEnd );
			OUString sEqual = *it++;
			assert( it != itEnd );
			OUString sRight = *it;
			numeric::opres::Equality eEq;
			if ( sEqual.equals( ascii( ">=" ) ) )
				eEq = GREATER_THAN_EQUAL;
			else if ( sEqual.equals( ascii( "<=" ) ) )
				eEq = LESS_THAN_EQUAL;
			else if ( sEqual.equals( ascii( "=" ) ) )
				eEq = EQUAL;
			else
				assert( !"unknown equality value" );
			setConstraint( sLeft, sRight, eEq );
		}
	}
}

//---------------------------------------------------------------------------

}
