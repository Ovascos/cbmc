void main(int argc, char **argv){
    int val;
    int out;

    __CPROVER_input("val", val);

    if(val * 2 > 42)
      out = 0;

    __CPROVER_mut_output(out);
}
