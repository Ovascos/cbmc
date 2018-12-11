void main(int argc, char **argv){
    int val;
    int ret = 0;

    __CPROVER_input("val", val);

    if(val >= 0)
      ret = 1;

    __CPROVER_mut_output(ret);
}
