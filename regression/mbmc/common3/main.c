void main(int argc, char **argv){
    unsigned val;

    __CPROVER_input("val", val);

    val %= -2;
    val += 1;
    if(val % 2)
      __CPROVER_assert(0, "should fail on mutation");

    __CPROVER_mut_output(val);
}
