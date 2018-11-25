#include "symex_target_merge_equation.h"

#include <util/expr.h>
#include <util/ssa_expr.h>

#include <set>
#include <list>
#include <string>

#include <iostream>

struct ssa_find_visitort : public const_expr_visitort
{
  symex_target_merge_equationt::ssa_storet &store;

  ssa_find_visitort(symex_target_merge_equationt::ssa_storet & _store)
    : store(_store)
  {}

  virtual void operator()(const exprt &ex)
  {
    if(ex.is_nil())
      return;
    if(!is_ssa_expr(ex))
      return;

    const ssa_exprt &ssa_ex = to_ssa_expr(ex);
    store.emplace(ssa_ex.get_identifier(), ssa_ex);
  }
};

void symex_target_merge_equationt::check(ssa_storet &ssa_exprs, const exprt &ex)
{
  if(ex.is_nil())
    return;
  ssa_find_visitort visitor(ssa_exprs);
  ex.visit(visitor);
}

void symex_target_merge_equationt::find_ssa_exprs(ssa_storet &ssa_exprs)
{
  for(const SSA_stept &step : equation.SSA_steps)
  {
    check(ssa_exprs, step.guard);
    check(ssa_exprs, step.ssa_lhs);
    check(ssa_exprs, step.ssa_full_lhs);
    check(ssa_exprs, step.original_full_lhs);
    check(ssa_exprs, step.ssa_rhs);
    check(ssa_exprs, step.cond_expr);
    for(const exprt &ex : step.io_args)
      check(ssa_exprs, ex);
    for(const exprt &ex : step.ssa_function_arguments)
      check(ssa_exprs, ex);
  }
}

void symex_target_merge_equationt::merge(unsigned prefix)
{
  ssa_storet ssa_exprs;
  find_ssa_exprs(ssa_exprs);

  irep_idt pref(std::to_string(prefix));

  for(auto ex : ssa_exprs)
  {
    if(ex.second.get_prefix() != pref)
      continue;

    ssa_storet::const_iterator it =
      ssa_exprs.find(ex.second.get_prefixfree_object_identifier());

    if(it == ssa_exprs.cend())
      continue;

    // match found
    std::cout << ex.second.get_identifier() << " <-> " << it->second.get_identifier() << std::endl;
  }
}

void symex_target_merge_equationt::merge(unsigned prefix1, unsigned prefix2)
{
  ssa_storet ssa_exprs;
  find_ssa_exprs(ssa_exprs);

}

