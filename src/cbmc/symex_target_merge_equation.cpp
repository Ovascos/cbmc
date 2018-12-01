#include "symex_target_merge_equation.h"

#include <util/expr.h>
#include <util/ssa_expr.h>
#include <util/std_expr.h>
#include <util/guard.h>
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

    if(eq.prefix == NO_PREFIX)
    {
      // register occurrence
      eq.ssa_store.emplace(ssa_ex.get_identifier(), ssa_ex);
    }
    else
    {
      // register and perform renaming
      const ssa_symbolst &no_prefix = eq.ssa_store;
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

  perform_renamings(step.source, new_ren);
}

void symex_target_merge_equationt::perform_renamings(
    const sourcet &source, const ssa_renamingst &renamings)
{
  for(irep_idt id : renamings)
  {
    // nothing to do if there is no no-prefix version
    if(ssa_store.count(id) == 0)
      continue;
    // nothing to do if already renamed
    if(ssa_renamings[prefix].count(id) != 0)
      continue;

    ssa_renamings[prefix].insert(id);
    
    INVARIANT(ssa_store.count(id) > 0, "Didn't find original SSA");
    add_equal_assumption(ssa_store[id], source);
  }
}

void symex_target_merge_equationt::add_equal_assumption(
    const ssa_exprt &ex, const sourcet &source)
{
  PRECONDITION(prefix != NO_PREFIX);
  PRECONDITION(SSA_steps.size() >= 1);

  ssa_exprt ren = ex;
  ren.set_prefix(prefix);
  equal_exprt eq(ex, ren);

  symex_target_equationt::assumption(true_exprt(), eq, source);

  // swap last two elements in order to insert assumption before the new step
  auto it =  SSA_steps.rbegin();
  SSA_stept &a = *it++;
  SSA_stept &b = *it;
  std::swap(a, b);
}

void symex_target_merge_equationt::set_prefix(int _prefix)
{
  PRECONDITION(_prefix >= 0);
  prefix = _prefix;
}

void symex_target_merge_equationt::insert_mutation_assertions()
{
  for(auto a : mut_input_symbols)
  {
    if(a.second.size() <= 1)
      continue;

    INVARIANT(a.second.size() == 2,
        "Merging of more than two versions not supported yet");

    mut_symbolt &s1 = a.second.front();
    mut_symbolt &s2 = a.second.back();
    equal_exprt cond(s1.symbol, s2.symbol);
    guardt      guard;
    guard.add(s1.guard);
    guard.add(s2.guard);
    guard.guard_expr(cond);

    symex_target_equationt::assumption(guard, cond, s1.source);
  }

  for(auto a : mut_output_symbols)
  {
    if(a.second.size() <= 1)
      continue; // ToDo warn?

    INVARIANT(a.second.size() == 2,
        "Merging of more than two versions not supported yet");

    mut_symbolt &s1 = a.second.front();
    mut_symbolt &s2 = a.second.back();
    irep_idt    property_id=s1.source.pc->source_location.get_property_id();
    equal_exprt cond(s1.symbol, s2.symbol);
    guardt      guard;
    guard.add(s1.guard);
    guard.add(s2.guard);
    guard.guard_expr(cond);

    symex_target_equationt::assertion(guard, cond, id2string(property_id), s1.source);
  }
}

// record mutation input/outputs
void symex_target_merge_equationt::mut_input(
    const exprt &guard,
    const exprt &symbol,
    const sourcet &source)
{
  PRECONDITION(is_ssa_expr(symbol) || can_cast_expr<constant_exprt>(symbol));

  // no need to specify input before mutation
  if(prefix == NO_PREFIX)
    return;

  irep_idt property_id=source.pc->source_location.get_property_id();
  INVARIANT(property_id != "", "property_id of mut output must not be empty");

  mut_input_symbols[property_id].push_back(mut_symbolt());
  mut_symbolt &mut_info=mut_input_symbols[property_id].back();

  mut_info.guard = guard;
  mut_info.source = source;
  mut_info.symbol = symbol;

  ssa_renamingst new_ren;
  check(new_ren, mut_info.guard);
  check(new_ren, mut_info.symbol);
  perform_renamings(source, new_ren);
}

void symex_target_merge_equationt::mut_output(
    const exprt &guard,
    const exprt &symbol,
    const sourcet &source)
{
  PRECONDITION(is_ssa_expr(symbol) || can_cast_expr<constant_exprt>(symbol));

  // cannot fail for instructions before the mutation
  if(prefix == NO_PREFIX)
    return;

  irep_idt property_id=source.pc->source_location.get_property_id();
  INVARIANT(property_id != "", "property_id of mut output must not be empty");

  mut_output_symbols[property_id].push_back(mut_symbolt());
  mut_symbolt &mut_info=mut_output_symbols[property_id].back();

  mut_info.guard = guard;
  mut_info.source = source;
  mut_info.symbol = symbol;

  ssa_renamingst new_ren;
  check(new_ren, mut_info.guard);
  check(new_ren, mut_info.symbol);
  perform_renamings(source, new_ren);
}


//
// delegates for ssa_exprt renaming, they call the functions they're overloading
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

/// delete a variable
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
