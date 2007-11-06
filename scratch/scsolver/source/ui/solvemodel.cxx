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

 
#include "solvemodel.hxx"
#include "solver.hxx"
#include "tool/global.hxx"
#include "unoglobal.hxx"
#include "lpbuilder.hxx"
#include "nlpbuilder.hxx"
#include "dialog.hxx"
#include "xcalc.hxx"
#include "option.hxx"
#include "numeric/lpmodel.hxx"
#include "numeric/nlpmodel.hxx"
#include "numeric/matrix.hxx"
#include "numeric/type.hxx"
#include "numeric/lpbase.hxx"
#include "numeric/exception.hxx"
//#include "numeric/lpsimplex.hxx"
#include "numeric/lpsolve.hxx"
#include "numeric/nlpnewton.hxx"
#include "numeric/cellfuncobj.hxx"
#ifdef ENABLE_SCSOLVER_UNO_ALGORITHM
#include "numeric/lpuno.hxx"
#endif

#include <memory>
#include <exception>
#include <vector>
#include <map>

#include "scsolver.hrc"

using namespace std;
using namespace scsolver::numeric;
using scsolver::numeric::Matrix;
using com::sun::star::table::CellAddress;

namespace scsolver {

typedef struct {
	short sheet;
	long  column;
	long  row;
} cell_address_t;

/**
 * Store the constant term by cell address.  The constant term
 * in a given cell is the value remaining even after the values
 * of all decision variables are set to zero.
 */
class ConstantTermStorage
{
public:
	ConstantTermStorage() {}
	~ConstantTermStorage() throw() {}

	void setValue(const CellAddress& addr, double value)
	{
		size_t key = 0;
		bool isNewKey = false;
		getKeyFromAddress(addr, key, isNewKey);
		if (isNewKey)
			m_List.insert( map<size_t,double>::value_type(key, value) );
#ifdef SCSOLVER_DEBUG
		else
		{
			// Make sure the value is identical.
			if (m_List[key] != value)
				throw AssertionWrong();
		}
#endif
	}

	double getValue(const CellAddress& addr)
	{
		size_t key = 0;
		bool isNewKey = false;
		getKeyFromAddress(addr, key, isNewKey);
		if (isNewKey)
		{
			// This should never happen, because we are just querying the 
			// existing value associated with a specified address.
			throw ConstraintError();
		}
		return m_List[key];
	}

private:
	map<size_t, double> m_List;
	vector<cell_address_t> m_AddrKeys;

	/**
	 * Given a set of sheet, column and row IDs, return a single
	 * number that is unique to that number set.  In other words,
	 * this method always returns identical number when the same
	 * cell address is given.
	 * 
	 * @param addr cell address
	 * @param key (reference) unique number for the cell address
	 * @param isNewKey (reference) true is the cell address is new
	 */
	void getKeyFromAddress(const CellAddress& addr, size_t& key, bool& isNewKey)
	{
		cell_address_t cell;
		cell.sheet = addr.Sheet;
		cell.column = addr.Column;
		cell.row = addr.Row;
		vector<cell_address_t>::iterator it, 
			itBeg = m_AddrKeys.begin(), itEnd = m_AddrKeys.end();

		for (it = itBeg; it != itEnd; ++it)
		{
			if (cell.sheet == it->sheet && cell.column == it->column && cell.row == it->row)
			{
				// Match found.
				key = distance(itBeg, it);
				isNewKey = false;
				return;
			}
		}

		// Match not found.
		m_AddrKeys.push_back(cell);
		key = m_AddrKeys.size() - 1;
		isNewKey = true;
		return;
	}
};

/**
 * Used only to safely switch off cell updates.  Declare a local
 * variable of this class type at the beginning of a block where
 * the cell updates should be disabled.  It automatically
 * re-enables cell updates when the variable goes out of scope.
 */
class CellUpdateSwitch
{
public:
	CellUpdateSwitch(CalcInterface* p) :
		m_pCalc(p)
	{
		p->disableCellUpdates();
	}

