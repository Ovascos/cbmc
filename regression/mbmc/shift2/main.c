void main(int argc, char **argv){
    int val;
    int out;

    __CPROVER_input("val", val);
    __CPROVER_input("out", out);

    val %= 6;
    
    for(int i = 0; i < val; i++)
      out = out * 2;

    __CPROVER_mut_output(out);
}
