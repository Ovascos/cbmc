#include "symex_target_merge_equation.h"

#include <util/expr.h>
#include <util/ssa_expr.h>
#include <util/invariant.h>

#include <unordered_set>
#include <iostream>

struct symex_target_merge_equationt::ssa_rename_visitort
  : public expr_visitort
{
  symex_target_merge_equationt &eq;
  ssa_renamingst &rename;

  ssa_rename_visitort(symex_target_merge_equationt &_eq,ssa_renamingst &_rename)
    : eq(_eq),
      rename(_rename)
  {}

  virtual void operator()(exprt &ex)
  {
    if(ex.is_nil())
      return;
    if(!is_ssa_expr(ex))
      return;

    ssa_exprt &ssa_ex = to_ssa_expr(ex);
    INVARIANT(eq.ssa_store[eq.prefix].count(ssa_ex.get_identifier()) > 0,
        "SSA violation");

    if(eq.prefix != NO_PREFIX)
    {
      const ssa_symbolst &no_prefix = eq.ssa_store[NO_PREFIX];
      if(no_prefix.count(ssa_ex.get_identifier()) > 0)
        rename.insert(ssa_ex.get_identifier()); 

      ssa_ex.set_prefix(eq.prefix);
    }
  }
};

// ToDo perform bettern renaming (no assumes)
void symex_target_merge_equationt::check(ssa_renamingst &renamings, exprt &ex)
{
  if(ex.is_nil())
    return;
  ssa_rename_visitort visitor(*this, renamings);
  ex.visit(visitor);
}

void symex_target_merge_equationt::rename(SSA_stept &step)
{
  ssa_renamingst new_ren;

  if(step.ssa_lhs.is_not_nil())
    ssa_store[prefix].emplace(step.ssa_lhs.get_identifier(), step.ssa_lhs);

  check(new_ren, step.guard);
  check(new_ren, step.ssa_lhs);
  check(new_ren, step.ssa_full_lhs);
  check(new_ren, step.original_full_lhs);
  check(new_ren, step.ssa_rhs);
  check(new_ren, step.cond_expr);
  for(exprt &ex : step.io_args)
    check(new_ren, ex);
  for(exprt &ex : step.ssa_function_arguments)
    check(new_ren, ex);

  perform_renamings(step, new_ren);
}

void symex_target_merge_equationt::perform_renamings(
    const SSA_stept &step, const ssa_renamingst &renamings)
{
  for(irep_idt id : renamings)
  {
    // nothing to do if there is no no-prefix version
    if(ssa_store[NO_PREFIX].count(id) == 0)
      continue;
    // nothing to do if already renamed
    if(ssa_renamings[prefix].count(id) != 0)
      continue;

    ssa_renamings[prefix].insert(id);
    
    INVARIANT(ssa_store[NO_PREFIX].count(id) > 0, "Didn't find original SSA");
    INVARIANT(ssa_store[prefix].count(id) > 0, "Didn't find to be renamed SSA");

    const ssa_exprt &orig = ssa_store[NO_PREFIX][id];
    ssa_exprt ren = ssa_store[prefix][id];
    ren.set_prefix(prefix);

    std::cout << orig.get_identifier() << "<->" << ren.get_identifier() << std::endl;
    //symex_target_equationt::assumption();
  }
}

void symex_target_merge_equationt::set_prefix(int _prefix)
{
  PRECONDITION(_prefix >= 0);
  prefix = _prefix;
}


//
// delegates for ssa_exprt renaming, they call will call the functions 
// they're overloading
//

/// read from a shared variable
void symex_target_merge_equationt::shared_read(
  const exprt &guard,
  const ssa_exprt &ssa_object,
  unsigned atomic_section_id,
  const sourcet &source)
{
  symex_target_equationt::shared_read(
    guard,
    ssa_object,
    atomic_section_id,
    source);

  rename(SSA_steps.back());
}

/// write to a sharedvariable
void symex_target_merge_equationt::shared_write(
  const exprt &guard,
  const ssa_exprt &ssa_object,
  unsigned atomic_section_id,
  const sourcet &source)
{
  symex_target_equationt::shared_write(
    guard,
    ssa_object,
    atomic_section_id,
    source);

  rename(SSA_steps.back());
}

/// spawn a new thread
void symex_target_merge_equationt::spawn(
  const exprt &guard,
  const sourcet &source)
{
  symex_target_equationt::spawn(
    guard,
    source);

  rename(SSA_steps.back());
}

void symex_target_merge_equationt::memory_barrier(
  const exprt &guard,
  const sourcet &source)
{
  symex_target_equationt::memory_barrier(
    guard,
    source);

  rename(SSA_steps.back());
}

