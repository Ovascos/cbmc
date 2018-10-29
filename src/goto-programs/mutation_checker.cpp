/*******************************************************************\

Module: Mutation Checker Interface

\*******************************************************************/

/// \file
/// Mutation Checker Interface

#include "mutation_checker.h"

mutation_checkert::mutation_checkert(const namespacet &_ns):
  ns(_ns)
{
}

mutation_checkert::mutation_checkert(
  const namespacet &_ns,
  message_handlert &_message_handler):
  messaget(_message_handler),
  ns(_ns)
{
}
