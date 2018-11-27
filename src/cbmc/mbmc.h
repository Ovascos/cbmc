#ifndef CPROVER_CBMC_MBMC_H
#define CPROVER_CBMC_MBMC_H

#include <util/options.h>
#include <util/ui_message.h>

#include <goto-programs/goto_model.h>
#include <goto-symex/path_storage.h>

#include <goto-instrument/mutator.h>

#include "checker.h"

class mbmct : public checkert
{
public:
  mbmct(
    const optionst &_options,
    const symbol_tablet &_outer_symbol_table,
    message_handlert &_message_handler,
    prop_convt &_prop_conv,
    path_storaget &_path_storage,
    mutatort &_mutator)
    : checkert(
        _options,
        _outer_symbol_table,
        _message_handler,
        _prop_conv,
        _path_storage,
        nullptr),
      mutator(_mutator)
  { }

  // path exploration is unspupported here
  static int do_mbmc(
    const path_strategy_choosert &path_strategy_chooser, // not used
    const optionst &opts,
    abstract_goto_modelt &model,
    mutatort &mutator,
    const ui_message_handlert::uit &ui,
    messaget &message);

protected:
  mutatort &mutator;

  virtual void report_success() override;
  virtual void report_failure() override;

private:
  void perform_symbolic_execution(goto_symext::get_goto_functiont get_goto_function) override;
};

#define OPT_MBMC                                                               \
  "(show-mutators)"                                                            \
  "(mutator):"                                                                 \
  "(list-locations)(mutate):"

#define HELP_MBMC                                                              \
  " --show-mutators              list available mutators\n"                    \
  " --mutator                    specify mutator to use\n"                     \
  "                              (use --show-mutators to get mutator names)\n" \
  " --list-locations             show possible locatons to be mutated\n"       \
  " --mutate                     perform mutation on specified location\n"     \
  "                              (use --list-locations to get location IDs)\n"

#endif // CPROVER_CBMC_MBMC_H
