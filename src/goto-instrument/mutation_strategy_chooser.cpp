#include "mutation.h"

#include <util/exit_codes.h>

#include <string>
#include <sstream>

std::string mutation_strategy_choosert::show_strategies() const
{
  std::stringstream ss;
  for(auto &pair : strategies)
    ss << pair.second.first;
  return ss.str();
}

void mutation_strategy_choosert::set_mutation_strategy_options(
  const cmdlinet &cmdline,
  optionst &options,
  messaget &message) const
{
  if(cmdline.isset("mutator"))
  {
    options.set_option("mutation-test", true);
    std::string strategy = cmdline.get_value("mutator");
    if(!is_valid_strategy(strategy))
    {
      message.error() << "Unknown strategy '" << strategy
                      << "'. Pass the --show-mutators flag to list "
                         "available mutation strategies."
                      << message.eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
    options.set_option("mutation-strategy", strategy);
  }
  else
    options.set_option("mutation-test", false);
}

mutation_strategy_choosert::mutation_strategy_choosert()
  : strategies(
    {
      {"less-2-uneq",
        {" less-2-uneq         replaces less (<) with unequal (!=)\n",
         []() { // NOLINT(whitespace/braces)
           return mutationt::factory(LESS_EQUAL_TO_UNEQUAL);
         }}},
       {"rm-plus1",
        {" rm-plus1            removes plus 1, i.e. replaces(x + 1) with (x)\n",
         []() { // NOLINT(whitespace/braces)
           return mutationt::factory(PLUS_ONE_REMOVE);
         }}}
    })
{
}
