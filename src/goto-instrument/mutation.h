#ifndef CPROVER_CBMC_MUTATION_H
#define CPROVER_CBMC_MUTATION_H

#include <util/expr.h>
#include <memory>

enum mutation_typet
{
  LESS_EQUAL_TO_UNEQUAL,
  PLUS_ONE_REMOVE
};

struct mutationt
{
  virtual bool check_pattern(const exprt& ex) const = 0;
  static std::unique_ptr<mutationt> factory(mutation_typet type);
};

#endif // CPROVER_CBMC_MUTATION_H

