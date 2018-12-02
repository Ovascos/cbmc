void main(int argc, char **argv){
    int val;

    __CPROVER_input("val", val);

    val = val + 1;

    if(val % 2)
      val = val + 3;
    
    __CPROVER_assert((val & 1) == 0, "i");
    __CPROVER_mut_output(val);
}