/// start an atomic section
void symex_target_merge_equationt::atomic_begin(
  const exprt &guard,
  unsigned atomic_section_id,
  const sourcet &source)
{
  symex_target_equationt::atomic_begin(
    guard,
    atomic_section_id,
    source);

  rename(SSA_steps.back());
}

/// end an atomic section
void symex_target_merge_equationt::atomic_end(
  const exprt &guard,
  unsigned atomic_section_id,
  const sourcet &source)
{
  symex_target_equationt::atomic_end(
    guard,
    atomic_section_id,
    source);

  rename(SSA_steps.back());
}

/// write to a variable
void symex_target_merge_equationt::assignment(
  const exprt &guard,
  const ssa_exprt &ssa_lhs,
  const exprt &ssa_full_lhs,
  const exprt &original_full_lhs,
  const exprt &ssa_rhs,
  const sourcet &source,
  assignment_typet assignment_type)
{
  symex_target_equationt::assignment(
    guard,
    ssa_lhs,
    ssa_full_lhs,
    original_full_lhs,
    ssa_rhs,
    source,
    assignment_type);

  rename(SSA_steps.back());
}

/// declare a fresh variable
void symex_target_merge_equationt::decl(
  const exprt &guard,
  const ssa_exprt &ssa_lhs,
  const sourcet &source,
  assignment_typet assignment_type)
{
  symex_target_equationt::decl(
    guard,
    ssa_lhs,
    source,
    assignment_type);

  rename(SSA_steps.back());
}

/// declare a fresh variable
void symex_target_merge_equationt::dead(
  const exprt &expr,
  const ssa_exprt &ssa_expr,
  const sourcet &source)
{
  // we don't record these
}

/// just record a location
void symex_target_merge_equationt::location(
  const exprt &guard,
  const sourcet &source)
{
  symex_target_equationt::location(
    guard,
    source);

  rename(SSA_steps.back());
}

/// just record a location
void symex_target_merge_equationt::function_call(
  const exprt &guard,
  const irep_idt &function_identifier,
  const std::vector<exprt> &ssa_function_arguments,
  const sourcet &source)
{
  symex_target_equationt::function_call(
    guard,
    function_identifier,
    ssa_function_arguments,
    source);

  rename(SSA_steps.back());
}

/// just record a location
void symex_target_merge_equationt::function_return(
  const exprt &guard,
  const irep_idt &function_identifier,
  const sourcet &source)
{
  symex_target_equationt::function_return(
    guard,
    function_identifier,
    source);

  rename(SSA_steps.back());
}

/// just record output
void symex_target_merge_equationt::output(
  const exprt &guard,
  const sourcet &source,
  const irep_idt &output_id,
  const std::list<exprt> &args)
{
  symex_target_equationt::output(
    guard,
    source,
    output_id,
    args);

  rename(SSA_steps.back());
}

/// just record formatted output
void symex_target_merge_equationt::output_fmt(
  const exprt &guard,
  const sourcet &source,
  const irep_idt &output_id,
  const irep_idt &fmt,
  const std::list<exprt> &args)
{
  symex_target_equationt::output_fmt(
    guard,
    source,
    output_id,
    fmt,
    args);

  rename(SSA_steps.back());
}

/// just record input
void symex_target_merge_equationt::input(
  const exprt &guard,
  const sourcet &source,
  const irep_idt &input_id,
  const std::list<exprt> &args)
{
  symex_target_equationt::input(
    guard,
    source,
    input_id,
    args);

  rename(SSA_steps.back());
}

/// record an assumption
void symex_target_merge_equationt::assumption(
  const exprt &guard,
  const exprt &cond,
  const sourcet &source)
{
  symex_target_equationt::assumption(
    guard,
    cond,
    source);

  rename(SSA_steps.back());
}

/// record an assertion
void symex_target_merge_equationt::assertion(
  const exprt &guard,
  const exprt &cond,
  const std::string &msg,
  const sourcet &source)
{
  symex_target_equationt::assertion(
    guard,
    cond,
    msg,
    source);

  rename(SSA_steps.back());
}

/// record a goto instruction
void symex_target_merge_equationt::goto_instruction(
  const exprt &guard,
  const exprt &cond,
  const sourcet &source)
{
  symex_target_equationt::goto_instruction(
    guard,
    cond,
    source);

  rename(SSA_steps.back());
}

/// record a constraint
void symex_target_merge_equationt::constraint(
  const exprt &cond,
  const std::string &msg,
  const sourcet &source)
{
  symex_target_equationt::constraint(
    cond,
    msg,
    source);

  rename(SSA_steps.back());
}
