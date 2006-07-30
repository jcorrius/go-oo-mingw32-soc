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

#include "numeric/nlpbase.hxx"

using ::scsolver::numeric::Matrix;

namespace scsolver { namespace numeric { namespace opres { namespace nlp {

struct BaseAlgorithmImpl
{
	Model* pModel;
	Matrix Solution;
};

BaseAlgorithm::BaseAlgorithm() : m_pImpl( new BaseAlgorithmImpl )
{
}

BaseAlgorithm::~BaseAlgorithm() throw()
{
}

Model* BaseAlgorithm::getModel() const
{
	return m_pImpl->pModel;
}

void BaseAlgorithm::setModel( Model* p )
{
	m_pImpl->pModel = p;
}

Matrix BaseAlgorithm::getSolution() const
{
	return m_pImpl->Solution;
}

void BaseAlgorithm::setSolution( const Matrix& other )
{
	Matrix m(other);
	m_pImpl->Solution.swap(m);
}

}}}}

