#ifndef CPROVER_CBMC_MUTATOR_H
#define CPROVER_CBMC_MUTATOR_H

#include "mutation.h"

#include <vector>
#include <memory>

#include <goto-programs/goto_model.h>
#include <goto-programs/goto_program.h>

#include <util/ui_message.h>
#include <util/expr.h>

struct mutation_locationt
{
  mutation_locationt(goto_programt::const_targett &_instr, exprt &_expr)
    : instr(_instr),
      expr(&_expr)
  {}

  goto_programt::const_targett instr;
  exprt                 *const expr;
};


class mutatort
{
public:
  mutatort(std::unique_ptr<mutationt> _m)
    : mutation(std::move(_m))
  {
  }

  void analyze(goto_modelt &);
  void analyze(goto_functionst &);
  void analyze(goto_programt &);

  void show_location_ids(ui_message_handlert::uit) const;

  void mutate(unsigned id);
  bool location_id_valid(unsigned id) const;
  goto_programt::const_targett get_instruction(unsigned id) const;

protected:
  struct expr_mutation_visitort;

  const std::unique_ptr<mutationt> mutation;
  std::vector<mutation_locationt> mutation_locations;

  void match(goto_programt::targett &instr, exprt &ex);
};

#endif // CPROVER_CBMC_MUTATOR_H
