
#include "GSLFDFSolver.h"
#include "GSLUtils.h"

bool GSLFDFSolver_setup::isCompatibleWith(const Solver &s) const
{
  if(typeid(s) == typeid(GSLFDFSolver))
    return true;
  else
    return false;
}

GSLFDFSolver::GSLFDFSolver(const std::string &gslName, Function::DerivEvalType gEvalType) : 
  AbstractGradientSolver(gEvalType)
{
  if(gslName == "conjugate_fr")
    type_ = gsl_multimin_fdfminimizer_conjugate_fr;
  else if(gslName == "conjugate_pr")
    type_ = gsl_multimin_fdfminimizer_conjugate_pr;
  else if(gslName == "vector_bfgs")
    type_ = gsl_multimin_fdfminimizer_vector_bfgs;
  else if(gslName == "vector_bfgs2")
    type_ = gsl_multimin_fdfminimizer_vector_bfgs2;
  else if(gslName == "steepest_descent")
    type_ = gsl_multimin_fdfminimizer_steepest_descent;
  else
    throw std::invalid_argument("invalid GSL minimizer name");
  
  gslSolver_ = NULL;
}

GSLFDFSolver::GSLFDFSolver(const gsl_multimin_fdfminimizer_type *type, Function::DerivEvalType gEvalType) : 
  AbstractGradientSolver(gEvalType)
{
  type_ = type;
  gslSolver_ = NULL;
}

GSLFDFSolver::~GSLFDFSolver()
{
  if(gslSolver_ != NULL)
    gsl_multimin_fdfminimizer_free(gslSolver_);
}

double GSLFDFSolver::getFVal() const
{
  return gslSolver_->f;
}

const vector< double > GSLFDFSolver::getGradient() const
{
  vector< double > g;
  gslutils::GSLToBoostVector(gslSolver_->gradient, g);
  return g;
}

const gsl_multimin_fdfminimizer *GSLFDFSolver::getGSLSolver() const
{
  return gslSolver_;
}

std::string GSLFDFSolver::getName() const
{
  return type_->name;
}

const vector< double > GSLFDFSolver::getX() const
{
  vector< double > x;
  gslutils::GSLToBoostVector(gslSolver_->x, x);
  return x;
}

NativeSolver::IterationStatus GSLFDFSolver::iterate_()
{
  int status = gsl_multimin_fdfminimizer_iterate(gslSolver_);
  
  // TODO: error codes
  if(status == GSL_ENOPROG)
    return NativeSolver::ITERATION_NO_PROGRESS;
  else
    return NativeSolver::ITERATION_CONTINUE;
}

void GSLFDFSolver::setup_(const Function &objFunc,
                          const vector< double > &x0,
                          const SolverSetup &solverSetup,
                          const Constraints &C)
{
  const int n = objFunc.getN();
  double stepSize, tol;
  
  AbstractGradientSolver::setup_(objFunc, x0, solverSetup, C);
  
  if(typeid(solverSetup) == typeid(DefaultSolverSetup))
  {
    stepSize = 1.0;
    tol = 0.1;
  }
  else
  {
    stepSize = dynamic_cast< const GSLFDFSolver_setup & >(solverSetup).stepSize;
    tol = dynamic_cast< const GSLFDFSolver_setup & >(solverSetup).tol;
  }
  
  if(gslSolver_ != NULL)
    gsl_multimin_fdfminimizer_free(gslSolver_);
  gslSolver_ = gsl_multimin_fdfminimizer_alloc(type_, n);
  
  gslFunction_.n      = n;
  gslFunction_.params = NULL;
  gslFunction_.f      = &gslutils::f;
  // by using a pointer-to-member it would be something like this:
  // (double (*)(const gsl_vector *, void *))(gslWrapper_.*(&GSLWrapper::f));
  gslFunction_.df     = &gslutils::df;
  gslFunction_.fdf    = &gslutils::fdf;
  
  gslutils::setFunction(objFunc_);
  gsl_vector *x0_ = gsl_vector_alloc(n);
  gslutils::BoostToGSLVector(x0, x0_);
  gsl_multimin_fdfminimizer_set(gslSolver_, &gslFunction_, x0_, stepSize, tol);
  gsl_vector_free(x0_);
}

bool GSLFDFSolver::usesGradient() const
{
  return true;
}

bool GSLFDFSolver::usesHessian() const
{
  return false;
}
