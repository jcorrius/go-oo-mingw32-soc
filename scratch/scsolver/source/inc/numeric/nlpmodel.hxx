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

#ifndef _NUMERIC_OPRES_NLP_NLPMODEL_HXX_
#define _NUMERIC_OPRES_NLP_NLPMODEL_HXX_

#include <numeric/matrix.hxx>
#include <numeric/type.hxx>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace scsolver { namespace numeric {
	class BaseFuncObj;
}}

namespace scsolver { namespace numeric { namespace opres { namespace nlp {

class ModelImpl;
class BaseAlgorithm;

class Model
{
public:
	Model();
	Model( const Model& );
	~Model() throw();

	void print() const;

	void setPrecision( unsigned long );
	unsigned long getPrecision() const;

	void setGoal( scsolver::numeric::opres::Goal );
	scsolver::numeric::opres::Goal getGoal() const;

	void setVerbose( bool );
	bool getVerbose() const;

	::scsolver::numeric::Matrix getSolution() const;
	void setSolution( const ::scsolver::numeric::Matrix& );

	void setFuncObject( const boost::shared_ptr<BaseFuncObj>& );
	const boost::shared_ptr<BaseFuncObj> getFuncObject() const;

	void setVariable( const std::vector<double>& );
	const std::vector<double> getVariable() const;

	void solve( const boost::shared_ptr<BaseAlgorithm>& );

private:
	std::auto_ptr<ModelImpl> m_pImpl;
};


}}}}


#endif
