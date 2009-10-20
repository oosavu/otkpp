
#include "AbstractGradientSolver.h"

AbstractGradientSolver::AbstractGradientSolver(Function::DerivEvalType gEvalType) : 
  gEvalType_(gEvalType) { }

void AbstractGradientSolver::setup_(const Function &objFunc,
                                    const vector< double > &x0,
                                    const SolverSetup &solverSetup,
                                    const Constraints &C)
{
  if(gEvalType_ != Function::DERIV_SYMBOLIC)
    objFunc_ = objFunc.createCopy(gEvalType_);
  NativeSolver::setup_(objFunc_, x0, solverSetup);
}
