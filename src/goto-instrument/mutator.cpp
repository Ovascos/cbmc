#include "mutator.h"

#include <iostream>

#include <util/expr.h>
#include <util/std_code.h>

#include <goto-programs/goto_program.h>

void mutatort::show_location_ids(
  ui_message_handlert::uit ui) const
{
  INVARIANT(ui == ui_message_handlert::uit::PLAIN,
      "other ui modes not supported yet");

  unsigned int cnt = 0;

  switch(ui)
  {
  case ui_message_handlert::uit::PLAIN:
    for(mutation_locationt loc : mutation_locations)
    {
      std::cout << cnt++ << ": " <<  loc.instr->source_location << "\n";
    }
    break;
  default:
    UNREACHABLE;
  }
}

void mutatort::mutate(unsigned id)
{
  INVARIANT(id < mutation_locations.size(),
      "invlid mutation id int mutate() call");
  mutation->mutate_expr(*(mutation_locations[id].expr));
}

struct mutatort::expr_mutation_visotort : expr_visitort
{
  expr_mutation_visotort(
    mutatort &_mutator,
    goto_programt::instructiont &_instr)
    : mutator(_mutator),
      instr(_instr)
  {}

  mutatort &mutator;
  goto_programt::instructiont &instr;

  void operator()(exprt &ex) override
  {
    if(mutator.mutation->check_expr(ex))
      mutator.mutation_locations.emplace_back(instr, ex);
  }
};

void mutatort::match(goto_programt::instructiont &instr, exprt &ex)
{
  expr_mutation_visotort visit(*this, instr);
  ex.visit(visit);
}

// ToDo: check https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html

void mutatort::analyze(goto_modelt &goto_model)
{
  analyze(goto_model.goto_functions);
}

void mutatort::analyze(goto_functionst &goto_functions)
{
  Forall_goto_functions(it, goto_functions)
    analyze(it->second.body);
}

// ToDo check with all true mutation (somehow there'r non-file location instructions

void mutatort::analyze(goto_programt &goto_program)
{
  Forall_goto_program_instructions(it, goto_program) {
    if(it->source_location.is_built_in() || it->source_location.get_hide())
      continue;

    switch(it->type) {
      case FUNCTION_CALL:
        // ToDo check if function call has a guard and/or code
        match(*it, it->code);
        match(*it, it->guard);
        break;

      case GOTO:
        // mutate guard
        match(*it, it->guard);
        break;

      case ASSIGN:
      case RETURN:
      case OTHER:
        match(*it, it->code);
        break;

      case ASSUME:
      case ASSERT:
        // ToDo mutate assert and assume?
        break;

      case SKIP:
      case DEAD:
      case LOCATION:
      case END_FUNCTION:
      case MUT_INPUT:
      case MUT_OUTPUT:
        // don't mutate in this types
        break;

      case THROW:
      case DECL:
      case CATCH:
        // don't know if we want to mutate those
        break;

      case ATOMIC_BEGIN:
      case ATOMIC_END:
      case START_THREAD:
      case END_THREAD:
        // concurrency not supported yet
        break;

      case NO_INSTRUCTION_TYPE:
      case INCOMPLETE_GOTO:
        UNREACHABLE;
    }
  }
}

