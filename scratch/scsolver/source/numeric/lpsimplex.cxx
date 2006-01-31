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

#include <osl/diagnose.h>

#include <numeric/lpsimplex.hxx>
#include <numeric/lpmodel.hxx>
#include <global.hxx>

#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <boost/numeric/ublas/matrix.hpp>

using namespace std;
	
namespace scsolver { namespace numeric { namespace opres { namespace lp {

typedef vector<size_t>				uInt32Container;
typedef uInt32Container::iterator		uInt32Iter;
typedef uInt32Container::const_iterator	uInt32CIter;

typedef vector<Bound>					BoundContainer;
typedef BoundContainer::iterator		BoundIter;
typedef BoundContainer::const_iterator	BoundCIter;

//---------------------------------------------------------------------------
// RevisedSimplexImpl

class RevisedSimplexImpl
{
public:
	RevisedSimplexImpl( RevisedSimplex* );
	~RevisedSimplexImpl();

	Matrix solve();

private:
	RevisedSimplex* m_pSelf;

	Matrix m_aBasicInv;
	Matrix m_aUpdateMatrix;
	Matrix m_aPriceVector;
	Matrix m_aX;
	size_t m_nIter;

	Matrix m_A, m_B, m_C;
	std::vector<bool> m_aBasicVar;
	std::vector<size_t> m_aBasicVarId;

	//-----------------------------------------------------------------------
	// Private methods

	Model* getModel() const { return m_pSelf->getModel(); }

	void runNormalInitSearch();
	void runTwoPhaseInitSearch( const std::vector<size_t>& );
	void printIterateHeader() const;
	bool iterate();

