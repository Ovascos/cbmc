#ifndef CPROVER_CBMC_SYMEX_TARGET_MERGE_EQUATION_H
#define CPROVER_CBMC_SYMEX_TARGET_MERGE_EQUATION_H

#include <unordered_map>
#include <unordered_set>
#include <goto-symex/symex_target_equation.h>

class symex_target_merge_equationt : public symex_target_equationt
{
public:
  typedef int prefixt;

  typedef std::unordered_map<irep_idt, ssa_exprt> ssa_symbolst;
  typedef std::unordered_map<prefixt, ssa_symbolst> ssa_prefix_symbols;

  typedef std::unordered_set<irep_idt> ssa_renamingst;
  typedef std::unordered_map<prefixt, ssa_renamingst> ssa_prefix_renamingst;

  enum { NO_PREFIX = -1 };

  virtual ~symex_target_merge_equationt() = default;

  symex_target_merge_equationt()
    : prefix(NO_PREFIX)
  {
    ssa_store[NO_PREFIX];
  }

  void set_prefix(int prefix);

protected:
  int prefix;

  /// stores all SSA symbols by prefix. (they are stored unrenamed)
  ssa_prefix_symbols ssa_store;
  /// stores all SSA symbols (by prefix) which are already renamed
  /// They are stored without prefix.
  ssa_prefix_renamingst ssa_renamings;

  void rename(SSA_stept &step);
  void check(ssa_renamingst &renamings, exprt &ex);
  void perform_renamings(ssa_renamingst &renamings);
  void perform_renamings(const SSA_stept &, const ssa_renamingst &);

private:
  struct ssa_rename_visitort;


  //
  // delegates for ssa_exprt renaming, they call will call the functions 
  // they're overloading
  //
public:
  // read event
  virtual void shared_read(
    const exprt &guard,
    const ssa_exprt &ssa_object,
    unsigned atomic_section_id,
    const sourcet &source);

  // write event
  virtual void shared_write(
    const exprt &guard,
    const ssa_exprt &ssa_object,
    unsigned atomic_section_id,
    const sourcet &source);

  // assignment to a variable - lhs must be symbol
  virtual void assignment(
    const exprt &guard,
    const ssa_exprt &ssa_lhs,
    const exprt &ssa_full_lhs,
    const exprt &original_full_lhs,
    const exprt &ssa_rhs,
    const sourcet &source,
    assignment_typet assignment_type);

  // declare fresh variable - lhs must be symbol
  virtual void decl(
    const exprt &guard,
    const ssa_exprt &ssa_lhs,
    const sourcet &source,
    assignment_typet assignment_type);

  // note the death of a variable - lhs must be symbol
  virtual void dead(
    const exprt &guard,
    const ssa_exprt &ssa_lhs,
    const sourcet &source);

  // record a function call
  virtual void function_call(
    const exprt &guard,
    const irep_idt &function_identifier,
    const std::vector<exprt> &ssa_function_arguments,
    const sourcet &source);

  // record return from a function
  virtual void function_return(
    const exprt &guard,
    const irep_idt &function_identifier,
    const sourcet &source);

  // just record a location
  virtual void location(
    const exprt &guard,
    const sourcet &source);

  // output
  virtual void output(
    const exprt &guard,
    const sourcet &source,
    const irep_idt &fmt,
    const std::list<exprt> &args);

  // output, formatted
  virtual void output_fmt(
    const exprt &guard,
    const sourcet &source,
    const irep_idt &output_id,
    const irep_idt &fmt,
    const std::list<exprt> &args);

  // input
  virtual void input(
    const exprt &guard,
    const sourcet &source,
    const irep_idt &input_id,
    const std::list<exprt> &args);

  // record an assumption
  virtual void assumption(
    const exprt &guard,
    const exprt &cond,
    const sourcet &source);

  // record an assertion
  virtual void assertion(
    const exprt &guard,
    const exprt &cond,
    const std::string &msg,
    const sourcet &source);

  // record a goto
  virtual void goto_instruction(
    const exprt &guard,
    const exprt &cond,
    const sourcet &source);

  // record a (global) constraint
  virtual void constraint(
    const exprt &cond,
    const std::string &msg,
    const sourcet &source);

  // record thread spawn
  virtual void spawn(
    const exprt &guard,
    const sourcet &source);

  // record memory barrier
  virtual void memory_barrier(
    const exprt &guard,
    const sourcet &source);

  // record atomic section
  virtual void atomic_begin(
    const exprt &guard,
    unsigned atomic_section_id,
    const sourcet &source);
  virtual void atomic_end(
    const exprt &guard,
    unsigned atomic_section_id,
    const sourcet &source);
};

#endif // CPROVER_CBMC_SYMEX_TARGET_MERGE_EQUATION_H
