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

#include "numeric/lpuno.hxx"
#include "numeric/lpmodel.hxx"
#include "numeric/matrix.hxx"
#include "xcalc.hxx"
#include "global.hxx"
#include "unoglobal.hxx"

#include "org/openoffice/sc/solver/XLpModel.hpp"
#include "org/openoffice/sc/solver/XLpAlgorithm.hpp"
#include "org/openoffice/sc/solver/Goal.hpp"
#include "org/openoffice/sc/solver/Equality.hpp"

#include <exception>

using namespace ::com::sun::star::uno;

namespace scsolver { namespace numeric { namespace opres { namespace lp {

class UnoServiceNotFound : public ::std::exception
{
	const char* what() const throw() { return "UNO Servcie not found"; }
};

class LpModelNotFound : public ::std::exception
{
	const char* what() const throw() { return "LpModel service not found"; }
};

class UnknownGoal : public ::std::exception
{
	const char* what() const throw() { return "Unknown goal"; }
};

class UnknownConstraintType : public ::std::exception
{
	const char* what() const throw() { return "Unknown constraint type"; }
};

class UnoAlgorithmImpl
{
public:
	UnoAlgorithmImpl( const rtl::OUString& service, CalcInterface* pCalc ) :
		m_sServiceName( service ),
		m_pCalc( pCalc ),
		m_mxSolution( 0, 0 ),
		m_pModel( NULL )
	{
	}

	~UnoAlgorithmImpl() throw() 
	{
	}

	void solve();
	Matrix getSolution() const;

	void setModel( Model* p ) { m_pModel = p; }
	Model* getModel() const { return m_pModel; }

	const rtl::OUString getServiceName() const { return m_sServiceName; }

private:
	rtl::OUString m_sServiceName;
	CalcInterface* m_pCalc;
	Matrix m_mxSolution;
	Model* m_pModel;

	CalcInterface* getCalcInterface() const { return m_pCalc; }
};

/**
 * Given an LP model represented by an object of class
 * lp::Model, create an UNO object (service) of type LpModel to
 * map model parameters to it, and to pass it to another UNO
 * object that provides an LP algorithm to find a solution.
 * 
 * Once a solution is found, set the solution into m_mxSolution
 * which will be later retrieved by the calling function.
 */
void UnoAlgorithmImpl::solve()
{
	using namespace ::org::openoffice::sc::solver;
	using numeric::Matrix;

	static const rtl::OUString sLpModelSrv = ascii( "org.openoffice.sc.solver.LpModel" );

	Reference< uno::XComponentContext >       xCC = getCalcInterface()->getComponentContext();
	Reference< lang::XMultiComponentFactory > xSM = getCalcInterface()->getServiceManager();

	Reference< uno::XInterface > svModel = xSM->createInstanceWithContext( sLpModelSrv, xCC );
	if ( svModel == NULL )
		throw LpModelNotFound();

	Reference< XLpModel > xModel( svModel, UNO_QUERY );

	numeric::opres::lp::Model* model = getModel();

	// Map LP model properties into XLpModel.

	// --- Goal ---

	switch ( model->getGoal() )
	{
	case GOAL_MINIMIZE:
		xModel->setGoal( Goal_MINIMIZE );
		break;
	case GOAL_MAXIMIZE:
		xModel->setGoal( Goal_MAXIMIZE );
		break;
	default:
		throw UnknownGoal();
	}

	// --- Costs ---

	Matrix mxCosts = model->getCostVector();
	size_t nDecVarSize = mxCosts.cols();
	xModel->setDecisionVarSize( nDecVarSize );
	for ( size_t i = 0; i < nDecVarSize; ++i )
		xModel->setCost( i, mxCosts( 0, i ) );

	// --- Constraints ---

	Matrix mxConstraint = model->getConstraintMatrix();
	Matrix mxRhs = model->getRhsVector();

	size_t nConstraintSize = mxConstraint.rows();
	xModel->setConstraintCount( nConstraintSize );
	for ( size_t i = 0; i < nConstraintSize; ++i )
	{
		for ( size_t j = 0; j < nDecVarSize; ++j )
			xModel->setConstraint( i, j, mxConstraint( i, j ) );

		switch ( model->getEqualityByRowId(i) )
		{
		case GREATER_THAN_EQUAL:
			xModel->setEquality( i, Equality_GREATER_EQUAL );
			break;
		case EQUAL:
			xModel->setEquality( i, Equality_EQUAL );
			break;
		case LESS_THAN_EQUAL:
			xModel->setEquality( i, Equality_LESS_EQUAL );
			break;
		default:
			throw UnknownConstraintType();
		}

		xModel->setRhsValue( i, mxRhs( i, 0 ) );
	}

	// Instantiate the UNO algorithm and run it.

	Reference< uno::XInterface > algorithm = xSM->createInstanceWithContext( getServiceName(), xCC );
	if ( algorithm == NULL )
		throw UnoServiceNotFound();

	Reference< XLpAlgorithm > xAlgorithm( algorithm, UNO_QUERY );
	xAlgorithm->setModel( xModel );
	xAlgorithm->solve();

	Matrix mxSol( nDecVarSize, 1 );
	for ( size_t i = 0; i < nDecVarSize; ++i )
		mxSol( i, 0 ) = xAlgorithm->getVar(i);

    m_mxSolution.swap( mxSol );
}

Matrix UnoAlgorithmImpl::getSolution() const
{
	return m_mxSolution;
}




UnoAlgorithm::UnoAlgorithm( const rtl::OUString& service, CalcInterface* pCalc ) :
	m_pImpl( new UnoAlgorithmImpl( service, pCalc ) )
{
}

UnoAlgorithm::~UnoAlgorithm() throw()
{
}

void UnoAlgorithm::solve()
{
	m_pImpl->setModel( getModel() );
	m_pImpl->solve();
	setSolution( m_pImpl->getSolution() );
}

}}}}
