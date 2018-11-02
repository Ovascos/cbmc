#include "mutation.h"

struct mutation_less_uniqualt : public mutationt
{
  bool check_pattern(const exprt& ex) const override 
  { 
    return true;
  }
};

struct mutation_plust : public mutationt
{
  bool check_pattern(const exprt& ex) const override 
  { 
    return true;
  }
};

std::unique_ptr<mutationt> mutationt::factory(mutation_typet type)
{
  switch(type)
  {
    case LESS_EQUAL_TO_UNEQUAL:
      return std::unique_ptr<mutationt>(new mutation_less_uniqualt);
    case PLUS_ONE_REMOVE:
      return std::unique_ptr<mutationt>(new mutation_plust);
    default:
      UNREACHABLE;
  }
  UNREACHABLE;
}
