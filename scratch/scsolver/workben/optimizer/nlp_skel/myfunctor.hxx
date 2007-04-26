#ifndef _SCSOLVER_NUMERIC_MYFUNCTOR_
#define _SCSOLVER_NUMERIC_MYFUNCTOR_

#include "numeric/funcobj.hxx"
#include <vector>

namespace scsolver {

namespace numeric { 

namespace nlp {

class MyFunctor : public BaseFuncObj
{
public:
    MyFunctor();
    ~MyFunctor() throw();

    virtual double eval();

    /**
     * Return a display-friendly function string.
     *
     * @return std::string
     */
    virtual std::string getFuncString() const;

    /**
     * Returns a read-only reference to an array of current variable values.
     * 
     * @return const::std::vector<double>&
     */
    virtual const ::std::vector<double>& getVars() const;

    virtual void setVar(size_t index, double var);

    virtual void setVars(const std::vector<double> &vars);

private:
    ::std::vector<double> mVars;
};

} // namespace nlp 
} // namespace numeric
} // namespace scsolver

#endif