	~CellUpdateSwitch() throw()
	{
		m_pCalc->enableCellUpdates();
	}

private:
	CellUpdateSwitch();              // disabled
	void* operator new(size_t size); // disabled

	CalcInterface* m_pCalc;
};

class SolveModelImpl
{
public:
	SolveModelImpl( SolverImpl* p ) :
		m_pSolverImpl(p),
		m_bSolved(false)
	{
	}

	~SolveModelImpl() throw()
	{
	}

	/**
	 * This is the gateway method that calls either solveLp() or
	 * solveNlp() as appropriate based on the corresponding option
	 * setting.
	 */
	void solve()
	{
		OptModelType type = getSolverImpl()->getOptionData()->getModelType();
		switch (type)
		{
		case OPTMODELTYPE_LP:
		case OPTMODELTYPE_MILP:
			solveLp();
			break;
		case OPTMODELTYPE_NLP:
		case OPTMODELTYPE_MINLP:
			solveNlp();
			break;
		default:
			break;
		}
	}

	/**
	 * This method takes model parameters from the dialog,
	 * constructs an internal representation of an LP model, chooses
	 * an algorithm, solves it, and in case a feasible solution is
	 * found it puts the solution back into the cells.
	 */
	void solveLp()
	{
		using namespace numeric;

		SolverDialog* pMainDlg = getSolverImpl()->getMainDialog();
		Goal eGoal = pMainDlg->getGoal();
		if ( eGoal == GOAL_UNKNOWN )
		{
			pMainDlg->showMessage( 
				pMainDlg->getResStr(SCSOLVER_STR_MSG_GOAL_NOT_SET) );
			return;
		}

		auto_ptr<LpModelBuilder> pBuilder( new LpModelBuilder );
		pBuilder->setGoal( eGoal );

		CellAddress addr = resolveObjectiveFuncAddress();
		pBuilder->setObjectiveFormulaAddress(addr);

		pBuilder->clearDecisionVarAddresses();
		vector<CellAddress> addrs = resolveDecisionVarAddress();
		vector<CellAddress>::iterator it,
			itBeg = addrs.begin(), itEnd = addrs.end();
		for ( it = itBeg; it != itEnd; ++it )
			pBuilder->setDecisionVarAddress(*it);

		resolveConstraintAddress( pBuilder.get() );
		parseConstraints( pBuilder.get() );

		lp::Model aModel = pBuilder->getModel();

		OptionData* pOption = getSolverImpl()->getOptionData();
		aModel.setVarPositive( pOption->getVarPositive() );
		aModel.setVarInteger( pOption->getVarInteger() );

#if SCSOLVER_DEBUG
        aModel.print(); // prints model to stdout
#endif
		aModel.setPrecision( 2 );
		auto_ptr<lp::BaseAlgorithm> algorithm = getLpAlgorithm();

		aModel.setVerbose(true);
		m_bSolved = false;
		try
		{
			algorithm->setModel( &aModel );
			algorithm->solve();
			m_bSolved = true;
			m_mxSolution = algorithm->getSolution();
			updateCells( pBuilder.get() );
			pMainDlg->showSolutionFound();
		}
		catch( const ModelInfeasible& e )
		{
			Debug( "model infeasible" );
			pMainDlg->showSolutionInfeasible();
		}
		catch( const scsolver::RuntimeError& e )
		{
			// This error message is localizable.
			pMainDlg->showMessage( e.getMessage() );
		}
		catch( const ::std::exception& e )
		{
			// standard exception.  should rarely happen.
			pMainDlg->showMessage(
				pMainDlg->getResStr(SCSOLVER_STR_MSG_STD_EXCEPTION_CAUGHT) );
		}
	}

