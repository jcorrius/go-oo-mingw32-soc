#ifndef _SCSOLVER_NUMERIC_MYOPTIMIZER_
#define _SCSOLVER_NUMERIC_MYOPTIMIZER_

#include "numeric/nlpbase.hxx"

namespace scsolver {

namespace numeric { 

namespace nlp {

class MyOptimizer : public BaseAlgorithm
{
public:
    MyOptimizer();
    virtual ~MyOptimizer() throw();

    virtual void solve();
};

} // namespace nlp 
} // namespace numeric
} // namespace scsolver

#endif
