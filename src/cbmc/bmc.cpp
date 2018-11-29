/*******************************************************************\

Module: Symbolic Execution of ANSI-C

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Symbolic Execution of ANSI-C

#include "bmc.h"

#include <util/exit_codes.h>

#include "cbmc_solvers.h"

/// Perform core BMC, using an abstract model to supply GOTO function bodies
/// (perhaps created on demand).
/// \param opts: command-line options affecting BMC
/// \param model: provides goto function bodies and the symbol table, perhaps
//    creating those function bodies on demand.
/// \param ui: user-interface mode (plain text, XML output, JSON output, ...)
/// \param message: used for logging
/// \param driver_configure_bmc: function provided by the driver program,
///   which applies driver-specific configuration to a bmct before running.
/// \param callback_after_symex: optional callback to be run after symex.
///   See class member `bmct::driver_callback_after_symex` for details.
int bmct::do_language_agnostic_bmc(
  const path_strategy_choosert &path_strategy_chooser,
  const optionst &opts,
  abstract_goto_modelt &model,
  const ui_message_handlert::uit &ui,
  messaget &message,
  std::function<void(bmct &, const symbol_tablet &)> driver_configure_bmc,
  std::function<bool(void)> callback_after_symex)
{
  safety_checkert::resultt final_result = safety_checkert::resultt::UNKNOWN;
  safety_checkert::resultt tmp_result = safety_checkert::resultt::UNKNOWN;
  const symbol_tablet &symbol_table = model.get_symbol_table();
  message_handlert &mh = message.get_message_handler();
  std::unique_ptr<path_storaget> worklist;
  std::string strategy = opts.get_option("exploration-strategy");
  INVARIANT(
    path_strategy_chooser.is_valid_strategy(strategy),
    "Front-end passed us invalid path strategy '" + strategy + "'");
  worklist = path_strategy_chooser.get(strategy);
  try
  {
    {
      cbmc_solverst solvers(opts, symbol_table, message.get_message_handler());
      solvers.set_ui(ui);
      std::unique_ptr<cbmc_solverst::solvert> cbmc_solver;
      cbmc_solver = solvers.get_solver();
      prop_convt &pc = cbmc_solver->prop_conv();
      bmct bmc(opts, symbol_table, mh, pc, *worklist, callback_after_symex);
      bmc.set_ui(ui);
      if(driver_configure_bmc)
        driver_configure_bmc(bmc, symbol_table);
      tmp_result = bmc.run(model);

      if(
        tmp_result == safety_checkert::resultt::UNSAFE &&
        opts.get_bool_option("stop-on-fail") && opts.is_set("paths"))
      {
        worklist->clear();
        return CPROVER_EXIT_VERIFICATION_UNSAFE;
      }

      if(tmp_result != safety_checkert::resultt::PAUSED)
        final_result = tmp_result;
    }
    INVARIANT(
      opts.get_bool_option("paths") || worklist->empty(),
      "the worklist should be empty after doing full-program "
      "model checking, but the worklist contains " +
        std::to_string(worklist->size()) + " unexplored branches.");

    // When model checking, the bmc.run() above will already have explored
    // the entire program, and final_result contains the verification result.
    // The worklist (containing paths that have not yet been explored) is thus
    // empty, and we don't enter this loop.
    //
    // When doing path exploration, there will be some saved paths left to
    // explore in the worklist. We thus need to run the above code again,
    // once for each saved path in the worklist, to continue symbolically
    // execute the program. Note that the code in the loop is similar to
    // the code above except that we construct a path_explorert rather than
    // a bmct, which allows us to execute from a saved state rather than
    // from the entry point. See the path_explorert documentation, and the
    // difference between the implementations of perform_symbolic_exection()
    // in bmct and path_explorert, for more information.

    while(!worklist->empty())
    {
      if(tmp_result != safety_checkert::resultt::PAUSED)
        message.status() << "___________________________\n"
                         << "Starting new path (" << worklist->size()
                         << " to go)\n"
                         << message.eom;
      cbmc_solverst solvers(opts, symbol_table, message.get_message_handler());
      solvers.set_ui(ui);
      std::unique_ptr<cbmc_solverst::solvert> cbmc_solver;
      cbmc_solver = solvers.get_solver();
      prop_convt &pc = cbmc_solver->prop_conv();
      path_storaget::patht &resume = worklist->peek();
      path_explorert pe(
        opts,
        symbol_table,
        mh,
        pc,
        resume.equation,
        resume.state,
        *worklist,
        callback_after_symex);
      if(driver_configure_bmc)
        driver_configure_bmc(pe, symbol_table);
      tmp_result = pe.run(model);

      if(
        tmp_result == safety_checkert::resultt::UNSAFE &&
        opts.get_bool_option("stop-on-fail") && opts.is_set("paths"))
      {
        worklist->clear();
        return CPROVER_EXIT_VERIFICATION_UNSAFE;
      }

      if(tmp_result != safety_checkert::resultt::PAUSED)
        final_result &= tmp_result;
      worklist->pop();
    }
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

void path_explorert::perform_symbolic_execution(
  goto_symext::get_goto_functiont get_goto_function)
{
  symex.resume_symex_from_saved_state(
    get_goto_function, saved_state, &equation, symex_symbol_table);
}
