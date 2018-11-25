#ifndef CPROVER_GOTO_SYMEX_SYMEX_TARGET_MERGE_EQUATION_H
#define CPROVER_GOTO_SYMEX_SYMEX_TARGET_MERGE_EQUATION_H

#include <goto-symex/symex_target_equation.h>

class symex_target_merge_equationt
{
public:
  typedef std::unordered_map<irep_idt,ssa_exprt> ssa_storet;

  symex_target_merge_equationt(symex_target_equationt &eq)
    : equation(eq)
  {}

  virtual ~symex_target_merge_equationt() = default;

  /// Merges prefixless variables with variables with specified prefix
  void merge(unsigned prefix);

  /// Merges variables with prefix1 with variables with prefix2
  void merge(unsigned prefix1, unsigned prefix2);

protected:
  symex_target_equationt equation;

  using SSA_stept = symex_target_equationt::SSA_stept;

  void check(ssa_storet &ssa_exprs, const exprt &ex);
  void find_ssa_exprs(ssa_storet &ssa_exprs);
};

#endif // CPROVER_GOTO_SYMEX_SYMEX_TARGET_MERGE_EQUATION_H
