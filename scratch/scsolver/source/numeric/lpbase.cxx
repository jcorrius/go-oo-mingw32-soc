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

#include "numeric/lpbase.hxx"
#include "numeric/lpmodel.hxx"
#include <list>
#include <vector>
#include <cstddef>
#include "tool/global.hxx"

namespace scsolver { namespace numeric { namespace lp {

class BaseAlgorithmImpl
{
public:
	BaseAlgorithmImpl();
	~BaseAlgorithmImpl() throw();

	Model* getModel() const { return m_pModel; }
	void setModel( Model* p ) { m_pModel = p; }
	Model* getCanonicalModel();
	
	Matrix getSolution() const { return m_mxSolution; }
	void setSolution( const Matrix& );
	void setCanonicalSolution( const Matrix& );

private:
	Model* m_pModel;						// original model
	::std::auto_ptr<Model> m_pCanonModel;	// canonical model
	Matrix m_mxSolution;

    /* permutation of variable indices to map IDs of canonical model
       those of original model */
    std::list<size_t> m_cnPermVarIndex;

	struct ConstDecVar
	{
		size_t Id;
		double Value;
	};

    /* This list is used to store IDs and values of constant equivalent
       values. */
	std::list<ConstDecVar> m_cnConstDecVarList;

	void initCanonicalModel();
	void initPermIndex();
};

}}}

using scsolver::numeric::Matrix;

scsolver::numeric::lp::BaseAlgorithmImpl::BaseAlgorithmImpl() :
	m_pModel( NULL ),
	m_pCanonModel( static_cast<Model*>(NULL) ),
	m_mxSolution( 0, 0 )
{
}

scsolver::numeric::lp::BaseAlgorithmImpl::~BaseAlgorithmImpl() throw()
{
}

/**
 * Initialize the canonical (simplified) model with all constant equivalent
 * variables taken out.  A variable is declared constant equivalent if its
 * upper and lower bound values are equal.
 */
void scsolver::numeric::lp::BaseAlgorithmImpl::initCanonicalModel()
{
	Debug( "initCanonicalModel" );

	initPermIndex();

	m_pCanonModel.reset( new Model( *getModel() ) );
	m_cnConstDecVarList.empty();
	size_t nColSize = m_pCanonModel->getCostVector().cols();
	size_t nRhsSize = m_pCanonModel->getRhsVector().rows();
	double fObjConst = 0.0;
	std::vector<double> cnRhsConstants( nRhsSize );
	std::vector<size_t> cnColsNuked;
	
	for ( size_t i = 0; i < nColSize; ++i )
	{
		if ( m_pCanonModel->isVarBounded( i, BOUND_LOWER ) && 
			 m_pCanonModel->isVarBounded( i, BOUND_UPPER ) )
		{
			double fLBound = m_pCanonModel->getVarBound( i, BOUND_LOWER );
			double fUBound = m_pCanonModel->getVarBound( i, BOUND_UPPER );
			if ( fLBound == fUBound )
			{
				std::cout << "var " << i << " == " << fLBound << std::endl;

				// This variable is constant-equivalent.  Remove it from 
				// the temporary model.
				ConstDecVar cdv;
				cdv.Id = i;
				cdv.Value = fLBound;
				m_cnConstDecVarList.push_back( cdv );
	
				double fCost = m_pCanonModel->getCostVector().operator()( 0, i );
				fObjConst -= fCost*fLBound;
				cnColsNuked.push_back( i );
				std::cout << "  (equal) fObjConstant = " << fObjConst << std::endl;
				for( size_t nRow = 0; nRow < nRhsSize; ++nRow )
				{
					double f = m_pCanonModel->getConstraint( nRow, i )*fLBound;
					std::cout << "  " << f;
					cnRhsConstants[nRow] -= f;
					std::cout << ":\t" << cnRhsConstants.at( nRow ) << std::endl;
				}
			}
		}
	}

	if ( !cnColsNuked.empty() )
	{
		// Delete designated columns from all affected matrices, and turn them
		// into constant values to be added to the RHS vector and objective function.
		// Also, remove the IDs of deleted columns from the permutation index list, 
		// and update the constant objective value list.

		m_pCanonModel->deleteVariables( cnColsNuked );
		for ( size_t i = 0; i < nRhsSize; ++i )
		{
			double fRhsVal = m_pCanonModel->getRhsValue(i);
			m_pCanonModel->setRhsValue( i, fRhsVal + cnRhsConstants.at(i) );
		}

		// Actually this may not be needed since this value will not affect
		// the solution at all.  But set this anyway.
		m_pCanonModel->setObjectiveFuncConstant( fObjConst );

		std::vector<size_t>::iterator it,
			itBeg = cnColsNuked.begin(), itEnd = cnColsNuked.end();

		// Remove its index from permutation index list.
		std::list<size_t>::iterator itPerm = m_cnPermVarIndex.begin();
		for ( it = itBeg; it != itEnd; ++it )
			m_cnPermVarIndex.remove( *it );
	}
}

