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

#include <numeric/lpmodel.hxx>
#include <numeric/lpbase.hxx>
#include <numeric/matrix.hxx>
#include <global.hxx>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <boost/numeric/ublas/matrix.hpp>

using namespace std;
using ::scsolver::numeric::Matrix;

namespace scsolver { namespace numeric { namespace opres { namespace lp {

class NonBoundingException : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Non bounding exception";
	}
};

struct AttrBound
{
	bool Enabled;
	double Value;
};

struct VarBounds
{
	AttrBound Upper;
	AttrBound Lower;
};

typedef map<unsigned long,VarBounds> VarBoundMap;

class ModelImpl
{
public:
	ModelImpl();
	ModelImpl( const ModelImpl& );
	~ModelImpl() throw();
	void swap( ModelImpl& ) throw();
	void print() const;

	Matrix getCostVector() const { return m_mxCost; }
	void setCostVectorElement( size_t, double );
	void setCostVector( const std::vector<double>& );

	AttrBound getVarBoundAttribute( unsigned long, Bound ) const;
	double    getVarBound( unsigned long, Bound ) const;
	void      setVarBound( unsigned long, Bound, double );
	bool  isVarBounded( unsigned long, Bound ) const;

	Goal getGoal() const { return m_eGoal; }
	void setGoal( Goal e ) { m_eGoal = e; }
	
	unsigned long getPrecision() const { return m_nPrecision; }
	void       setPrecision( unsigned long n ) { m_nPrecision = n; }

	bool getVarPositive() const { return m_bVarPositive; }
	void setVarPositive( bool b ) { m_bVarPositive = b; }

	bool getVerbose() const { return m_bVerbose; }
	void setVerbose( bool b ) { m_bVerbose = b; }

	Matrix getConstraintMatrix() const { return m_mxConstraint; }
	Matrix getRhsVector() const { return m_mxRHS; }
	vector<Equality> getEqualityVector() const { return m_eEqualities; }
	Equality getEqualityByRowId( unsigned long i ) const { return m_eEqualities.at( i ); }
	void addConstraint( const std::vector<double>&, Equality, double );
	void setStandardConstraintMatrix( const Matrix&, const Matrix& );
	
	void solve( BaseAlgorithm* );
	void solve( const std::auto_ptr<BaseAlgorithm>& );
	Matrix getSolution() const { return m_mxSolution; }
	void setSolution( const Matrix& );

private:
	Matrix m_mxCost;
	Matrix m_mxConstraint;
	Matrix m_mxRHS;

	map<unsigned long, VarBounds> m_aVarBoundMap;

	vector<Equality> m_eEqualities;

	Goal m_eGoal;
	unsigned long m_nPrecision;
	bool m_bVarPositive;

	bool m_bVerbose;

