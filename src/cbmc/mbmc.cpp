/*******************************************************************\

Module: Symbolic Execution of mutated ANSI-C program

Author: Thomas Hader

\*******************************************************************/

/// \file
/// Symbolic Execution of mutated ANSI-C program

#include "mbmc.h"

#include <util/options.h>
#include <util/exit_codes.h>
#include <util/ui_message.h>
#include <util/decision_procedure.h>

#include <goto-programs/goto_model.h>

#include <goto-symex/symex_target_equation.h>
#include <goto-symex/path_storage.h>

#include "symex_bmc.h"
#include "cbmc_solvers.h"

int mbmct::do_mbmc(
  const path_strategy_choosert &path_strategy_chooser, // not used
  const optionst &opts,
  abstract_goto_modelt &model,
  mutatort &mutator,
  const ui_message_handlert::uit &ui,
  messaget &message)
{
  safety_checkert::resultt final_result = safety_checkert::resultt::UNKNOWN;

  const symbol_tablet &symbol_table = model.get_symbol_table();
  message_handlert &mh = message.get_message_handler();

  INVARIANT(
    !opts.get_bool_option("paths"), 
    "Path evaluation is not supported yet.");

  std::unique_ptr<path_storaget> worklist = path_strategy_chooser.get("fifo");

  mutator.mutate(opts.get_unsigned_int_option("mutation-location"));

  try
  {
    cbmc_solverst solvers(opts, symbol_table, message.get_message_handler());
    solvers.set_ui(ui);
    std::unique_ptr<cbmc_solverst::solvert> cbmc_solver;
    cbmc_solver = solvers.get_solver();
    prop_convt &pc = cbmc_solver->prop_conv();
    mbmct mbmc(opts, symbol_table, mh, pc, *worklist, mutator);
    mbmc.set_ui(ui);
    final_result = mbmc.run(model);
  }
  catch(const char *error_msg)
  {
    message.error() << error_msg << message.eom;
    return CPROVER_EXIT_EXCEPTION;
  }
  catch(const std::string &error_msg)
  {
    message.error() << error_msg << message.eom;
    return CPROVER_EXIT_EXCEPTION;
  }
  catch(std::runtime_error &e)
  {
    message.error() << e.what() << message.eom;
    return CPROVER_EXIT_EXCEPTION;
  }

  INVARIANT(worklist->empty(), "The worklist should be empty, since path "
      "exploraton is not implemented with mutation checking yet.");

  switch(final_result)
  {
  case safety_checkert::resultt::SAFE:
    return CPROVER_EXIT_VERIFICATION_SAFE;
  case safety_checkert::resultt::UNSAFE:
    return CPROVER_EXIT_VERIFICATION_UNSAFE;
  case safety_checkert::resultt::ERROR:
    return CPROVER_EXIT_INTERNAL_ERROR;
  case safety_checkert::resultt::UNKNOWN:
    return CPROVER_EXIT_INTERNAL_ERROR;
  case safety_checkert::resultt::PAUSED:
    UNREACHABLE;
  }
  UNREACHABLE;
}