	Matrix solvePriceVector( std::vector<size_t>, const Matrix&, const Matrix& );
	void getLambda( const Matrix&, const Matrix&, double&, size_t& ) const;
};


RevisedSimplexImpl::RevisedSimplexImpl( RevisedSimplex* p ) : m_pSelf( p ),
	m_aBasicInv( 0, 0 ), m_aUpdateMatrix( 0, 0 ), m_aPriceVector( 0, 0 ),
	m_aX( 0, 0 ), m_nIter( 0 )
{
}

RevisedSimplexImpl::~RevisedSimplexImpl()
{
}

Matrix RevisedSimplexImpl::solve()
{
	//-------------------------------------------------------------------------
	// Initialize
	
	//-------------------------------------------------------------------------
	// Normalize the objective with use of slack variable(s), and store in the 
	// following variables such that:
	//
	//     A = expanded constraint matrix
	//     B = right hand side vector
	//     C = expanded cost vector
	
	Model* pModel = getModel();
	Matrix A( pModel->getConstraintMatrix() );
	Matrix B( pModel->getRhsVector() );
	Matrix C( pModel->getCostVector() );

	for ( size_t i = 0; i < A.rows(); ++i )
	{
		Equality eEq = pModel->getEqualityByRowId( i );
		switch( eEq )
		{
		case LESS_THAN_EQUAL:
			A( i, A.cols() ) = 1;
			break;
		case GREATER_THAN_EQUAL:
			A( i, A.cols() ) = -1;
			break;
		default:
			OSL_ASSERT( eEq == EQUAL );
		}
		if ( eEq != EQUAL )
			C( 0, C.cols() ) = 0;
	}

	if ( !pModel->getVarPositive() )
	{
		if ( pModel->getVerbose() )
			cout << "non-positive variables are not yet supported" << endl;
		throw ModelInfeasible();
	}

	// Search for an initial solution.
	vector<size_t> aSatRows;    // collection of satisfied rows
	vector<size_t> aNonSatRows; // collection of non-satisfied rows
	for ( size_t i = 0; i < A.rows(); ++i )
		aNonSatRows.push_back( i );

	// Determine initial basic variables.
	vector<bool> aBasicVar;	
	for ( size_t j = 0; j < A.cols(); ++j )
	{
		bool bNonZeroFound = false;
		long nUniqueRowSigned = -1;
		aBasicVar.push_back( false );
		
		// Check all the coefficients in that column and store the row ID of
		// a non-zero coefficient if and only if that non-zero coefficient is
		// the only non-zero coefficient in that particular column.
		
		for ( size_t i = 0; i < A.rows(); ++i )
			if ( A( i, j ) != 0.0 )
				if ( bNonZeroFound )
				{
					nUniqueRowSigned = -1;
					break;
				}
				else
				{
					bNonZeroFound = true;
					nUniqueRowSigned = i;
				}
		
		if ( nUniqueRowSigned >= 0 )
		{
			// Such non-zero coefficient exists (see above).

			size_t nUniqueRow = nUniqueRowSigned;
			double fVal = A( nUniqueRow, j );
			double fRHS = B( nUniqueRow, 0 );
			
			if ( ( fVal >= 0.0 && fRHS >= 0.0 ) || ( fVal < 0.0 && fRHS < 0.0 ) )
			{
				// The signs match
				bool bDupMatch = false;
				vector<size_t>::iterator pos;				
				pos = find( aSatRows.begin(), aSatRows.end(), nUniqueRow );
				if ( pos == aSatRows.end() )
					aSatRows.push_back( nUniqueRow );
				else
					bDupMatch = true;
				
				pos = remove( aNonSatRows.begin(), aNonSatRows.end(), nUniqueRow );
				if ( pos != aNonSatRows.end() )
					aNonSatRows.erase( pos, aNonSatRows.end() );
				if ( !bDupMatch )
					aBasicVar[j] = true;
			}
		}
	}

	if ( pModel->getVerbose() )
	{
		cout << "A:" << endl;
		A.print( 0 );
		cout << "B:" << endl;
		B.print( 0 );
		cout << "C:" << endl;
		C.print( 0 );
		
		cout << "aSatRows: ";
		printElements( aSatRows, " " );
		cout << "\t" << "aNonSatRows: ";
		printElements( aNonSatRows, " " );
		cout << "\t" << "aBasicVar: ";
		printElements( aBasicVar, " " );
		cout << endl;
	}
	
	m_aBasicVar = aBasicVar;

	m_A = A;
	m_B = B;
	m_C = C;

	if ( aNonSatRows.size() > 0 )
		runTwoPhaseInitSearch( aNonSatRows );
	else
		runNormalInitSearch();
	
	// Store all necessary variables before iterations start

	m_aUpdateMatrix.clear();
	m_aBasicVarId.clear();
	vector<bool>::iterator pos;
	for ( pos = m_aBasicVar.begin(); pos != m_aBasicVar.end(); ++pos )
		if ( *pos )
			m_aBasicVarId.push_back( distance( m_aBasicVar.begin(), pos ) );

	m_aPriceVector = solvePriceVector( m_aBasicVarId, m_aBasicInv, C );

	// Start iterations
	m_nIter = 0;
	while ( !iterate() );

	Matrix aCost = pModel->getCostVector();
	Matrix solution( aCost.cols(), 1 );
	solution.setResizable( false );
	for ( size_t i = 0; i < aCost.cols(); ++i )
		solution( i, 0 ) = m_aX( i, 0 );
	
	if ( pModel->getVerbose() )
	{
		cout << "x = ";
		solution.trans().print();	
	}
	
	return solution;
}

/** Find an initial X via normal (non two-phase) search, and set the following 
	member variables:
		m_aBasicInv : an invert basic matrix
		m_aX		: an initial X
*/
void RevisedSimplexImpl::runNormalInitSearch()
{
	// Two phase method NOT necessary
	Model* pModel = getModel();
	if ( pModel->getVerbose() )
		Debug( "two phase method NOT necessary" );
	
	Matrix ABasic( m_A );
	vector<size_t> aNonBasicVarId;
	vector<bool>::iterator pos;
	for ( pos = m_aBasicVar.begin(); pos != m_aBasicVar.end(); ++pos )
		if ( !*pos )
			aNonBasicVarId.push_back( distance( m_aBasicVar.begin(), pos ) );
	
	if ( pModel->getVerbose() )
	{
		cout << "aNonBasicVarId: ";
		printElements( aNonBasicVarId );
		cout << endl;
	}
		
	ABasic.deleteColumns( aNonBasicVarId );
	Matrix aBasicInv( ABasic.inverse() );
	Matrix XBasic = aBasicInv * m_B;
		
	size_t nRow = 0;
	Matrix X( 0, 0 );
	for ( size_t j = 0; j < m_A.cols(); ++j )
	{
		// Remember that the X is a single-column matrix
		if ( m_aBasicVar.at(j) )
			X( j, 0 ) = XBasic( nRow++, 0 );
		else
			X( j, 0 ) = 0.0;
	}
	
	// Update member variables.
	
	m_aX = X;
	m_aBasicInv = aBasicInv;
}

/** Find an initial X via two-phase search, and set the following member
	variables:
		m_aBasicVar : bool vector holding whether each variable is basic
		m_aBasicInv : an invert basic matrix
		m_aX		: an initial X
*/
void RevisedSimplexImpl::runTwoPhaseInitSearch( const vector<size_t>& aNonSatRows )
{
	static const bool bDebugTwoPhase = true;

	Model* pModel = getModel();
	if ( pModel->getVerbose() )
		Debug( "Entering a two-phase search for initial solution" );

	Matrix A2( m_A );
	
	cout << "initial number of column(s): " << A2.cols() << endl;
	vector<size_t>::const_iterator nIter, end = aNonSatRows.end();
	for ( nIter = aNonSatRows.begin(); nIter != end; ++nIter )
	{
		size_t nRowId = *nIter;
		if ( m_B( nRowId, 1 ) >= 0 )
			A2( nRowId, A2.cols() ) = 1;
		else
			A2( nRowId, A2.cols() ) = -1;
	}
	
	cout << A2.cols() - m_A.cols() << " column(s) added" << endl;

	auto_ptr<Model> model( new Model );
	model->setGoal( GOAL_MINIMIZE );
	model->setStandardConstraintMatrix( A2, m_B );
	model->setVarPositive( true );
	for ( size_t i = 0; i < A2.cols(); ++i )
		if ( i < m_A.cols() )
			model->setCostVectorElement( i, 0 );
		else
			model->setCostVectorElement( i, 1 );

	if ( bDebugTwoPhase )
		model->print();

	model->setVerbose( bDebugTwoPhase );
	auto_ptr<BaseAlgorithm> algorithm( new RevisedSimplex );
	model->solve( algorithm );
	Matrix Xtmp( model->getSolution() );

	size_t nANumRow = m_A.rows(), nANumCol = m_A.cols(), nA2NumCol = A2.cols();
	for ( size_t i = nANumCol; i < nA2NumCol; ++i )
	{
		double f = Xtmp( i, 0 ); // Do I need to round this to precision ?
		cout << "f = " << f << endl;
		if ( f != 0.0 )
			throw ModelInfeasible();
	}

	Matrix X( 0, 0 );
	vector<size_t> aNonBasicVarId;
	vector<bool> aBasicVar( m_aBasicVar );

	size_t i = nANumCol;
	do
	{
		double f = Xtmp( --i, 0 );
		X( i, 0 ) = f;
		if ( aNonBasicVarId.size() < nANumCol - nANumRow && f == 0.0 )
		{
			aBasicVar[i] = false;
			aNonBasicVarId.push_back( i );
		}
		else
			aBasicVar[i] = true;
	}
	while ( i != 0 );

	sort( aNonBasicVarId.begin(), aNonBasicVarId.end() );
	cout << "aNonBasicVarId = ";
	printElements( aNonBasicVarId, " " );
	cout << aNonBasicVarId.size() << endl;

	Matrix ABasic( m_A );
	ABasic.print( 0 );

	cout << "(" << ABasic.rows() << "," << ABasic.cols() << ")" << endl;

	ABasic.deleteColumns( aNonBasicVarId );
	ABasic.print( 0 );

	cout << "(" << ABasic.rows() << "," << ABasic.cols() << ")" << endl;

	Matrix aBasicInv( ABasic.inverse() );
	cout << "inverse" << endl;
	aBasicInv.print();

	// Update member variables.
	
	m_aBasicVar = aBasicVar;
	m_aX = X;
	m_aBasicInv = aBasicInv;
	
	if ( pModel->getVerbose() )
		Debug( "Two-phase search found an initial solution" );
}

void RevisedSimplexImpl::printIterateHeader() const
{
	cout << endl;
	string line = repeatString( "-", 70 );

	cout << line << endl;
	cout << "Iteration " << m_nIter << endl;
	cout << line << endl;
	
	cout << "Basic Variable: ";
	printElements( m_aBasicVar );
	cout << endl;
	
	cout << "x(" << m_nIter << ") = ";
	m_aX.trans().print( getModel()->getPrecision() );
	cout << "cx = " << (m_C*m_aX).operator()( 0, 0 ) << endl;
	cout << line << endl;
	
	cout << "Basic Variable ID: ";
	printElements( m_aBasicVarId, " " );
	cout << endl << endl;;
	
	cout << "A^(-1)" << endl;
	m_aBasicInv.print();
	
	cout << endl << "v = ";
	m_aPriceVector.print();
}

bool RevisedSimplexImpl::iterate()
{
	bool bVerbose = getModel()->getVerbose();
	if ( bVerbose )
		printIterateHeader();

	// m_aBasicVar, m_aBasicVarId, m_aBasicInv, m_aPriceVector
	// m_aX, m_A, m_B, m_C

	// Determine IDs of non-basic variables
	vector<size_t> aNonBasicVarId;
	vector<bool>::iterator bIter;
	for ( bIter = m_aBasicVar.begin(); bIter != m_aBasicVar.end(); ++bIter )
		if ( !*bIter )
			aNonBasicVarId.push_back( distance( m_aBasicVar.begin(), bIter ) );

	// Determine entering non-basic variable	
	vector<size_t>::iterator nIter, nBegin = aNonBasicVarId.begin(), nEnd = aNonBasicVarId.end();
	map<size_t,double> fEnterBasicVars;
	double fMin, fMax;
	size_t nMin, nMax;
	for ( nIter = nBegin; nIter != nEnd; ++nIter )
	{
		size_t nId = *nIter;
		double fPrice = m_C( 0, nId ) - 
			( m_aPriceVector*m_A.getColumn( nId ) ).operator()( 0, 0 );

		if ( bVerbose )
			cout << "c(" << nId << ") = " << fPrice << endl;

		fEnterBasicVars.insert( make_pair( nId, fPrice ) );
		if ( nIter == nBegin )
		{
			fMin = fMax = fPrice;
			nMin = nMax = nId;
		}
		else
		{
			if ( fPrice > fMax )
			{
				fMax = fPrice;
				nMax = nId;
			}
			if ( fPrice < fMin )
			{
				fMin = fPrice;
				nMin = nId;
			}
		}			
	}
	
	Goal eGoal = getModel()->getGoal();
	if ( ( eGoal == GOAL_MAXIMIZE && fMax <= 0.0 ) ||
		 ( eGoal == GOAL_MINIMIZE && fMin >= 0.0 ) )
	{
		if ( bVerbose )
			cout << "Optimum solution reached" << endl;
		return true;
	}
	
	size_t nEnterVarId = 0;
	if ( eGoal == GOAL_MAXIMIZE )
		nEnterVarId = nMax;
	else if ( eGoal == GOAL_MINIMIZE )
		nEnterVarId = nMin;
	else
		throw;

	// Calculate dX (delta-X)
	Matrix dXBasic = m_aBasicInv * m_A.getColumn( nEnterVarId ) * (-1);
	Matrix dX( 0, 0 );
	
	OSL_ASSERT( dXBasic.rows() == m_aBasicVarId.size() );
	
	for ( size_t i = 0; i < dXBasic.rows(); ++i )
		dX( m_aBasicVarId.at( i ), 0 ) = dXBasic( i, 0 );
	
	nEnd = aNonBasicVarId.end();
	for ( nIter = aNonBasicVarId.begin(); nIter != nEnd; ++nIter )
	{
		size_t nId = *nIter;
		dX( nId, 0 ) = nId == nEnterVarId ? 1.0 : 0.0;
	}
	if ( bVerbose )
	{
		cout << "dX[" << nEnterVarId << "] = ";
		dX.trans().print( getModel()->getPrecision() );
	}

	// Check all components of dX to make sure there is at least one negative 
	// component.
	bool bNegativeFound = false;
	for ( size_t i = 0; i < dX.rows(); ++i )
		if ( dX( i, 0 ) < 0.0 )
		{
			bNegativeFound = true;
			break;
		}
		
	if ( !bNegativeFound )
	{
		if ( bVerbose )
			cout << "Unbounded model with no solution" << endl;
		throw ModelInfeasible();
	}

	// Calculate lambda value
	double fLambda;
	size_t nLeaveVarId;
	getLambda( m_aX, dX, fLambda, nLeaveVarId );
	
	if ( bVerbose )
		cout << "lambda = " << fLambda << "  x" << nEnterVarId << " enters and "
			 << "x" << nLeaveVarId << " leaves" << endl;

	m_aX += dX*fLambda;
	m_aBasicVar[nEnterVarId].flip();
	m_aBasicVar[nLeaveVarId].flip();
	size_t nIndexLeaveVar;
	for ( nIter = m_aBasicVarId.begin(); nIter < m_aBasicVarId.end(); ++nIter )
		if ( *nIter == nLeaveVarId )
		{
			nIndexLeaveVar = distance( m_aBasicVarId.begin(), nIter );
			*nIter = nEnterVarId;
			break;
		}

	Matrix E( m_aBasicInv.rows(), m_aBasicInv.cols(), true );
	double fLeaveVar = dX( nLeaveVarId, 0 );
	for ( size_t i = 0; i < E.rows(); ++i )
	{
		int nId = m_aBasicVarId.at( i );
		double fdXVal = dX( nId, 0 );
		if ( i == nIndexLeaveVar )
			E( i, i ) = 1.0/fLeaveVar*(-1.0);
		else
			E( i, nIndexLeaveVar ) = fdXVal / fLeaveVar * (-1.0);
	}
	
	// Update invert matrix and price vector.
	m_aBasicInv = E*m_aBasicInv;
	m_aPriceVector = solvePriceVector( m_aBasicVarId, m_aBasicInv, m_C );
	++m_nIter;
	
	return false;
}

Matrix RevisedSimplexImpl::solvePriceVector( std::vector<size_t> aBasicVarId,
		const Matrix& AInv, const Matrix& C )
{
	Matrix c;
	std::vector<size_t>::iterator pos;
	for ( pos = aBasicVarId.begin(); pos != aBasicVarId.end(); ++pos )
	{
		size_t i = distance( aBasicVarId.begin(), pos );
		double f = C( 0, aBasicVarId.at( i ) );
		c( 0, i ) = f;
	}
	return c*AInv;
}

void RevisedSimplexImpl::getLambda( const Matrix& X, const Matrix& dX, 
		double& rLambda, size_t& rLeaveVarId ) const
{
	OSL_ASSERT( X.rows() == dX.rows() );

	double fLambda = 0.0;
	size_t nId = 0;
	bool bFirst = true;
	for ( size_t i = 0; i < X.rows(); ++i )
	{
		double fXVal = X( i, 0 );
		double fdXVal = dX( i, 0 );
		if ( fdXVal < 0 )
		{
			double fVal = fXVal / fdXVal * (-1.0);
			if ( bFirst )
			{
				fLambda = fVal;
				nId = i;
				bFirst = false;
			}
			else
			{
				if ( fVal < fLambda )
				{
					fLambda = fVal;
					nId = i;
				}
			}
		}
	}
	
	rLambda = fLambda;
	rLeaveVarId = nId;
}


//---------------------------------------------------------------------------
// RevisedSimplex

RevisedSimplex::RevisedSimplex() : BaseAlgorithm(),
	m_pImpl( new RevisedSimplexImpl( this ) )
{
}

RevisedSimplex::~RevisedSimplex()
{
}

void RevisedSimplex::solve()
{
	Matrix sol = m_pImpl->solve();
	setSolution( sol );
}


//---------------------------------------------------------------------------
// BoundedRevisedSimplexImpl

struct VarBoundary
{
	size_t Id;
	double Value;
	Bound BoundType;
};

struct EnterBasicVar
{
	size_t Id;
	double Price;
	Bound BoundType;
};

class BoundedRevisedSimplexImpl
{
public:
	BoundedRevisedSimplexImpl( BoundedRevisedSimplex* );
	~BoundedRevisedSimplexImpl();
	
