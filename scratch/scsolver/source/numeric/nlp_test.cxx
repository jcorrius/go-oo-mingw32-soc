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
#include <numeric/funcobj.hxx>
#include <numeric/nlpnewton.hxx>
#include <numeric/matrix.hxx>
#include <numeric/type.hxx>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using ::scsolver::numeric::Matrix;
using ::scsolver::numeric::BaseFuncObj;
using namespace ::scsolver::numeric::opres;
using namespace ::scsolver::numeric::opres::nlp;
using namespace boost;
using namespace std;

class TestFunc : public BaseFuncObj
{
public:
	TestFunc() {}
	virtual ~TestFunc() throw() {}

	virtual double operator()( const vector<double>& cnX ) const
	{
		double x = cnX.at( 0 );
		return cos( x + 1.3 ) + sin( x )*cos( x + 1.5 + sin( x ) );
	}

	virtual string getFuncString() const
	{
		string s = "f(x) = cos(x + 1.3) + sin(x)*cos(x + 1.5 + sin(x))";
		return s;
	}
};

class TestFunc2 : public BaseFuncObj
{
public:
	virtual ~TestFunc2() throw() {}

	virtual double operator()( const vector<double>& cnX ) const
	{
		double x = cnX.at( 0 );
		return -1*x*x + 2;
	}

	virtual string getFuncString() const
	{
		string s = "f(x) = -x^2 + 2";
		return s;
	}
};

class TestFunc3 : public BaseFuncObj
{
};

void testModel1()
{
	shared_ptr<Model> p( new Model() );
	shared_ptr<BaseFuncObj> oF( new TestFunc() );
	shared_ptr<QuasiNewton> pAlgorithm( new QuasiNewton );
	vector<double> cnX;
	cnX.push_back( 1.0 );
	p->setVariable( cnX );
	p->setPrecision( 5 );
	p->setVerbose( true );
	p->setFuncObject( oF );
	p->setGoal( GOAL_MINIMIZE );
	p->print();
	p->solve( pAlgorithm );
	Matrix sol = p->getSolution();
	sol.print();
}

void testModel2()
{
	shared_ptr<Model> p( new Model() );
	shared_ptr<BaseFuncObj> oF( new TestFunc2() );
	shared_ptr<QuasiNewton> pAlgorithm( new QuasiNewton );
	vector<double> cnX;
	cnX.push_back( 2.0 );
	p->setVariable( cnX );
	p->setPrecision( 10 );
	p->setVerbose( true );
	p->setFuncObject( oF );
	p->setGoal( GOAL_MAXIMIZE );
	p->print();
	p->solve( pAlgorithm );
	Matrix sol = p->getSolution();
	sol.print();
}

int main()
{
	testModel1();
	testModel2();
}
















