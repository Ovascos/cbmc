#ifndef CPROVER_CBMC_CHECKER_H
#define CPROVER_CBMC_CHECKER_H

#include <util/options.h>
#include <util/ui_message.h>
#include <util/decision_procedure.h>

#include <goto-programs/goto_model.h>
#include <goto-programs/goto_trace.h>
#include <goto-programs/safety_checker.h>

#include <goto-symex/symex_target_equation.h>
#include <goto-symex/path_storage.h>
#include <goto-symex/memory_model.h>

#include "symex_bmc.h"

class checkert : public safety_checkert
{
public:
  virtual ~checkert() = default;

  virtual resultt run(const goto_functionst &goto_functions)
  {
    wrapper_goto_modelt model(outer_symbol_table, goto_functions);
    return run(model);
  }

  virtual resultt run(abstract_goto_modelt &model);

  void setup();
  safety_checkert::resultt execute(abstract_goto_modelt &);
  safety_checkert::resultt solve(abstract_goto_modelt &);

  void set_ui(ui_message_handlert::uit _ui) { ui=_ui; }

  // the safety_checkert interface
  virtual resultt operator()(
    const goto_functionst &goto_functions)
  {
    return run(goto_functions);
  }

  void add_loop_unwind_handler(symex_bmct::loop_unwind_handlert handler)
  {
    symex.add_loop_unwind_handler(handler);
  }

  void add_unwind_recursion_handler(
    symex_bmct::recursion_unwind_handlert handler)
  {
    symex.add_recursion_unwind_handler(handler);
  }

protected:
  /// \brief Constructor for path exploration from saved state
  ///
  /// This constructor exists as a delegate for the path_explorert class.
  /// It differs from \ref bmct's public constructor in that it actually
  /// does something with the path_storaget argument, and also takes a
  /// symex_target_equationt. See the documentation for path_explorert for
  /// details.
  checkert(
    const optionst &_options,
    const symbol_tablet &outer_symbol_table,
    message_handlert &_message_handler,
    prop_convt &_prop_conv,
    symex_target_equationt &_equation,
    path_storaget &_path_storage,
    std::function<bool(void)> callback_after_symex)
    : safety_checkert(ns, _message_handler),
      options(_options),
      outer_symbol_table(outer_symbol_table),
      ns(outer_symbol_table, symex_symbol_table),
      equation(_equation),
      path_storage(_path_storage),
      symex(
        _message_handler,
        outer_symbol_table,
        equation,
        options,
        path_storage),
      prop_conv(_prop_conv),
      ui(ui_message_handlert::uit::PLAIN),
      driver_callback_after_symex(callback_after_symex)
  {
    symex.constant_propagation = options.get_bool_option("propagation");
    symex.record_coverage =
      !options.get_option("symex-coverage-report").empty();
    symex.self_loops_to_assumptions =
      options.get_bool_option("self-loops-to-assumptions");
  }

  const optionst &options;
  /// \brief symbol table for the goto-program that we will execute
  const symbol_tablet &outer_symbol_table;
  /// \brief symbol table generated during symbolic execution
  symbol_tablet symex_symbol_table;
  namespacet ns;
  symex_target_equationt &equation;
  path_storaget &path_storage;
  symex_bmct symex;
  prop_convt &prop_conv;
  std::unique_ptr<memory_model_baset> memory_model;
  // use gui format
  ui_message_handlert::uit ui;

  virtual decision_proceduret::resultt
    run_decision_procedure(prop_convt &prop_conv);

  virtual resultt decide(
    const goto_functionst &,
    prop_convt &);

  void do_conversion();

  virtual void freeze_program_variables();

  trace_optionst trace_options()
  {
    return trace_optionst(options);
  }

  virtual resultt all_properties(
    const goto_functionst &goto_functions,
    prop_convt &solver);
  virtual resultt stop_on_fail(prop_convt &solver);
  virtual void report_success() {}
  virtual void report_failure() {}

  virtual void error_trace();
  void output_graphml(resultt result);

  void get_memory_model();
  void slice();
  void show();

  bool cover(const goto_functionst &goto_functions);

  friend class bmc_all_propertiest;
  friend class bmc_covert;
  template <template <class goalt> class covert>
  friend class bmc_goal_covert;
  friend class fault_localizationt;

private:
  /// \brief Class-specific symbolic execution
  ///
  /// This private virtual should be overridden by derived classes to
  /// invoke the symbolic executor in a class-specific way. This
  /// implementation invokes goto_symext::operator() to perform
  /// full-program model-checking from the entry point of the program.
  virtual void perform_symbolic_execution(
    goto_symext::get_goto_functiont get_goto_function);

  /// Optional callback, to be run after symex but before handing the resulting
  /// equation to the solver. If it returns true then we will skip the solver
  /// stage and return "safe" (no assertions violated / coverage goals reached),
  /// similar to the behaviour when 'show-vcc' or 'program-only' are specified.
  std::function<bool(void)> driver_callback_after_symex;
};

#endif // CPROVER_CBMC_CHECKER_H
