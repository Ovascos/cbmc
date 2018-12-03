void main(int argc, char **argv){
    int val;
    int max;

    __CPROVER_input("val", val);
    __CPROVER_input("max", max);
    max %= 10;

    val = val + 1;
    if(val % 2) {
      // do something
      while(max-- > 0) {
        val *= 2;
        __CPROVER_assert(val % 2 == 0, "must be even");
      }
    } else {
      // do something complete different
      if(val > 0)
        val = -val;
      __CPROVER_assert(val <= 0, "must be negative");
    }

    __CPROVER_mut_output(val);
}
