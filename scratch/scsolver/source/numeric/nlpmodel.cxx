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

#include <numeric/nlpmodel.hxx>
#include <numeric/nlpbase.hxx>
#include <numeric/funcobj.hxx>
#include <global.hxx>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>

using namespace ::scsolver::numeric;
using namespace std;

namespace scsolver { namespace numeric { namespace opres { namespace nlp {

//---------------------------------------------------------------------------
// IMPLEMENTATION

class ModelImpl
{
public:
	ModelImpl( Model* );
	ModelImpl( const ModelImpl& );
	~ModelImpl() throw();

	void setPrecision( unsigned long n ) { m_nPrec = n; }
	unsigned long getPrecision() const { return m_nPrec; }

	void setGoal( opres::Goal e ) { m_eGoal = e; }
	opres::Goal getGoal() const { return m_eGoal; }

	void setVerbose( bool b ) { m_bVerbose = b; }
	bool getVerbose() const { return m_bVerbose; }

	void setFuncObject( const boost::shared_ptr<BaseFuncObj>& );
	boost::shared_ptr<BaseFuncObj> getFuncObject() const { return m_pFuncObj; }

	void solve( const boost::shared_ptr<BaseAlgorithm>& );

	Matrix getSolution() const { return m_mxSolution; }
	void setSolution( const Matrix& );

	void setVariable( const std::vector<double>& );
	const std::vector<double> getVariable() const;

	void print() const;

private:
	Model* m_pSelf;
	unsigned long m_nPrec;
	opres::Goal m_eGoal;
	bool m_bFuncObjSet;
	bool m_bVerbose;
	Matrix m_mxSolution;
	std::vector<double> m_cnVars;

	boost::shared_ptr<BaseFuncObj> m_pFuncObj;
};

ModelImpl::ModelImpl( Model* p ) : 
	m_pSelf( p ),
	m_nPrec( 9 ), 
	m_eGoal( opres::GOAL_UNKNOWN ), 
	m_bFuncObjSet( false ), 
	m_bVerbose( false ), 
	m_mxSolution( 0, 0 )
{
}

ModelImpl::ModelImpl( const ModelImpl& other ) : 
	m_nPrec( other.m_nPrec ),
	m_pFuncObj( other.m_pFuncObj )
{
}

ModelImpl::~ModelImpl() throw()
{
}

void ModelImpl::setFuncObject( const boost::shared_ptr<BaseFuncObj>& obj )
{
	m_pFuncObj = obj;
	m_bFuncObjSet = true;
}

void ModelImpl::solve( const boost::shared_ptr<BaseAlgorithm>& pAlgorithm )
{
	pAlgorithm->setModel( m_pSelf );
	pAlgorithm->solve();
	setSolution( pAlgorithm->getSolution() );
}

void ModelImpl::setSolution( const Matrix& other )
{
	Matrix m( other );
	m_mxSolution.swap( m );
}

void ModelImpl::setVariable( const std::vector<double>& cn )
{
	vector<double> cnVars( cn.begin(), cn.end() );
	swap( m_cnVars, cnVars );
}

const vector<double> ModelImpl::getVariable() const
{
	return m_cnVars;
}

void ModelImpl::print() const
{
	cout << repeatString( "-", 70 ) << endl;

	cout << "objective: ";
	if ( m_bFuncObjSet )
		cout << m_pFuncObj->getFuncString() << endl;
	else
		cout << "objective function not set!" << endl;

	cout << "goal: ";
	switch ( m_eGoal )
	{
	case opres::GOAL_MAXIMIZE:
		cout << "maximize";
		break;
	case opres::GOAL_MINIMIZE:
		cout << "minimize";
		break;
	default:
		cout << "unknown";
	}
	cout << endl;

	cout << repeatString( "-", 70 ) << endl;
}

//---------------------------------------------------------------------------
// ENVELOPE

Model::Model() : m_pImpl( new ModelImpl( this ) )
{
}

Model::Model( const Model& other ) : m_pImpl( new ModelImpl( *other.m_pImpl.get() ) )
{
}

Model::~Model() throw()
{
}

void Model::print() const
{
	m_pImpl->print();
}

void Model::setPrecision( unsigned long n )
{
	m_pImpl->setPrecision( n );
}

unsigned long Model::getPrecision() const
{
	return m_pImpl->getPrecision();
}

void Model::setGoal( opres::Goal e )
{
	m_pImpl->setGoal( e );
}

opres::Goal Model::getGoal() const
{
	return m_pImpl->getGoal();
}

void Model::setVerbose( bool b )
{
	m_pImpl->setVerbose( b );
}

bool Model::getVerbose() const
{
	return m_pImpl->getVerbose();
}

Matrix Model::getSolution() const
{
	return m_pImpl->getSolution();
}

void Model::setSolution( const Matrix& mx )
{
	m_pImpl->setSolution( mx );
}

void Model::setFuncObject( const boost::shared_ptr<BaseFuncObj>& obj )
{
	m_pImpl->setFuncObject( obj );
}

const boost::shared_ptr<BaseFuncObj> Model::getFuncObject() const
{
	return m_pImpl->getFuncObject();
}

void Model::setVariable( const std::vector<double>& cn )
{
	m_pImpl->setVariable( cn );
}

const std::vector<double> Model::getVariable() const
{
	return m_pImpl->getVariable();
}

void Model::solve( const boost::shared_ptr<BaseAlgorithm>& obj )
{
	m_pImpl->solve( obj );
}

}}}}