scsolver::numeric::lp::Model* scsolver::numeric::lp::BaseAlgorithmImpl::getCanonicalModel()
{
	if ( m_pCanonModel.get() == NULL )
		initCanonicalModel();
	return m_pCanonModel.get();
}

void scsolver::numeric::lp::BaseAlgorithmImpl::setSolution( const Matrix& other )
{
	Matrix m( other );
	m_mxSolution.swap( m );
}

void scsolver::numeric::lp::BaseAlgorithmImpl::setCanonicalSolution( const Matrix& mxCanonSol )
{
	size_t nCostSize = getModel()->getCostVector().cols();
	std::cout << "original cost size is " << nCostSize << std::endl;
	Matrix mxSol( nCostSize, 1 );
	mxSol.setResizable( false );

	// Map solved variables into their original position.
	std::list<size_t>::const_iterator itBeg = m_cnPermVarIndex.begin(),
		itEnd = m_cnPermVarIndex.end(), it;
	for ( it = itBeg; it != itEnd; ++it )
	{
		size_t nSrcId = ::std::distance( itBeg, it );
		size_t nDstId = *it;
		std::cout << "mapped var id: " << nSrcId << " -> " << nDstId << std::endl;
		mxSol( nDstId, 0 ) = mxCanonSol( nSrcId, 0 );
	}

	// Insert constant variables if any
	std::list<ConstDecVar>::const_iterator itCdvBeg = m_cnConstDecVarList.begin(),
		itCdvEnd = m_cnConstDecVarList.end(), itCdv;
	for ( itCdv = itCdvBeg; itCdv != itCdvEnd; ++itCdv )
	{
		std::cout << "constant-equivalent variable: " << itCdv->Id << "\t" << itCdv->Value << std::endl;
		mxSol( itCdv->Id, 0 ) = itCdv->Value;
	}

	m_mxSolution.swap( mxSol );
	m_pCanonModel.reset( static_cast<Model*>(NULL) );
}

/**
 * Initialize the permutation index list.  This list is used to set the
 * values of decision variables back to their original position in case
 * of model reduction.
 */
void scsolver::numeric::lp::BaseAlgorithmImpl::initPermIndex()
{
	m_cnPermVarIndex.clear();
	size_t nCostSize = getModel()->getCostVector().cols();
	for ( size_t i = 0; i < nCostSize; ++i )
		m_cnPermVarIndex.push_back( i );
}


//---------------------------------------------------------------------------
// BaseAlgorithm

scsolver::numeric::lp::BaseAlgorithm::BaseAlgorithm() :
	m_pImpl( new BaseAlgorithmImpl() )
{
}

scsolver::numeric::lp::BaseAlgorithm::~BaseAlgorithm() throw()
{
}

scsolver::numeric::lp::Model* scsolver::numeric::lp::BaseAlgorithm::getModel() const
{
	return m_pImpl->getModel();
}

void scsolver::numeric::lp::BaseAlgorithm::setModel( Model* p )
{
	m_pImpl->setModel( p );
}

scsolver::numeric::lp::Model* scsolver::numeric::lp::BaseAlgorithm::getCanonicalModel() const
{
	return m_pImpl->getCanonicalModel();
}

scsolver::numeric::Matrix scsolver::numeric::lp::BaseAlgorithm::getSolution() const
{
	return m_pImpl->getSolution();
}

void scsolver::numeric::lp::BaseAlgorithm::setSolution( const Matrix& mx )
{
	m_pImpl->setSolution( mx );
}

void scsolver::numeric::lp::BaseAlgorithm::setCanonicalSolution( const Matrix& mx )
{
	m_pImpl->setCanonicalSolution( mx );
}

