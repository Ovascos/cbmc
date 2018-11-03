#ifndef CPROVER_CBMC_MUTATION_H
#define CPROVER_CBMC_MUTATION_H

#include <memory>
#include <map>

#include <util/expr.h>
#include <util/options.h>
#include <util/cmdline.h>
#include <util/ui_message.h>
#include <util/invariant.h>

enum mutation_typet
{
  LESS_EQUAL_TO_UNEQUAL,
  PLUS_ONE_REMOVE
};

struct mutationt
{
  virtual bool check_expr(const exprt& ex) const = 0;
  virtual bool mutate_expr(exprt& ex) const = 0;
  virtual ~mutationt() {}

  static std::unique_ptr<mutationt> factory(mutation_typet type);
};

class mutation_strategy_choosert
{
public:
  mutation_strategy_choosert();

  /// \brief suitable for displaying as a front-end help message
  std::string show_strategies() const;

  /// \brief is there a factory constructor for the named strategy?
  bool is_valid_strategy(const std::string strategy) const
  {
    return strategies.find(strategy) != strategies.end();
  }

  /// \brief Factory for a mutation_strategy
  ///
  /// Ensure that mutation_strategy_choosert::is_valid_strategy() returns true
  /// for a particular string before calling this function on that string.
  std::unique_ptr<mutationt> get(const std::string strategy) const
  {
    auto found = strategies.find(strategy);
    INVARIANT(
      found != strategies.end(), "Unknown strategy '" + strategy + "'.");
    return found->second.second();
  }

  /// \brief add options, suitable to be invoked from front-ends.
  void
  set_mutation_strategy_options(const cmdlinet &, optionst &, messaget &) const;

protected:
  /// Map from the name of a strategy (to be supplied on the command line), to
  /// the help text for that strategy and a factory thunk returning a pointer to
  /// a derived class of mutationt that implements that strategy.
  std::map<const std::string,
           std::pair<const std::string,
                     const std::function<std::unique_ptr<mutationt>()>>>
    strategies;
};
#endif // CPROVER_CBMC_MUTATION_H