	Matrix m_mxSolution;

};


ModelImpl::ModelImpl() : 
	m_mxCost( 0, 0 ), m_mxConstraint( 0, 0 ), m_mxRHS( 0, 0 ),
	m_nPrecision( 2 ), m_bVarPositive( true ), m_bVerbose( false ), m_mxSolution( 0, 0 )
{
}

ModelImpl::ModelImpl( const ModelImpl& other ) :
	m_mxCost( other.m_mxCost ), 
	m_mxConstraint( other.m_mxConstraint ), 
	m_mxRHS( other.m_mxRHS ),
	m_aVarBoundMap( other.m_aVarBoundMap ), 
	m_eEqualities( other.m_eEqualities ), 
	m_eGoal( other.m_eGoal ), 
	m_nPrecision( other.m_nPrecision ), 
	m_bVarPositive( other.m_bVarPositive ), 
	m_bVerbose( other.m_bVerbose ), 
	m_mxSolution( other.m_mxSolution )
{
}

ModelImpl::~ModelImpl() throw()
{
}

void ModelImpl::swap( ModelImpl& other ) throw()
{
	m_mxCost.swap( other.m_mxCost );
	m_mxConstraint.swap( other.m_mxConstraint );
	m_mxRHS.swap( other.m_mxRHS );
	std::swap( m_aVarBoundMap, other.m_aVarBoundMap );
	std::swap( m_eEqualities, other.m_eEqualities );
	std::swap( m_eGoal, other.m_eGoal );
	std::swap( m_nPrecision, other.m_nPrecision );
	std::swap( m_bVarPositive, other.m_bVarPositive );
	std::swap( m_bVerbose, other.m_bVerbose );
	m_mxSolution.swap( other.m_mxSolution );
}

void ModelImpl::setCostVectorElement( size_t nId, double fVal )
{
	// Cost vector must be a row vector because the X vector is a column.
	m_mxCost( 0, nId ) = fVal;
}

void ModelImpl::setCostVector( const std::vector<double>& cn )
{
	std::vector<double>::const_iterator it, 
		itBeg = cn.begin(), itEnd = cn.end();
	for ( it = itBeg; it != itEnd; ++it )
	{
		size_t nDist = std::distance( itBeg, it );
		setCostVectorElement( nDist, *it );
	}
}

AttrBound ModelImpl::getVarBoundAttribute( unsigned long i, Bound e ) const
{
	VarBoundMap::const_iterator pos = m_aVarBoundMap.find( i );
	AttrBound ab;
	if ( pos == m_aVarBoundMap.end() )
	{
		ab.Enabled = false;
		ab.Value = 0.0;
	}
	else
	{
		VarBounds vb = pos->second;
		switch ( e )
		{
		case BOUND_UPPER:
			ab = vb.Upper;
			break;
		case BOUND_LOWER:
			ab = vb.Lower;
			break;
		default:
			OSL_ASSERT( !"unknown boundary" );			
		}
	}
	return ab;
}

double ModelImpl::getVarBound( unsigned long i, Bound e ) const
{
	AttrBound ab = getVarBoundAttribute( i, e );
	if ( ab.Enabled )
		return ab.Value;
	else
		throw NonBoundingException();
}

void ModelImpl::setVarBound( unsigned long i, Bound e, double fBound )
{
	VarBoundMap::iterator pos = m_aVarBoundMap.find( i );
	if ( pos == m_aVarBoundMap.end() )
	{
		// First entry
		VarBounds vb;
		AttrBound upper;
		AttrBound lower;
		switch ( e )
		{
		case BOUND_UPPER:
			upper.Enabled = true;
			upper.Value = fBound;
			vb.Upper = upper;
			lower.Enabled = false;
			lower.Value = 0.0;
			vb.Lower = lower;
			break;
		case BOUND_LOWER:
			upper.Enabled = false;
			upper.Value = 0.0;
			vb.Upper = upper;
			lower.Enabled = true;
			lower.Value = fBound;
			vb.Lower = lower;
			break;
		default:
			OSL_ASSERT( !"unknown boundary" );
		}
		m_aVarBoundMap.insert( map<unsigned long,VarBounds>::value_type( i, vb ) );
	}
	else
	{
		// Entry exists
		VarBounds vb = pos->second;
		switch ( e )
		{
		case BOUND_UPPER:
			vb.Upper.Enabled = true;
			vb.Upper.Value = fBound;
			break;
		case BOUND_LOWER:
			vb.Lower.Enabled = true;
			vb.Lower.Value = fBound;
			break;
		default:
			OSL_ASSERT( !"unknown boundary" );
		}
		m_aVarBoundMap[i] = vb;
	}
}

bool ModelImpl::isVarBounded( unsigned long i, Bound e ) const
{
	AttrBound ab = getVarBoundAttribute( i, e );
	return ab.Enabled;
}

void ModelImpl::addConstraint( const std::vector<double>& aConst, Equality eEqual, double fRHS )
{
	unsigned long nRowId = m_mxConstraint.rows();
	vector<double>::const_iterator pos;
	for ( pos = aConst.begin(); pos != aConst.end(); ++pos )
	{
		unsigned long nColId = distance( aConst.begin(), pos );
		m_mxConstraint( nRowId, nColId ) = *pos;
	}
	m_eEqualities.push_back( eEqual );
	
	// RHS vector must be a column.
	m_mxRHS( m_mxRHS.rows(), 0 ) = fRHS;
}

void ModelImpl::setStandardConstraintMatrix( const Matrix& A, const Matrix& B )
{
	if ( A.rows() != B.rows() )
		throw MatrixSizeMismatch();
	
	for ( unsigned long i = 0; i < A.rows(); ++i )
	{
		vector<double> aConst;
		for ( unsigned long j = 0; j < A.cols(); ++j )
			aConst.push_back( A( i, j ) );
		addConstraint( aConst, EQUAL, B( i, 0 ) );
	}
}

void ModelImpl::print() const
{
	using namespace ::boost::numeric::ublas;
	unsigned long nColSpace = 2;
	string sX = "x_";
	cout << endl << repeatString( "-", 70 ) << endl;
	string sGoal;
	if ( m_eGoal == GOAL_MAXIMIZE )
		sGoal = "max";
	else if ( m_eGoal == GOAL_MINIMIZE )
		sGoal = "min";
	else
		//OSL_ASSERT( !"wrong goal" );
		sGoal = "unknown";

	ostringstream osLine;
	osLine << "Objective: ";
	bool bFirst = true;
	for ( unsigned long j = 0; j < m_mxCost.cols(); ++j )
	{
		double fVal = m_mxCost( 0, j );
		if ( fVal != 0.0 )
		{
			if ( bFirst )
			{
				bFirst = false;
				osLine << fVal << sX << j;
			}
			else
			{
				if ( fVal > 0.0 )
					osLine << " + " << fVal << sX << j;
				else
					osLine << " - " << abs(fVal) << sX << j;
			}
		}
	}
	osLine << "  (" << sGoal << ")";
	cout << osLine.str() << endl << repeatString( "-", 70 ) << endl;
	cout << "Subject to Constraints:" << endl << endl;
	matrix< string > mElements = m_mxConstraint.getDisplayElements( 0, nColSpace, true );
	matrix< string > mRHS = m_mxRHS.getDisplayElements( 0, nColSpace, false );
	
	// Print constraints
	for ( unsigned long i = 0; i < m_mxConstraint.rows(); ++i )
	{
		osLine.str( "" );
		for ( unsigned long j = 0; j < m_mxConstraint.cols(); ++j )
		{
			string s = mElements( i, j );
			double f = m_mxConstraint( i, j );
			ostringstream osVar;
			osVar << s << sX << j;
			if ( f == 0.0 )
				osLine << repeatString( " ", osVar.str().length() );
			else
				osLine << osVar.str();
		}
		
		osLine << repeatString( " ", nColSpace );
		
		switch ( m_eEqualities.at( i ) )
		{
			case GREATER_THAN_EQUAL:
				osLine << ">=";
				break;
			case EQUAL:
				osLine << " =";
				break;
			case LESS_THAN_EQUAL:
				osLine << "<=";
				break;
			default:
				OSL_ASSERT( !"wrong case" );
		}
		
		osLine << repeatString( " ", nColSpace ) << m_mxRHS( i, 0 );
		cout << osLine.str() << endl;
	}
	cout << repeatString( "-", 70 ) << endl;
	
	if ( m_bVarPositive )
	{
		cout << "All decision variables are assumed positive" << endl;
		cout << repeatString( "-", 70 ) << endl;
	}
	
	// Display variable boundaries.
	Matrix mxCost = getCostVector(); // row vector
	osLine.str( "" );
	for ( unsigned long i = 0; i < mxCost.cols(); ++i )
	{
		osLine << "x_" << i << ": ";
		bool bLower = isVarBounded( i, BOUND_LOWER );
		bool bUpper = isVarBounded( i, BOUND_UPPER );
		if ( bLower || bUpper )
		{
			if ( bLower )
			{
				double fBound = getVarBound( i, BOUND_LOWER );
				osLine << fBound;
			}
			else
				osLine << "[none]";

			osLine << " - ";
			
			if ( bUpper )
			{
				double fBound = getVarBound( i, BOUND_UPPER );
				osLine << fBound;
			}
			else
				osLine << "[none]";
		}
		else
			osLine << "[unbounded]";
		osLine << endl;
	}
	cout << osLine.str();
	cout << repeatString( "-", 70 ) << endl;
}

void ModelImpl::solve( BaseAlgorithm* pAlgorithm )
{
	pAlgorithm->solve();
	setSolution( pAlgorithm->getSolution() );
}

void ModelImpl::solve( const std::auto_ptr<BaseAlgorithm>& pAlgorithm )
{
	pAlgorithm->solve();
	setSolution( pAlgorithm->getSolution() );
}

void ModelImpl::setSolution( const Matrix& other )
{
	Matrix m( other );
	m_mxSolution.swap( m );
}


//---------------------------------------------------------------------------
// Model

Model::Model() : m_pImpl( new ModelImpl() )
{
}

Model::Model( const Model& other ) : m_pImpl( new ModelImpl( *other.m_pImpl.get() ) )
{
}

Model::~Model()
{
}

void Model::print() const
{
	m_pImpl->print();
}

/** get a matrix representing a cost vector.  Note that
 * a cost vector is a row matrix i.e. it only has one row.
 * 
 * @return row matrix representing a cost vector
 */
Matrix Model::getCostVector() const
{
	return m_pImpl->getCostVector();
}

void Model::setCostVectorElement( size_t i, double fVal )
{
	m_pImpl->setCostVectorElement( i, fVal );
}

void Model::setCostVector( const std::vector<double>& cn )
{
	m_pImpl->setCostVector( cn );
}

/** Beware that the caller is responsible for making sure that the specified 
	boundary exists by calling isVarBounded(...) beforehand.
 */
double Model::getVarBound( unsigned long i, Bound e ) const
{
	return m_pImpl->getVarBound( i, e );
}

void Model::setVarBound( unsigned long i, Bound e, double fValue )
{
	m_pImpl->setVarBound( i, e, fValue );
}

bool Model::isVarBounded( unsigned long i, Bound e ) const
{
	return m_pImpl->isVarBounded( i, e );
}

Goal Model::getGoal() const
{
	return m_pImpl->getGoal();
}

void Model::setGoal( Goal e )
{
	m_pImpl->setGoal( e );
}

unsigned long Model::getPrecision() const
{
	return m_pImpl->getPrecision();
}

void Model::setPrecision( unsigned long i )
{
	m_pImpl->setPrecision( i );
}

bool Model::getVarPositive() const
{
	return m_pImpl->getVarPositive();
}

void Model::setVarPositive( bool b )
{
	m_pImpl->setVarPositive( b );
}

bool Model::getVerbose() const
{
	return m_pImpl->getVerbose();
}

void Model::setVerbose( bool b )
{
	m_pImpl->setVerbose( b );
}

Matrix Model::getConstraintMatrix() const
{
	return m_pImpl->getConstraintMatrix();
}

Matrix Model::getRhsVector() const
{
	return m_pImpl->getRhsVector();
}

std::vector<Equality> Model::getEqualityVector() const
{
	return m_pImpl->getEqualityVector();
}

Equality Model::getEqualityByRowId( unsigned long i ) const
{
	return m_pImpl->getEqualityByRowId( i );
}

void Model::addConstraint( const std::vector< double >& v, Equality e, double fRhs )
{
	m_pImpl->addConstraint( v, e, fRhs );
}

void Model::setStandardConstraintMatrix( const Matrix& mxConst, const Matrix& mxRhs )
{
	m_pImpl->setStandardConstraintMatrix( mxConst, mxRhs );
}

void Model::solve( BaseAlgorithm* pAlgorithm )
{
	pAlgorithm->setModel( this );
	m_pImpl->solve( pAlgorithm );
}

void Model::solve( const std::auto_ptr<BaseAlgorithm>& ptr )
{
	ptr->setModel( this );
	m_pImpl->solve( ptr );
}

Matrix Model::getSolution() const
{
	return m_pImpl->getSolution();
}

void Model::setSolution( const Matrix& mx )
{
	m_pImpl->setSolution( mx );
}


}}}}



