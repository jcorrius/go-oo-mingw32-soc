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

#ifndef OPRES_NLP_NLPBASE_HXX_
#define OPRES_NLP_NLPBASE_HXX_

#include <vector>
#include <memory>

namespace scsolver { namespace numeric {

namespace nlp {

class Model;
class BaseAlgorithmImpl;

class BaseAlgorithm
{
public:
	BaseAlgorithm();
	virtual ~BaseAlgorithm() throw() = 0;

    /**
     * Call this method to solve for optimized solution.
     */
	virtual void solve() = 0;

    /**
     * Set NLP model to be optimized.  <b>Note that this class does NOT manage the
     * life cycle of this model instance;</b> it is the responsibility of the 
     * calling function to ensure that it gets deleted after the optimization code 
     * has run. 
     *  
     * @param model pointer to NLP model.
     */
	void setModel( Model* model );

    /**
     * Get the optimized solution.  Call this method after solve() returns.
     * 
     * @return ::std::vector<double> optimized solution
     */
	const ::std::vector<double>& getSolution() const;

protected:
    /**
     * Get current NLP model.
     * 
     * @return Model* pointer to current model.
     */
	Model* getModel() const;

private:
	::std::auto_ptr<BaseAlgorithmImpl> m_pImpl;
};

}}}


#endif
