#include "mutation.h"

#include <util/std_expr.h>

static inline bool has_n_op(const exprt &ex, unsigned int n) {
  return ex.operands().size() == n;
}

#define CHECK(x) do { if(!(x)) return false; } while(0)

struct mutation_noop : public mutationt
{
  bool mutate_expr(exprt& ex) const override
  {
    return true;
  }

  bool check_expr(const exprt& ex) const override
  {
    return false;
  }
};

struct mutation_plust : public mutationt
{
  bool mutate_expr(exprt& ex) const override
  {
    INVARIANT(check_expr(ex), "invalid exprt passed");
    ex = ex.op0();
    return true;
  }

  bool check_expr(const exprt& ex) const override 
  {
    CHECK(ex.id() == ID_plus);
    CHECK(has_n_op(ex, 2));

    const exprt& op = 
      (ex.op1().id() == ID_typecast) ? ex.op1().op0() : ex.op1();
    CHECK(op.is_constant());

    const constant_exprt& c_ex = static_cast<const constant_exprt&>(op);
    CHECK(c_ex.value_is_one_string());
    return true;
  }
};

struct mutation_less_uniqualt : public mutationt
{
  bool mutate_expr(exprt& ex) const override
  {
    INVARIANT(check_expr(ex), "invalid exprt passed");
    ex = not_exprt(equal_exprt(ex.op0(), ex.op1()));
    return true;
  }

  bool check_expr(const exprt& ex) const override 
  { 
    CHECK(ex.id() == ID_lt);
    CHECK(has_n_op(ex, 2));
    return true;
  }
};

struct mutation_mul2_shift : public mutationt
{
  bool mutate_expr(exprt& ex) const override
  {
    INVARIANT(check_expr(ex), "invalid exprt passed");
    exprt& op = (ex.op1().id() == ID_typecast) ? ex.op1().op0() : ex.op1();
    static_cast<constant_exprt&>(op).set_value("01");
    ex=shl_exprt(ex.op0(), ex.op1());
    return true;
  }

  bool check_expr(const exprt& ex) const override
  {
    CHECK(ex.id() == ID_mult);
    CHECK(has_n_op(ex, 2));

    const exprt& op =
      (ex.op1().id() == ID_typecast) ? ex.op1().op0() : ex.op1();
    CHECK(op.is_constant());

    const constant_exprt& c_ex = static_cast<const constant_exprt&>(op);
    CHECK(c_ex.value_is_n_string(2));
    return true;
  }
};

struct mutation_eq_uneq : public mutationt
{
  bool mutate_expr(exprt& ex) const override
  {
    INVARIANT(check_expr(ex), "invalid exprt passed");
    ex = not_exprt(equal_exprt(ex.op0(), ex.op1()));
    return true;
  }

  bool check_expr(const exprt& ex) const override
  {
    CHECK(ex.id() == ID_equal);
    CHECK(has_n_op(ex, 2));
    return true;
  }
};

std::unique_ptr<mutationt> mutationt::factory(mutation_typet type)
{
  switch(type)
  {
    case NOOP:
      return std::unique_ptr<mutationt>(new mutation_noop);
    case LESS_EQUAL_TO_UNEQUAL:
      return std::unique_ptr<mutationt>(new mutation_less_uniqualt);
    case PLUS_ONE_REMOVE:
      return std::unique_ptr<mutationt>(new mutation_plust);
    case MUL2_TO_SHIFT:
      return std::unique_ptr<mutationt>(new mutation_mul2_shift);
    case EQ_TO_UNEQ:
      return std::unique_ptr<mutationt>(new mutation_eq_uneq);
    default:
      UNREACHABLE;
  }
  UNREACHABLE;
}
