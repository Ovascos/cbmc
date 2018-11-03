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
  
  if(cmdline.isset("list-locations"))
  {
    if(!cmdline.isset("mutator"))
    {
      message.error() << "--list-locations requires --mutator to be given"
                      << message.eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
    options.set_option("mutation-list", true);
  }

  if(cmdline.isset("mutate"))
  {
    if(!cmdline.isset("mutator"))
    {
      message.error() << "Mutator not selected. Pass the --mutator flag to "
                         "select mutator"
                      << message.eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
    try
    {
      options.set_option("mutation-location",
          (unsigned int)std::stoul(cmdline.get_value("mutate")));
    }
    catch(...)
    {
      message.error() << "Invalid mutaion location \""
                      << cmdline.get_value("mutate") << "\"."
                      << message.eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
    options.set_option("mutate", true);
  }
}

std::unique_ptr<mutationt> mutation_strategy_choosert::get(
    const std::string strategy) const
{
  if(strategy.empty())
    return mutationt::factory(NOOP);

  auto found = strategies.find(strategy);
  INVARIANT(
    found != strategies.end(), "Unknown strategy '" + strategy + "'.");
  return found->second.second();
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