	/**
	 * Solve non-linear model.  Still work in progress.
	 */
	void solveNlp()
	{
		using namespace numeric;

		SolverDialog* pMainDlg = getSolverImpl()->getMainDialog();
		Goal eGoal = pMainDlg->getGoal();
		if ( eGoal == GOAL_UNKNOWN )
		{
			pMainDlg->showMessage( ascii_i18n("Goal is not set") );
			return;
		}

        auto_ptr<numeric::CellFuncObj> pFuncObj(
            new numeric::CellFuncObj(m_pSolverImpl->getCalcInterface()) );
        CellAddress addr = resolveObjectiveFuncAddress();

        // Build the model by using the builder.
        NlpModelBuilder builder(m_pSolverImpl);
        builder.setFuncObj(pFuncObj.get());
        builder.setObjectiveFormulaAddress(addr);
        builder.clearDecVarAddresses();
        vector<CellAddress> addrs = resolveDecisionVarAddress();
        vector<CellAddress>::iterator it, itBeg = addrs.begin(), itEnd = addrs.end();
        for ( it = itBeg; it != itEnd; ++it )
            builder.appendDecVarAddress(*it);

        nlp::Model model = builder.getModel();
        model.setGoal(eGoal);
        model.print();

		auto_ptr<nlp::BaseAlgorithm> algorithm = getNlpAlgorithm();
		m_bSolved = false;
		try
		{
			algorithm->setModel(&model);
			algorithm->solve();
			m_bSolved = true;
			pMainDlg->showSolutionFound();
		}
		catch ( const IterationTimedOut& )
		{
			pMainDlg->showMessage(
				getSolverImpl()->getResStr(SCSOLVER_STR_MSG_ITERATION_TIMED_OUT) );
		}
		catch ( const MaxIterationReached& )
		{
			pMainDlg->showMessage(
				getSolverImpl()->getResStr(SCSOLVER_STR_MSG_MAX_ITERATION_REACHED) );
		}
		catch ( const RuntimeError& e )
		{
			pMainDlg->showMessage( e.getMessage() );
		}
		catch ( const ::std::exception& )
		{
			pMainDlg->showMessage(
				getSolverImpl()->getResStr(SCSOLVER_STR_MSG_STD_EXCEPTION_CAUGHT) );
		}
	}

	bool isSolved() const
	{
		return m_bSolved;
	}

private:
	SolverImpl* m_pSolverImpl;
	bool m_bSolved;
	Matrix m_mxSolution;
	
	SolverImpl* getSolverImpl() const
	{ 
		return m_pSolverImpl; 
	}

	auto_ptr<lp::BaseAlgorithm> getLpAlgorithm() const;

	auto_ptr<nlp::BaseAlgorithm> getNlpAlgorithm() const
	{
		auto_ptr<nlp::BaseAlgorithm> p( new nlp::QuasiNewton );
		return p;
	}

