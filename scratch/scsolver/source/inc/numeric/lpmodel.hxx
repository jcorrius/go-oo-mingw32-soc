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


#ifndef _LPMODEL_HXX_
#define _LPMODEL_HXX_

#include <numeric/type.hxx>
#include <vector>
#include <memory>

namespace scsolver { namespace numeric {

class Matrix;

}}

namespace scsolver { namespace numeric { namespace opres { namespace lp {


class BaseAlgorithm;
class ModelImpl;


/** This class is responsible for constructing an LP model which is to be solved
	by using an LP algorithm class defined separately.
 */
class Model
{
public:

	Model();
	Model( const Model& );
	~Model();
	
	void print() const;

	::scsolver::numeric::Matrix getCostVector() const;
	void setCostVectorElement( size_t, double );
	void setCostVector( const std::vector<double>& );
	void deleteCostVectorElements( const std::vector<size_t> & );

	double   getVarBound( unsigned long, Bound ) const;
	void     setVarBound( unsigned long, Bound, double );
	bool isVarBounded( unsigned long, Bound ) const;

	Goal getGoal() const;
	void setGoal( Goal );
	
	double getObjectiveFuncConstant() const;
	void setObjectiveFuncConstant( double );

	unsigned long getPrecision() const;
	void       setPrecision( unsigned long );

	bool getVarPositive() const;
	void setVarPositive( bool );

	bool getVerbose() const;
	void setVerbose( bool );

	double getConstraint( size_t, size_t ) const;
	::scsolver::numeric::Matrix getConstraintMatrix() const;
	::scsolver::numeric::Matrix getRhsVector() const;
	double getRhsValue( size_t ) const;
	void setRhsValue( size_t, double );
	std::vector<Equality> getEqualityVector() const;
	Equality getEqualityByRowId( unsigned long ) const;
	void addConstraint( const std::vector<double>&, Equality, double );
	void setStandardConstraintMatrix( const ::scsolver::numeric::Matrix&, const ::scsolver::numeric::Matrix& );
	void deleteConstraintMatrixColumns( const std::vector<size_t>& );
	
	void solve( BaseAlgorithm* );
	void solve( const std::auto_ptr<BaseAlgorithm>& );

	::scsolver::numeric::Matrix getSolution() const;
	void setSolution( const ::scsolver::numeric::Matrix& );

private:

	std::auto_ptr<ModelImpl> m_pImpl;
};


}}}}


#endif
