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

#ifndef _SCSOLVER_NLPBUILDER_HXX_
#define _SCSOLVER_NLPBUILDER_HXX_

#include <memory>

namespace com { namespace sun { namespace star { namespace table {
	class CellAddress;
}}}}

namespace scsolver {

class SolverImpl;

namespace numeric { 

    class CellFuncObj;

namespace nlp {

	class Model;

}}

class NlpModelBuilderImpl;

class NlpModelBuilder
{
public:
	NlpModelBuilder( SolverImpl* p );
	~NlpModelBuilder() throw();

    void setFuncObj(scsolver::numeric::CellFuncObj* p);
	void setObjectiveFormulaAddress( ::com::sun::star::table::CellAddress addr );
	void clearDecVarAddresses();
	void appendDecVarAddress( ::com::sun::star::table::CellAddress addr );
	numeric::nlp::Model getModel() const;

private:
	NlpModelBuilder();
	std::auto_ptr<NlpModelBuilderImpl> m_pImpl;
};

}

#endif