	/**
	 * Transform a LP model given in the cells into the standard
	 * format.  This is done by setting the value of one of the
	 * decision variable cells to 1 and all the others to 0, and
	 * interpret the values of the objective function and constraint
	 * cells, and repeat it for every decision variable cell.
	 */
	void parseConstraints( LpModelBuilder* pBuilder )
	{
		CellUpdateSwitch aCellUpdate( m_pSolverImpl->getCalcInterface() );

		// Create a cost vector from the decision variables.

		vector<CellAddress> aAddrs = pBuilder->getAllDecisionVarAddresses();
		vector<double> aOrigVal;
		vector<CellAddress>::iterator pos;
		vector<CellAddress>::iterator aAddrsBegin = aAddrs.begin(), aAddrsEnd = aAddrs.end();
		CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
		for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
		{
			// Run through the decision variable cells, store their original formulas,
			// and set "=0" to each of these cells.
			table::CellAddress aAddr = *pos;
			rtl::OUString sFormula = pCalc->getCellFormula( aAddr );
			pBuilder->setTempCellFormula( aAddr, sFormula );
			pCalc->setCellFormula( aAddr, ascii("=0") );
		}

		// At this point, all decision variables are set to zero.  Go
		// throuth all constraint cells, and store their current value
		// as its constant term for later use.
		vector<ConstraintAddress> aConstAddrs = pBuilder->getAllConstraintAddresses();
		vector<ConstraintAddress>::iterator posCA, posCAEnd = aConstAddrs.end();
		ConstantTermStorage cnConstTerm;
		for (posCA = aConstAddrs.begin(); posCA != posCAEnd; ++posCA)
		{
			if ( !posCA->isLeftCellNumeric() )
			{
				double fValL = pCalc->getCellValue( posCA->getLeftCellAddr() );
				cnConstTerm.setValue(posCA->getLeftCellAddr(), fValL);
			}
			if ( !posCA->isRightCellNumeric() )
			{
				double fValR = pCalc->getCellValue( posCA->getRightCellAddr() );
				cnConstTerm.setValue(posCA->getRightCellAddr(), fValR);
			}
		}		

		// Set "=1" to each of the decision variable cells one at a time, and 
		// interpret the formula result.  Also set the constraint matrix size
		// so that it won't get resized at later time (resize is expensive).

		CellAddress aObjAddr = pBuilder->getObjectiveFormulaAddress();
		
		pBuilder->setConstraintMatrixSize( aConstAddrs.size(), aAddrs.size() );

		for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
		{
			table::CellAddress aAddr = *pos;
			pCalc->setCellFormula( aAddr, ascii("=1") );

			double fVal = pCalc->getCellValue( aObjAddr );
			pBuilder->setCostVector( aAddr, fVal );

			// Go through the constraints to construct constraint matrix
			// as well as the RHS vector.

			for ( posCA = aConstAddrs.begin(); posCA != posCAEnd; ++posCA )
			{
				double fValL = 0.0, fConstValL = 0.0;
				if ( posCA->isLeftCellNumeric() )
					fConstValL = posCA->getLeftCellValue();
				else
				{
					fValL = pCalc->getCellValue( posCA->getLeftCellAddr() );
					// Subtract the LHS constant term from the LHS value.
					fConstValL = cnConstTerm.getValue( posCA->getLeftCellAddr() );
					fValL -= fConstValL;
				}

				double fValR = 0.0, fConstValR = 0.0;
				if ( posCA->isRightCellNumeric() )
					fConstValR = posCA->getRightCellValue();
				else
				{
					fValR = pCalc->getCellValue( posCA->getRightCellAddr() );
					fConstValR = cnConstTerm.getValue( posCA->getRightCellAddr() );
					fValL -= fValR - fConstValR;
				}
				pBuilder->setConstraintCoefficient( aAddr, *posCA, fValL, fConstValR-fConstValL );
			}		

			pCalc->setCellFormula( aAddr, ascii("=0") );
		}

		// Restore the original formulas to the decision variable cells.
		for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
		{
			table::CellAddress aAddr = *pos;
			rtl::OUString sOrigFormula = pBuilder->getTempCellFormula( aAddr );
			pCalc->setCellFormula( aAddr, sOrigFormula );
		}

#if SCSOLVER_DEBUG
		// Check to ensure the model has the right cost vector.
		for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
		{
			table::CellAddress aAddr = *pos;
			double f = pBuilder->getCostVector( aAddr );
			cout << aAddr.Column << ", " << aAddr.Row << " = " << f << endl;
			cout.flush();
		}
#endif
	}

	void resolveConstraintAddress( LpModelBuilder* pBuilder );

	/**
	 * Takes the string form of an objective function cell address
	 * (e.g. $'Sheet1'.$A$2), converts it into a 3D address.
	 * 
	 * @return cell address object
	 */
	CellAddress resolveObjectiveFuncAddress()
	{
		rtl::OUString sTargetCellAddr = m_pSolverImpl->getMainDialog()->getTargetCellAddress();
		if ( !sTargetCellAddr.getLength() )
			throw RuntimeError( ascii_i18n("Target cell address empty") );

		CellAddress aAddr = m_pSolverImpl->getCalcInterface()->getCellAddress(
			sTargetCellAddr );

		return aAddr;
	}

