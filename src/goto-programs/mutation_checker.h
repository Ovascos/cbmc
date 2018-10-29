/*******************************************************************\

Module: Safety Checker Interface

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Safety Checker Interface

#ifndef CPROVER_GOTO_PROGRAMS_MUTATION_CHECKER_H
#define CPROVER_GOTO_PROGRAMS_MUTATION_CHECKER_H

// this is just an interface -- it won't actually do any checking!

#include <util/invariant.h>
#include <util/message.h>

#include "goto_trace.h"
#include "goto_functions.h"

class mutation_checkert:public messaget
{
public:
  explicit mutation_checkert(
    const namespacet &_ns);

  explicit mutation_checkert(
    const namespacet &_ns,
    message_handlert &_message_handler);

  enum class resultt
  {
    /// Mutation is equal
    EQUAL,
    /// Mutation can be killed with a testcase
    KILLED,
    /// Safety is unknown due to an error during safety checking
    ERROR,
    /// We haven't yet assigned a safety check result to this object. A value of
    /// UNKNOWN can be used to initialize a resultt object, and that object may
    /// then safely be used with the |= and &= operators.
    UNKNOWN
  };

  // check whether all assertions in goto_functions are safe
  // if UNSAFE, then a trace is returned

  virtual resultt operator()(
    const goto_functionst &goto_functions)=0;

  // this is the counterexample
  goto_tracet error_trace;

protected:
  // the namespace
  const namespacet &ns;
};

/// \brief The worst of two results
inline mutation_checkert::resultt &
operator&=(mutation_checkert::resultt &a, mutation_checkert::resultt const &b)
{
  switch(a)
  {
  case mutation_checkert::resultt::UNKNOWN:
    a = b;
    return a;
  case mutation_checkert::resultt::ERROR:
    return a;
  case mutation_checkert::resultt::EQUAL:
    a = b;
    return a;
  case mutation_checkert::resultt::KILLED:
    a = b == mutation_checkert::resultt::ERROR ? b : a;
    return a;
  }
  UNREACHABLE;
}

/// \brief The best of two results
inline mutation_checkert::resultt &
operator|=(mutation_checkert::resultt &a, mutation_checkert::resultt const &b)
{
  switch(a)
  {
  case mutation_checkert::resultt::UNKNOWN:
    a = b;
    return a;
  case mutation_checkert::resultt::EQUAL:
    return a;
  case mutation_checkert::resultt::ERROR:
    a = b;
    return a;
  case mutation_checkert::resultt::KILLED:
    a = b == mutation_checkert::resultt::EQUAL ? b : a;
    return a;
  }
  UNREACHABLE;
}
#endif // CPROVER_GOTO_PROGRAMS_MUTATION_CHECKER_H
