/*******************************************************************\

Module: Symbolic Execution of mutated ANSI-C program

Author: Thomas Hader

\*******************************************************************/

/// \file
/// Symbolic Execution of mutated ANSI-C program

#include "mbmc.h"

#include <iostream>

#include <util/options.h>
#include <util/exit_codes.h>
#include <util/ui_message.h>
#include <util/decision_procedure.h>

#include <goto-programs/goto_model.h>

#include <goto-symex/symex_target_equation.h>
#include <goto-symex/goto_symex_state.h>
#include <goto-symex/path_storage.h>

#include "symex_bmc.h"
#include "cbmc_solvers.h"

#if 0
// ToDo implement symex callbacks for input and output
safety_checkert::resultt mbmct::run(abstract_goto_modelt &model) {
  resultt result_after_symex;

  // generate symex_target_equation for unmodified program
  setup();
  result_after_symex = execute(model);
  if(result_after_symex == safety_checkert::resultt::ERROR)
    return safety_checkert::resultt::ERROR;

  INVARIANT(result_after_symex != safety_checkert::resultt::PAUSED,
      "Path exploration is not supported in MBMC");

  // ToDo rename SSA-variables

  // perform mutation
  mutator.mutate(options.get_unsigned_int_option("mutation-location"));

  // generate symex_target_equation for modified program
  result_after_symex = execute(model);
  if(result_after_symex == safety_checkert::resultt::ERROR)
    return safety_checkert::resultt::ERROR;

  INVARIANT(result_after_symex != safety_checkert::resultt::PAUSED,
      "Path exploration is not supported in MBMC");

  // ToDo add assumes and asserts

  return solve(model);
}
#endif

void mbmct::perform_symbolic_execution(
  goto_symext::get_goto_functiont get_goto_function)
{
  unsigned mut_id = options.get_unsigned_int_option("mutation-location");

  goto_symex_statet state;
  symex.symex_until_instruction(state, get_goto_function, mutator.get_instruction(mut_id));

  // don't use resume_symex_from_saved_state because our state and equation
  // are still valid (since we are in the same mbmct instance)

  // ToDo maybe copy symbol_table as well
  // ToDo insert necesary copy equations
  // Since statet has a huge memory footprint, let it go out of scopt asap
  {
    goto_symex_statet state_orig(state, &equation);
    symex.symex_with_state(state_orig, get_goto_function, symex_symbol_table);
  }

  mutator.mutate(mut_id);

  {
    goto_symex_statet state_mut(state, &equation);
    symex.symex_with_state(state_mut, get_goto_function, symex_symbol_table);
  }

  // ToDo insert necessary asserts
}

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

void mbmct::report_success() {
  std::cout << "Juhu" << std::endl;
}

// ToDo check if failed assertion(s) had been added by user program or mbmc
void mbmct::report_failure() {
  std::cout << "Buuh" << std::endl;
}