	/**
	 * Convert a 3D cell reference (e.g. $'Sheet Name'.$B$5:$C$7)
	 * into an array of individual cell addresses. These addresses
	 * represent a series of decision variables in sequential order.
	 * 
	 * @return an array of cell addresses
	 */
	vector<CellAddress> resolveDecisionVarAddress()
	{
		using com::sun::star::table::CellRangeAddress;
	
		rtl::OUString sAddr = m_pSolverImpl->getMainDialog()->getVarCellAddress();
		if ( !sAddr.getLength() )
			throw RuntimeError( ascii_i18n("Decision variable cells empty") );

		CellRangeAddress aRangeAddr = m_pSolverImpl->getCalcInterface()->getCellRangeAddress(sAddr);
		
		// Convert address range into an array of individual cell coordinates.
		sal_Int16 nSheetId = aRangeAddr.Sheet;
		sal_Int32 nSCol = aRangeAddr.StartColumn, nSRow = aRangeAddr.StartRow;
		sal_Int32 nECol = aRangeAddr.EndColumn, nERow = aRangeAddr.EndRow;
		
		vector<CellAddress> cn;
		cn.reserve( (nECol-nSCol)*(nERow-nSRow) );
		for ( sal_Int32 nCol = nSCol; nCol <= nECol; ++nCol )
		{
			for ( sal_Int32 nRow = nSRow; nRow <= nERow; ++nRow )
			{
				CellAddress addr;
				addr.Sheet  = nSheetId;
				addr.Column = nCol;
				addr.Row    = nRow;
				cn.push_back(addr);
			}
		}
		return cn;
	}

