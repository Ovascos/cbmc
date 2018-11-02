#ifndef CPROVER_CBMC_MUTATOR_H
#define CPROVER_CBMC_MUTATOR_H

#include "mutation.h"

#include <vector>

#include <goto-programs/goto_model.h>
#include <goto-programs/goto_program.h>

#include <util/ui_message.h>
#include <util/expr.h>

struct mutation_locationt
{
  mutation_locationt(goto_programt::instructiont &_inst, exprt &_expr)
    : inst(&_inst),
      expr(&_expr)
  {}

  goto_programt::instructiont *const inst;
  exprt                       *const expr;
};


class mutatort
{
public:
  mutatort(const mutationt &_mutation)
    : mutation(_mutation)
  {
  }

  void analyze(goto_modelt &);
  void analyze(goto_functionst &);
  void analyze(goto_programt &);

  void show_location_ids(ui_message_handlert::uit);

protected:
  struct expr_mutation_visotort;

  const mutationt& mutation;
  std::vector<mutation_locationt> mutation_locations;

  void match(goto_programt::instructiont &instr, exprt &ex);
};

#endif // CPROVER_CBMC_MUTATOR_H
