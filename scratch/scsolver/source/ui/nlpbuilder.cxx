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

#include "nlpbuilder.hxx"
#include "solver.hxx"
#include "xcalc.hxx"
#include "numeric/nlpmodel.hxx"
#include "numeric/cellfuncobj.hxx"

#include "com/sun/star/table/CellAddress.hpp"

#include <list>
#include <stdio.h>

using std::list;
using scsolver::numeric::nlp::Model;
using scsolver::numeric::CellFuncObj;
using com::sun::star::table::CellAddress;

namespace scsolver {

class NlpModelBuilderImpl
{
public:
	NlpModelBuilderImpl( SolverImpl* p ) :
		m_pSolverImpl(p),
        m_pFuncObj(NULL)
	{
	}

	~NlpModelBuilderImpl() throw()
	{
	}

	Model getModel()
	{
		Model model;

        if (m_pFuncObj)
        {
            m_pFuncObj->setTargetCell(m_ObjFormAddr);
            model.setFuncObject(m_pFuncObj);
            CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
            list<CellAddress>::iterator it,
                itBeg = m_cnDecVarAddr.begin(),
                itEnd = m_cnDecVarAddr.end();
            for (it = itBeg; it != itEnd; ++it)
            {
                model.pushVar(pCalc->getCellValue(*it));
                m_pFuncObj->appendDecVarCell(*it);
            }
            model.print();
        }

		return model;
	}

	void setObjectiveFormulaAddress( CellAddress addr )
	{
		m_ObjFormAddr = addr;
	}

	void clearDecVarAddresses()
	{
		m_cnDecVarAddr.clear();
	}

	void appendDecVarAddress( CellAddress addr )
	{
		m_cnDecVarAddr.push_back(addr);
	}

    void setFuncObj(CellFuncObj* p)
    {
        m_pFuncObj = p;
    }

private:
	SolverImpl* m_pSolverImpl;
	CellAddress m_ObjFormAddr;
	list<CellAddress> m_cnDecVarAddr;
    CellFuncObj* m_pFuncObj;
};

//-----------------------------------------------------------------

NlpModelBuilder::NlpModelBuilder( SolverImpl* p ) :
	m_pImpl( new NlpModelBuilderImpl(p) )
{
	fprintf( stderr, "NlpModelBuilder ctor\n" );
}

NlpModelBuilder::~NlpModelBuilder() throw()
{
	fprintf( stderr, "NlpModelBuilder dtor\n" );
}

void NlpModelBuilder::setFuncObj(CellFuncObj* p)
{
    m_pImpl->setFuncObj(p);
}

void NlpModelBuilder::setObjectiveFormulaAddress( CellAddress addr )
{
	m_pImpl->setObjectiveFormulaAddress(addr);
}

void NlpModelBuilder::clearDecVarAddresses()
{
	m_pImpl->clearDecVarAddresses();
}

void NlpModelBuilder::appendDecVarAddress( CellAddress addr )
{
	m_pImpl->appendDecVarAddress(addr);
}

Model NlpModelBuilder::getModel() const
{
	return m_pImpl->getModel();
}

}
