#ifndef CPROVER_CBMC_MBMC_H
#define CPROVER_CBMC_MBMC_H

#include "bmc.h"

#include <util/options.h>
#include <util/ui_message.h>

#include <goto-programs/goto_model.h>
#include <goto-symex/path_storage.h>

class mutatort;

// ToDo: separate bmct into bmct and checkert
class mbmct : public bmct
{
public:
  mbmct(
    const optionst &_options,
    const symbol_tablet &_outer_symbol_table,
    message_handlert &_message_handler,
    prop_convt &_prop_conv,
    path_storaget &_path_storage,
    mutatort &_mutator)
    : bmct(
        _options,
        _outer_symbol_table,
        _message_handler,
        _prop_conv,
        _path_storage,
        nullptr),
      mutator(_mutator)
  {

  }


  // ToDo path exploration is unspupported here
  static int do_mbmc(
    const path_strategy_choosert &path_strategy_chooser, // not used
    const optionst &opts,
    abstract_goto_modelt &model,
    const ui_message_handlert::uit &ui,
    messaget &message,
    mutatort &mutator);

protected:
  mutatort &mutator;
};

#define OPT_MBMC                                                               \
  "(list-mutators)"                                                            \
  "(mutator):"                                                                 \
  "(list-locations)(mutate):"

#define HELP_MBMC                                                              \
  " --list-mutators              list possible mutators\n"                     \
  " --mutator                    specify mutator to use\n"                     \
  "                              (use --list-mutators to get mutator IDs)\n"   \
  " --list-locations             show possible locatons to be mutated\n"       \
  " --mutate                     perform mutation on specified location\n"     \
  "                              (use --list-locations to get location IDs)\n"

#endif // CPROVER_CBMC_MBMC_H