	void updateCells( LpModelBuilder* pBuilder );
};

/**
 * This method returns an algorithm object to use to solve a
 * given LP model.  While all this method does currently is
 * simply return the pre-selected algorithm, in future this
 * method may be used to pick a user-selected algorithm based on
 * his/her settings.
 * 
 * @return auto_ptr<lp::BaseAlgorithm>
 */
auto_ptr<lp::BaseAlgorithm> SolveModelImpl::getLpAlgorithm() const
{
#ifdef ENABLE_SCSOLVER_UNO_ALGORITHM
	auto_ptr<lp::BaseAlgorithm> algorithm( new lp::UnoAlgorithm(
		ascii("org.openoffice.sc.solver.LpSolve"), getSolverImpl()->getCalcInterface() ) );
#else
	//auto_ptr<lp::BaseAlgorithm> algorithm( new lp::RevisedSimplex );
	auto_ptr<lp::BaseAlgorithm> algorithm( new lp::LpSolve );
#endif

	return algorithm;
}

static bool lcl_isNumeric( const rtl::OUString& sVal )
{
	double fVal = sVal.toDouble();
	if ( fVal == 0.0 )
	{
		if ( sVal.indexOf( ascii("$") ) == 0 )
			return false;
		else
			return true;
	}
	else
		return true;
}

/**
 * Get all the constraint strings (both the LHS and RHS strings)
 * from the main dialog, resolve their addresses into sheet,
 * column, and row IDs, and give them to the builder.
 */
void SolveModelImpl::resolveConstraintAddress( LpModelBuilder* pBuilder )
{
	vector< ConstraintString > aConstraint = m_pSolverImpl->getMainDialog()->getAllConstraints();
	vector< ConstraintString >::const_iterator pos = aConstraint.begin(), posEnd = aConstraint.end();
	CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
	pBuilder->clearConstraintAddresses();

	for ( ; pos != posEnd; ++pos)
	{
		ConstraintString aConstStr = *pos;
		bool bLHSNumeric = lcl_isNumeric(aConstStr.Left);
		bool bRHSNumeric = lcl_isNumeric(aConstStr.Right);

		if (bLHSNumeric && bRHSNumeric)
		{
			Debug("Both LHS and RHS values are numeric.  Skipping...");
			continue;
		}

		// Left hand side cell address
		table::CellRangeAddress aRangeAddrL = pCalc->getCellRangeAddress( aConstStr.Left );
		sal_Int16 nLSheet = aRangeAddrL.Sheet;
		sal_Int32 nLColS  = aRangeAddrL.StartColumn;
		sal_Int32 nLColE  = aRangeAddrL.EndColumn;
		sal_Int32 nLRowS  = aRangeAddrL.StartRow;
		sal_Int32 nLRowE  = aRangeAddrL.EndRow;

		// Right hand side cell address
		table::CellRangeAddress aRangeAddrR = pCalc->getCellRangeAddress( aConstStr.Right );
		sal_Int16 nRSheet = aRangeAddrR.Sheet;
		sal_Int32 nRColS  = aRangeAddrR.StartColumn;
		sal_Int32 nRColE  = aRangeAddrR.EndColumn;
		sal_Int32 nRRowS  = aRangeAddrR.StartRow;
		sal_Int32 nRRowE  = aRangeAddrR.EndRow;

		if (bLHSNumeric)
		{
			// Only the right hand side is cell reference.
			for ( sal_Int32 i = 0; i <= nRColE - nRColS; ++i )
			{
				for ( sal_Int32 j = 0; j <= nRRowE - nRRowS; ++j )
				{
					ConstraintAddress aConstAddr;
					table::CellAddress aAddrR;
					aAddrR.Sheet  = nRSheet;
					aAddrR.Row    = nRRowS + j;
					aAddrR.Column = nRColS + i;
					aConstAddr.setRightCellAddr( aAddrR );
					aConstAddr.setEquality( aConstStr.Equal );
					aConstAddr.setLeftCellValue( aConstStr.Left.toDouble() );

					pBuilder->setConstraintAddress( aConstAddr );
				}
			}
		}
		else if (bRHSNumeric)
		{
			// Only the left hand side is cell reference.
			for ( sal_Int32 i = 0; i <= nLColE - nLColS; ++i )
			{
				for ( sal_Int32 j = 0; j <= nLRowE - nLRowS; ++j )
				{
					ConstraintAddress aConstAddr;
					table::CellAddress aAddrL;
					aAddrL.Sheet  = nLSheet;
					aAddrL.Row    = nLRowS + j;
					aAddrL.Column = nLColS + i;
					aConstAddr.setLeftCellAddr( aAddrL );
					aConstAddr.setEquality( aConstStr.Equal );
					aConstAddr.setRightCellValue( aConstStr.Right.toDouble() );

					pBuilder->setConstraintAddress( aConstAddr );
				}
			}
		}
		else
		{
			// Both sides are cell reference.
			if ( nLColE - nLColS != nRColE - nRColS || nLRowE - nLRowS != nRRowE - nRRowS )
				throw RuntimeError(
					getSolverImpl()->getResStr(SCSOLVER_STR_MSG_CELL_GEOMETRIES_DIFFER) ); // This should not happen !

			for ( sal_Int32 i = 0; i <= nLColE - nLColS; ++i )
			{
				for ( sal_Int32 j = 0; j <= nLRowE - nLRowS; ++j )
				{
					ConstraintAddress aConstAddr;
					table::CellAddress aAddrL, aAddrR;
					aAddrL.Sheet  = nLSheet;
					aAddrL.Row    = nLRowS + j;
					aAddrL.Column = nLColS + i;
					aConstAddr.setLeftCellAddr( aAddrL );
					aConstAddr.setEquality( aConstStr.Equal );

					aAddrR.Sheet  = nRSheet;
					aAddrR.Row    = nRRowS + j;
					aAddrR.Column = nRColS + i;
					aConstAddr.setRightCellAddr( aAddrR );

					pBuilder->setConstraintAddress( aConstAddr );
				}
			}
		}
	}
}

void SolveModelImpl::updateCells( LpModelBuilder* pBuilder )
{
	vector<CellAddress> cnAddrs = pBuilder->getAllDecisionVarAddresses();
	CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
	OSL_ASSERT( m_mxSolution.rows() == cnAddrs.size() );
	vector<CellAddress>::iterator it, itEnd = cnAddrs.end();
	size_t nIdx = 0;
	for ( it = cnAddrs.begin(); it != itEnd; ++it )
		pCalc->setCellValue( *it, m_mxSolution( nIdx++, 0 ) );
}

//---------------------------------------------------------------------------
// SolveModel

SolveModel::SolveModel( SolverImpl* p ) :
		m_pImpl( new SolveModelImpl( p ) )
{
}

SolveModel::~SolveModel() throw()
{
}

void SolveModel::solve()
{
	m_pImpl->solve();
}

bool SolveModel::isSolved() const
{
	return m_pImpl->isSolved();
}


}

		



