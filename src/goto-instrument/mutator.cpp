#include "mutator.h"

#include <iostream>

#include <goto-programs/goto_program.h>

#include <util/std_code.h>
#include <util/irep_ids.h>

void mutatort::show_location_ids(
  ui_message_handlert::uit ui)
{
#if 0
  std::cout << "Loop "
            << instr->function << "." << mut_id << ":" << "\n";

  std::cout << "  " << it->source_location << "\n";
  std::cout << "\n";
#endif
}

void mutatort::match(goto_programt::instructiont &instr, exprt &ex)
{
  // ToDo use visit function
  if(!mutation.check_pattern(ex))
    return;

  mutation_locations.emplace_back(instr, ex);
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

void mutatort::analyze(goto_programt &goto_program)
{
  mutation_locations.clear();

  Forall_goto_program_instructions(it, goto_program) {
    if(it->source_location.is_built_in())
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

