
#ifndef COMPOUNDSTOPPINGCRITERION_H

#include <otkpp/lib/Cloneable.h>
#include <otkpp/stopcrit/StoppingCriterion.h>
#include <list>

/// Defines a combination of several stopping criteria.
/**
 * This is for testing multiple stopping criteria. 
 * If one of them is satisfied, the result of this 
 * test is positive.
 */
class CompoundStoppingCriterion : public Cloneable< CompoundStoppingCriterion, StoppingCriterion >
{
 public:
  CompoundStoppingCriterion() { }
  CompoundStoppingCriterion(const StoppingCriterion &sc) { }
  ~CompoundStoppingCriterion() { }
  
  CompoundStoppingCriterion operator+(const StoppingCriterion &sc);
  double getTestValue(const NativeSolver &s) const;
  bool test(const NativeSolver &s) const ;
 private:
  std::list< const StoppingCriterion * > stopCrit_;
};

#define COMPOUNDSTOPPINGCRITERION_H

#endif
