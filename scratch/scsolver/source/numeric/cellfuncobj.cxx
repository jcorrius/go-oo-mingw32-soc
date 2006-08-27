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

#include "numeric/cellfuncobj.hxx"
#include "xcalc.hxx"
#include "unoglobal.hxx"
#include "com/sun/star/table/CellAddress.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <stdio.h>

using namespace std;
using com::sun::star::table::CellAddress;

namespace scsolver { namespace numeric {

struct CellFuncObjImpl
{
	CalcInterface* pCalc;
	CellAddress TargetCell;
	vector<CellAddress> DecVarCells;
};

//-----------------------------------------------------------------

/*
CellFuncObj::CellFuncObj()
{
	// disabled
}
*/

CellFuncObj::CellFuncObj( CalcInterface* pCalc ) :
	m_pImpl( new CellFuncObjImpl )
{
	m_pImpl->pCalc = pCalc;
}

CellFuncObj::~CellFuncObj() throw()
{
}

double CellFuncObj::operator()( const vector<double>& cnX ) const
{
	if( cnX.size() != m_pImpl->DecVarCells.size() )
		fprintf( stderr, "CellFuncObj: warning - values and cells have different sizes\n" );

	vector<CellAddress>::const_iterator it, 
		itBeg = m_pImpl->DecVarCells.begin(), 
		itEnd = m_pImpl->DecVarCells.end();

	vector<double>::const_iterator itX,
		itXBeg = cnX.begin(),
		itXEnd = cnX.end();

	for( it = itBeg, itX = itXBeg; it != itEnd && itX != itXEnd; ++it, ++itX )
		m_pImpl->pCalc->setCellValue( *it, *itX );

	return m_pImpl->pCalc->getCellValue( m_pImpl->TargetCell );
}

std::string CellFuncObj::getFuncString() const
{
	ostringstream os;
	int sheet = m_pImpl->TargetCell.Sheet;
	int column = m_pImpl->TargetCell.Column;
	int row = m_pImpl->TargetCell.Row;
	os << "(" << sheet << ", " << column << ", " << row << ")";

	return os.str();
}

void CellFuncObj::setTargetCell( const CellAddress& addr )
{
	m_pImpl->TargetCell = addr;
}

void CellFuncObj::appendDecVarCell( const CellAddress& addr )
{
	m_pImpl->DecVarCells.push_back(addr);
}

void CellFuncObj::test()
{
	CalcInterface* p = m_pImpl->pCalc;
	p->setCellFormula( m_pImpl->TargetCell, ascii("I got it") );

	vector<CellAddress>::iterator it,
		itBeg = m_pImpl->DecVarCells.begin(),
		itEnd = m_pImpl->DecVarCells.end();
	for ( it = itBeg; it != itEnd; ++it )
		p->setCellFormula( *it, ascii("dec var") );
}

}}

