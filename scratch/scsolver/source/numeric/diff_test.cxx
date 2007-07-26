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

#include "numeric/diff.hxx"
#include "numeric/funcobj.hxx"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>

using namespace std;
using namespace scsolver::numeric;

class TestFailed {};

class BaseTestFunc : public BaseFuncObj
{
public:
    virtual ~BaseTestFunc() throw() {}
    virtual double evalDef1() const = 0;
    virtual double evalDef2() const = 0;
};

class TestFunc : public BaseTestFunc
{
public:

    virtual ~TestFunc() throw()
    {
    }

	virtual const string getFuncString() const
	{
		return string("f(x) = 2x^2 + 5x");
	}

    virtual const vector<double>& getVars() const
    {
        return m_Vars;
    }

    virtual void setVars(const vector<double>& vars)
    {
        vector<double> tmp(vars);
        m_Vars.swap(tmp);
    }

    virtual void setVar(size_t index, double var)
    {
        m_Vars.at(index) = var;
    }

    virtual double eval()
    {
		double x = m_Vars.at(0);
        return (2*x + 5)*x;
    }

    virtual double evalDef1() const
    {
        double x = m_Vars.at(0);
        return 4*x + 5;
    }

    virtual double evalDef2() const
    {
        return 4;
    }

private:
    vector<double> m_Vars;
};

class TestFunc2 : public BaseTestFunc
{
public:
    virtual ~TestFunc2() throw()
    {
    }

	virtual const string getFuncString() const
	{
		return string("f(x) = 2x^3 + 5x^2 - 2x");
	}

	virtual double eval()
	{
        // ( (2*x + 5) * x - 2)*x
		double x = m_Vars.at( 0 );
        double f = ((2*x + 5)*x - 2)*x;
        return f;
	}

    virtual double evalDef1() const
    {
        // 6x^2 + 10x - 2 = (6x + 10) * x - 2
        double x = m_Vars.at(0);
        double f = (6*x + 10) * x - 2;
        return f;
    }

    virtual double evalDef2() const
    {
        double x = m_Vars.at(0);
        return 12*x + 10;
    }

    virtual const vector<double>& getVars() const
    {
        return m_Vars;
    }

    virtual void setVars(const vector<double>& vars)
    {
        vector<double> tmp(vars);
        m_Vars.swap(tmp);
    }

    virtual void setVar(size_t index, double var)
    {
        m_Vars.at(index) = var;
    }

private:
    vector<double> m_Vars;
};

void checkDelta(double delta)
{
    cout << "delta = " << delta << endl;
    if (delta > 0.05)
        throw TestFailed();
}

void test(BaseTestFunc* pF)
{
    static const double vars[] = {0, 1.5, 10.0, 20.5, 50.0};
    for (int i = 0; i < 5; ++i)
    {
        double var = vars[i];
        cout << "----------------------------------------" << endl;
        cout << " x = " << var << endl;
        vector<double> cnX;
        cnX.push_back(var);
        auto_ptr<NumericalDiffer> pDiff( new NumericalDiffer );
        pDiff->setVariables( cnX );
        pDiff->setPrecision(5);
        pDiff->setFuncObject(pF);
        double answer = pDiff->run();
        double delta = fabs(answer - pF->evalDef1())/fabs(answer);
        checkDelta(delta);
        
        pDiff->setSecondOrder(true);
        answer = pDiff->run();
        delta = (answer - pF->evalDef2())/answer;
        checkDelta(delta);
    }
}

int main()
{
    try
    {
        auto_ptr<TestFunc> pF( new TestFunc );
        test(pF.get());
        auto_ptr<TestFunc2> pF2( new TestFunc2 );
        test(pF2.get());
        cout << "Test passed!" << endl;
    }
    catch (const TestFailed& )
    {
        cout << "Test failed" << endl;
    }
}
