	Matrix solve();
	
private:
	BoundedRevisedSimplex* m_pSelf;
	
	Matrix m_mxBasicInv;
	Matrix m_mxUpdateMatrix;
	Matrix m_mxPriceVector;
	Matrix m_mxX;
	size_t m_nIter;

	Matrix m_mxA, m_mxB, m_mxC;
	std::vector<bool> m_aBasicVar;
	std::vector<size_t> m_aBasicVarId;
	std::vector<size_t> m_aNonBasicVarId;
	std::vector<size_t> m_aSkipBasicVarId;
	std::vector<Bound> m_aNonBasicVarBoundType;
	
	std::auto_ptr<Model> m_pModel; // A copy of original model

	Model* getModel() const { return m_pSelf->getModel(); }
	
	bool findInitialSolution();
	bool makeVarCombo( size_t, size_t, vector<VarBoundary>& );
	bool isSolutionFeasible( const Matrix& ) const;
	const Matrix solvePriceVector( const vector<size_t>&, const Matrix&, const Matrix& ) const;
	
	bool iterate();
	bool determineEnteringNonBasicVar( EnterBasicVar& );
	void calculateNewX( const EnterBasicVar&, size_t&, Matrix& );
	void updateNonBasicVars( const EnterBasicVar&, size_t );
	void updateInverseBasicMatrix( const EnterBasicVar&, size_t, const Matrix& );
	void printIterateHeader() const;
	bool isPriceBoundEligible( const EnterBasicVar& ) const;
	void getLambda( const Matrix&, const Matrix&, double&, size_t& ) const;

};

BoundedRevisedSimplexImpl::BoundedRevisedSimplexImpl( BoundedRevisedSimplex* p ) :
	m_pSelf( p ), m_mxBasicInv( 0, 0 ), m_mxUpdateMatrix( 0, 0 ), m_mxPriceVector( 0, 0 ),
	m_mxX( 0, 0 ), m_nIter( 0 ), m_mxA( 0, 0 ), m_mxB( 0, 0 ), m_mxC( 0, 0 )
{
}

BoundedRevisedSimplexImpl::~BoundedRevisedSimplexImpl()
{
}

Matrix BoundedRevisedSimplexImpl::solve()
{
	//-------------------------------------------------------------------------
	// Initialize
	
	//-------------------------------------------------------------------------
	// Normalize the objective with use of slack variable(s), and store in the 
	// following variables such that:
	//
	//     A = expanded constraint matrix
	//     B = right hand side vector
	//     C = expanded cost vector

	m_mxBasicInv.clear();
	m_mxUpdateMatrix.clear();

	auto_ptr<Model> ptr( new Model( *getModel() ) );
	m_pModel = ptr; // Note: transfer of ownership

	m_mxA = m_pModel->getConstraintMatrix();
	m_mxB = m_pModel->getRhsVector();
	m_mxC = m_pModel->getCostVector();

	const size_t nRowSizeA = m_mxA.rows();
	for ( size_t i = 0; i < nRowSizeA; ++i )
	{
		Equality eEq = m_pModel->getEqualityByRowId( i );
		switch( eEq )
		{
		case LESS_THAN_EQUAL:
			m_mxA( i, m_mxA.cols() ) = 1;
			break;
		case GREATER_THAN_EQUAL:
			m_mxA( i, m_mxA.cols() ) = -1;
			break;
		default:
			OSL_ASSERT( eEq == EQUAL );
		}
		if ( eEq != EQUAL )
		{
			size_t nCol = m_mxC.cols();
			m_pModel->setCostVectorElement( nCol, 0 );
			m_pModel->setVarBound( nCol, BOUND_LOWER, 0 );
			m_mxC( 0, nCol ) = 0;
		}
	}
	m_pModel->print();

	if ( m_pModel->getVerbose() )
	{
		cout << "A:" << endl;
		m_mxA.print();
		cout << "B:" << endl;
		m_mxB.print();
		cout << "C:" << endl;
		m_mxC.print();
	}

	if ( !findInitialSolution() )
	{
		if ( m_pModel->getVerbose() )
		{
			cout << "Initial solution not found" << endl;
			throw ModelInfeasible();
		}
	}
	
	if ( m_pModel->getVerbose() )
		cout << "Initial solution found" << endl;

	m_mxPriceVector = solvePriceVector( m_aBasicVarId, m_mxBasicInv, m_mxC );

	m_nIter = 0;
	m_aSkipBasicVarId.clear();
	
	while ( !iterate() );
	
	Matrix mxSolution;
	for ( size_t i = 0; i < m_pModel->getCostVector().cols(); ++i )
		mxSolution( i, 0 ) = m_mxX( i, 0 );

	if ( m_pModel->getVerbose() )
	{
		cout << "x = ";
		mxSolution.trans().print();	
	}

	return mxSolution;
}

void BoundedRevisedSimplexImpl::printIterateHeader() const
{
	cout << endl;
	string line = repeatString( "-", 70 );

	cout << line << endl;
	cout << "Iteration " << m_nIter << endl;
	cout << line << endl;
	
	cout << "x(" << m_nIter << ") = ";
	m_mxX.trans().print( getModel()->getPrecision() );
	cout << "cx = " << (m_mxC*m_mxX).operator()( 0, 0 ) << endl;
	cout << line << endl;
	
	cout << "Basic Variable ID: ";
	printElements( m_aBasicVarId );
	cout << endl << endl;;
	
	vector<size_t>::const_iterator itr, 
		itrBeg = m_aNonBasicVarId.begin(),
		itrEnd = m_aNonBasicVarId.end();
	for ( itr = itrBeg; itr != itrEnd; ++itr )
	{
		cout << *itr << ": ";
		int idx = distance( itrBeg, itr );
		switch ( m_aNonBasicVarBoundType.at( idx ) )
		{
		case BOUND_LOWER:
			cout << "L" << endl;
			break;
		case BOUND_UPPER:
			cout << "U" << endl;
			break;
		default:
			OSL_ASSERT( !"wrong bound type" );
		}
	}
	cout << endl;
	
	cout << "A^(-1) (shortcut calculation)" << endl;
	m_mxBasicInv.print();
	
	cout << endl << "v = ";
	m_mxPriceVector.print();
}

bool BoundedRevisedSimplexImpl::isPriceBoundEligible( const EnterBasicVar& aVar ) const
{
	bool b;

	switch ( m_pModel->getGoal() )
	{
	case GOAL_MAXIMIZE:
		// A maximizing model requires either a lower-bounded non-basic with a 
		// positive price or a upper-bounded non-basic with a negative price.

		b = ( aVar.Price > 0.0 && aVar.BoundType == BOUND_LOWER ) ||
			( aVar.Price < 0.0 && aVar.BoundType == BOUND_UPPER );
		break;

	case GOAL_MINIMIZE:
		// A minimizing model requires a lower-bounded non-basic with a negative 
		// price or an upper-bounded non-basic with a positive price.

		b = ( aVar.Price < 0.0 && aVar.BoundType == BOUND_LOWER ) ||
			( aVar.Price > 0.0 && aVar.BoundType == BOUND_UPPER );
		break;
	default:
		OSL_ASSERT( !"wrong goal" );
	}

	return b;
}

void BoundedRevisedSimplexImpl::getLambda( const Matrix& X, const Matrix& dX, 
		double& rLambda, size_t& rLeaveVarId ) const
{
	OSL_ASSERT( X.rows() == dX.rows() );
	double fLmdPos = 0.0, fLmdNeg = 0.0, fLmd = 0.0;
	size_t nId;
	
	for ( size_t i = 0; i < X.rows(); ++i )
	{
		double fXVal  = X( i, 0 );
		double fdXVal = dX( i, 0 );
		if ( fdXVal > 0.0 && m_pModel->isVarBounded( i, BOUND_UPPER ) )
		{
			// dX positive
			double fUpper = m_pModel->getVarBound( i, BOUND_UPPER );
			double fTmp = ( fUpper - fXVal ) / fdXVal;
			if ( ( fLmdPos != 0.0 && fLmdPos > fTmp ) || fLmdPos == 0.0 )
			{
				cout << "pos: " << fTmp << " " << i << endl;
				fLmdPos = fTmp;
			}
		}
		else if ( fdXVal < 0.0 && m_pModel->isVarBounded( i, BOUND_LOWER ) )
		{
			// dX negative
			double fLower = m_pModel->getVarBound( i, BOUND_LOWER );
			double fTmp = ( fXVal - fLower ) / ( fdXVal*(-1) );
			if ( ( fLmdNeg != 0.0 && fLmdNeg > fTmp ) || fLmdNeg == 0.0 )
			{
				cout << "neg: " << fTmp << " " << i << endl;
				fLmdNeg = fTmp;
			}
		}
		
		double fLmdTmp;
		if ( fLmdPos != 0.0 && fLmdNeg != 0.0 )
			fLmdTmp = min( fLmdPos, fLmdNeg );
		else if ( fLmdPos != 0.0 )
			fLmdTmp = fLmdPos;
		else if ( fLmdNeg != 0.0 )
			fLmdTmp = fLmdNeg;
		else
			fLmdTmp = 0.0;
			
		if ( ( fLmdTmp != 0.0 && fLmdTmp < fLmd ) || fLmd == 0.0 )
		{
			fLmd = fLmdTmp;
			nId = i;
		}
	}
	std::swap( fLmd, rLambda );
	std::swap( nId, rLeaveVarId );
}

bool BoundedRevisedSimplexImpl::iterate()
{
	if ( getModel()->getVerbose() )
		printIterateHeader();

	EnterBasicVar aEnterVar; // Entering basic variable (ID, Price and BoundType)
	if ( determineEnteringNonBasicVar( aEnterVar ) )
		return true;

	size_t nLeaveVarId;	// Leaving basic variable ID
	Matrix mxDX;
	calculateNewX( aEnterVar, nLeaveVarId, mxDX );
	updateNonBasicVars( aEnterVar, nLeaveVarId );
	updateInverseBasicMatrix( aEnterVar, nLeaveVarId, mxDX );

	++m_nIter;
	return false;
}

/** As the function name suggests, this method determines an entering non-basic variable
	if any.  If there is no entering non-basic, then that means an optimum solution is 
	found, and it returns true.  Otherwise it returns false.
 */
bool BoundedRevisedSimplexImpl::determineEnteringNonBasicVar( EnterBasicVar& rEnterVar )
{
	//-------------------------------------------------------------------------
	// Determine the entering non-basic variable

	vector<size_t>   aEnterBasicVarId;
	vector<EnterBasicVar> aEnterBasicVars;

	vector<size_t>::const_iterator itr,
			itrBeg = m_aNonBasicVarId.begin(),
			itrEnd = m_aNonBasicVarId.end();

	for ( itr = itrBeg; itr != itrEnd; ++itr )
	{
		EnterBasicVar aVar;
		size_t id = *itr;
		aEnterBasicVarId.push_back( id );
		aVar.Id = id;
		size_t i = distance( itrBeg, itr );
		aVar.BoundType = m_aNonBasicVarBoundType.at( i );
		
		// Evaluate pricing
		aVar.Price = m_mxC( 0, id ) - 
				( m_mxPriceVector*m_mxA.getColumn( id ) ).operator()( 0, 0 );

		if ( getModel()->getVerbose() )
			cout << "c(" << aVar.Id << ") = " << aVar.Price << endl;
		aEnterBasicVars.push_back( aVar );
	}

	bool bFirstEnterVarFound = false;		
	itrBeg = aEnterBasicVarId.begin();
	itrEnd = aEnterBasicVarId.end();
	for ( itr = itrBeg; itr != itrEnd; ++itr )
	{
		size_t i = distance( itrBeg, itr );
		EnterBasicVar aVar = aEnterBasicVars.at( i );
		OSL_ASSERT( aVar.Id == *itr );
		
		if ( isPriceBoundEligible( aVar ) )
		{
			if ( !bFirstEnterVarFound )
			{
				bFirstEnterVarFound = true;
				rEnterVar = aVar;
			}
			else if ( abs( rEnterVar.Price ) < abs( aVar.Price ) )
				rEnterVar = aVar;
		}
	}
	
	if ( !bFirstEnterVarFound )
	{
		if ( getModel()->getVerbose() )
			cout << "Optimum solution reached" << endl;
		return true;
	}
	return false;
}

/** This methods calculates, given an entering non-basic variable, a dX, lambda, and leaving
	non-basic variable.  It then calculates a new X from them.
 */
void BoundedRevisedSimplexImpl::calculateNewX( const EnterBasicVar& aEnterVar,
		size_t& nLeaveVarId, Matrix& mxDX )
{
	Matrix mxB = m_mxA.getColumn( aEnterVar.Id );
	if ( aEnterVar.BoundType == BOUND_LOWER )
		mxB *= (-1);

	Matrix dXBasic( m_mxBasicInv*mxB );

	Matrix dX;
	for ( size_t i = 0; i < dXBasic.rows(); ++i )
		dX( m_aBasicVarId.at( i ) , 0 ) = dXBasic( i, 0 );

	uInt32CIter itr;
	uInt32CIter itrBeg = m_aNonBasicVarId.begin(), itrEnd = m_aNonBasicVarId.end();
	
	for ( itr = itrBeg; itr != itrEnd; ++itr )
	{
		size_t nId = *itr;
		if ( nId == aEnterVar.Id )
		{
			switch ( aEnterVar.BoundType )
			{
			case BOUND_LOWER:
				dX( nId, 0 ) = 1.0;
				break;
			case BOUND_UPPER:
				dX( nId, 0 ) = -1.0;
				break;
			default:
				OSL_ASSERT( !"wrong bound type" );
			}
		}
		else
			dX( nId, 0 ) = 0.0;
	}
	
	if ( getModel()->getVerbose() )
	{
		cout << "dX[" << aEnterVar.Id << "] = ";
		dX.trans().print( getModel()->getPrecision() );
	}

	double fLambda;
	getLambda( m_mxX, dX, fLambda, nLeaveVarId );

	if ( fLambda == 0.0 )
	{
		if ( m_pModel->getVerbose() )
			cout << "Unbounded model with no solution";
		throw ModelInfeasible();
	}
	
	if ( m_pModel->getVerbose() )
		cout << "lambda = " << fLambda << "  x_" << nLeaveVarId << " leaves and x_"
			 << aEnterVar.Id << " enters" << endl;

	m_mxX += dX*fLambda;
	mxDX = dX;
}

/** Given entering and leaving basic variables, update corresponding member containers 
	to reflect the change.
 */
void BoundedRevisedSimplexImpl::updateNonBasicVars( const EnterBasicVar& aEnterVar, size_t nLeaveVarId )
{
	//-------------------------------------------------------------------------
	// Update non-basic variable information
	
	uInt32Iter itr, itrBeg = m_aNonBasicVarId.begin(), itrEnd = m_aNonBasicVarId.end();
	BoundIter itr2, itr2Beg = m_aNonBasicVarBoundType.begin(), itr2End = m_aNonBasicVarBoundType.end();
	
	for ( itr = itrBeg, itr2 = itr2Beg ; itr != itrEnd; ++itr, ++itr2 )
		if ( *itr == aEnterVar.Id )
		{
			OSL_ASSERT( *itr2 == aEnterVar.BoundType );
			m_aNonBasicVarId.erase( itr );
			m_aNonBasicVarBoundType.erase( itr2 );
			break;
		}
	OSL_ASSERT ( itr != itrEnd && itr2 != itr2End );
	
	// Since the variable with the index of nLeaveVar is now non-basic, it must be
	// bounded at either end.

	if ( m_pModel->isVarBounded( nLeaveVarId, BOUND_LOWER ) &&
		 m_mxX( nLeaveVarId, 0 ) == m_pModel->getVarBound( nLeaveVarId, BOUND_LOWER ) )
	{
		m_aNonBasicVarId.push_back( nLeaveVarId );
		m_aNonBasicVarBoundType.push_back( BOUND_LOWER );
	}
	else if ( m_pModel->isVarBounded( nLeaveVarId, BOUND_UPPER ) &&
			  m_mxX( nLeaveVarId, 0 ) == m_pModel->getVarBound( nLeaveVarId, BOUND_UPPER ) )
	{
		m_aNonBasicVarId.push_back( nLeaveVarId );
		m_aNonBasicVarBoundType.push_back( BOUND_UPPER );
	}
	
	OSL_ASSERT( m_pModel->isVarBounded( nLeaveVarId, BOUND_LOWER ) || 
			m_pModel->isVarBounded( nLeaveVarId, BOUND_UPPER ) );
}

void BoundedRevisedSimplexImpl::updateInverseBasicMatrix( const EnterBasicVar& aEnterVar, size_t nLeaveVarId, const Matrix& mxDX )
{
	//-------------------------------------------------------------------------
	// Update the inverse matrix if the leaving variable is basic
	
	uInt32Iter itrBeg = m_aBasicVarId.begin(), itrEnd = m_aBasicVarId.end();
	uInt32Iter itr = find( itrBeg, itrEnd, nLeaveVarId );
	if ( itr != itrEnd )
	{
		*itr = aEnterVar.Id;
		size_t nIndexLeaveVar = distance( itrBeg, itr );
		OSL_ASSERT( m_mxBasicInv.isSquare() );
		Matrix mxE( m_mxBasicInv.rows(), m_mxBasicInv.cols(), true );
		double fLeaveVar = mxDX( nLeaveVarId, 0 );
		
		for ( size_t i = 0; i < mxE.rows(); ++i )
		{
			size_t nId = m_aBasicVarId.at( i );
			double fDXVal = mxDX( nId, 0 );
			if ( i == nIndexLeaveVar )
				if ( aEnterVar.BoundType == BOUND_LOWER )
					mxE( i, i ) = -1.0 / fLeaveVar;
				else
					mxE( i, i ) =  1.0 / fLeaveVar; //??
			else
				mxE( i, nIndexLeaveVar ) = fDXVal / fLeaveVar * (-1.0);
		}
		
		if ( m_pModel->getVerbose() )
		{
			cout << "E:";
			mxE.print( m_pModel->getPrecision() );
		}
		
		m_mxBasicInv = mxE * m_mxBasicInv;
		m_mxPriceVector = solvePriceVector( m_aBasicVarId, m_mxBasicInv, m_mxC );
	}
}

bool BoundedRevisedSimplexImpl::findInitialSolution()
{
	m_pModel->print();
	size_t nNumInitNonBasic = m_mxA.cols() - m_mxB.rows();
	if ( m_pModel->getVerbose() )
	{
		const string line = repeatString( "-", 70 );
		cout << endl << line << endl;
		cout << "Initial solution search" << endl << line << endl;
		cout << "nNumInitNonBasic = " << nNumInitNonBasic << endl;
	}

	vector<VarBoundary> aArray;
	return makeVarCombo( 0, nNumInitNonBasic, aArray );
}

bool BoundedRevisedSimplexImpl::makeVarCombo( size_t nIdx, size_t nUpper, 
		vector<VarBoundary>& aArray )
{	
	size_t nNumCost = m_pModel->getCostVector().cols();	// number of cost vector elements
	vector<VarBoundary>::const_iterator iter;
	vector<VarBoundary>::const_iterator itrEnd = aArray.end();
	
//     if ( nIdx == nNumCost || aArray.size() == nUpper )
	if ( aArray.size() == nUpper )
	{
		// Solve the rest of the elements, and check if this initial X is feasible.  If yes,
		// return true, if no return false.

		Matrix mxX;
		vector<size_t> aBasicVarId;
		vector<size_t> aNonBasicVarId;
		vector<Bound> aNonBasicVarBoundType;
		
		vector<VarBoundary>::const_iterator iter;
		vector<VarBoundary>::const_iterator itrEnd = aArray.end();
		for ( iter = aArray.begin() ; iter != itrEnd; ++iter )
		{
			VarBoundary iva = *iter;
			aNonBasicVarId.push_back( iva.Id );
			aNonBasicVarBoundType.push_back( iva.BoundType );
			mxX( iva.Id, 0 ) = iva.Value;
		}
		
		for ( size_t i = 0; i < m_mxA.cols(); ++i )
			if ( find( aNonBasicVarId.begin(), aNonBasicVarId.end(), i ) == aNonBasicVarId.end() )
				aBasicVarId.push_back( i );
		
		if ( m_pModel->getVerbose() )
		{
			cout << "aBasicVarId = ";
			printElements( aBasicVarId );
			cout << "  aNonBasicVarId = ";
			printElements( aNonBasicVarId );
			cout << endl;
		}
		
		Matrix mxTmpA( m_mxA );
		mxTmpA.deleteColumns( aBasicVarId );

		Matrix mxTmp;
		
		size_t nId = 0;
		for ( iter = aArray.begin(); iter != itrEnd; ++iter )
			mxTmp( nId++, 0 ) = (*iter).Value;
			
		Matrix mxTmpB( m_mxB );
		mxTmpB -= mxTmpA*mxTmp;
		
		Matrix mxBasA( m_mxA );
		mxBasA.deleteColumns( aNonBasicVarId );
		Matrix mxBasAInv = mxBasA.inverse();
		Matrix mxBasX = mxBasAInv * mxTmpB;
		cout << "mxBasAInv: " << endl;
		mxBasA.print();
		mxBasAInv.print();
		mxTmpB.print();
		cout << "mxBasX: " << endl;
		mxBasX.print();
		
		for ( size_t i = 0; i < mxBasX.rows(); ++i )
			mxX( aBasicVarId.at( i ), 0 ) = mxBasX( i, 0 );
			
		if ( m_pModel->getVerbose() )
		{
			cout << "X[0] = ";
			mxX.trans().print();
		}
		
		// Now check if the mxX is feasible.
		if ( isSolutionFeasible( mxX ) )
		{
			m_mxBasicInv = mxBasAInv;
			m_mxX = mxX;
			m_aBasicVarId = aBasicVarId;
			m_aNonBasicVarId = aNonBasicVarId;
			m_aNonBasicVarBoundType = aNonBasicVarBoundType;
			return true;
		}
		else
			return false;
	}
	
	Bound e[2] = { BOUND_LOWER, BOUND_UPPER };
	for ( size_t i = 0; i < 2; ++i )
		if ( m_pModel->isVarBounded( nIdx, e[i] ) )
		{
			vector<VarBoundary> aArray1 = aArray;
			VarBoundary temp;
			temp.Id = nIdx;
			temp.Value = m_pModel->getVarBound( nIdx, e[i] );
			temp.BoundType = e[i];
			aArray1.push_back( temp );
			if ( makeVarCombo( nIdx + 1, nUpper, aArray1 ) )
				return true;
		}
		
	if ( !m_pModel->isVarBounded( nIdx, BOUND_LOWER ) &&
		 !m_pModel->isVarBounded( nIdx, BOUND_UPPER ) )
		if ( makeVarCombo( nIdx + 1, nUpper, aArray ) )
			return true;

	return false;
}

bool BoundedRevisedSimplexImpl::isSolutionFeasible( const Matrix& mxX ) const
{
	const size_t nNumCost = m_mxC.cols();
	OSL_ASSERT( nNumCost == mxX.rows() );
	
	for ( size_t i = 0; i < mxX.rows(); ++i )
	{
		double fVal = mxX( i, 0 );
		if ( ( m_pModel->isVarBounded( i, BOUND_LOWER ) && 
			   fVal < m_pModel->getVarBound( i, BOUND_LOWER ) ) ||
			 ( m_pModel->isVarBounded( i, BOUND_UPPER ) &&
			   fVal > m_pModel->getVarBound( i, BOUND_UPPER ) ) )
			return false;
	}

	if ( m_mxA*mxX != m_mxB )
		return false;

	return true;
}

const Matrix BoundedRevisedSimplexImpl::solvePriceVector( 
		const vector<size_t>& aBasicVarId,
		const Matrix& mxAInv, const Matrix& mxC ) const
{
	Matrix c;
	vector<size_t>::const_iterator itr;
	vector<size_t>::const_iterator itrBeg = aBasicVarId.begin();
	vector<size_t>::const_iterator itrEnd = aBasicVarId.end();
	for ( itr = itrBeg; itr != itrEnd; ++itr )
	{
		int idx = distance( itrBeg, itr );
		c( 0, idx ) = mxC( 0, *itr );
	}
	return c*mxAInv;
}


//---------------------------------------------------------------------------
// BoundedRevisedSimplex

BoundedRevisedSimplex::BoundedRevisedSimplex() : BaseAlgorithm(),
	m_pImpl( new BoundedRevisedSimplexImpl( this ) )
{
}

BoundedRevisedSimplex::~BoundedRevisedSimplex()
{
}

void BoundedRevisedSimplex::solve()
{
	Matrix sol = m_pImpl->solve();
	setSolution( sol );
}


}}}}
