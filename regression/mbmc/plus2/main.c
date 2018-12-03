void main(int argc, char **argv){
    unsigned val;

    __CPROVER_input("val", val);

    val %= 8;
    if(val)
      val = val + 1;
    while(val > 0)
      val--;

    __CPROVER_mut_output(val);
}
