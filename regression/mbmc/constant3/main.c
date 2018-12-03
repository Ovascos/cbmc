void main(int argc, char **argv){
    int val;

    __CPROVER_input("val", val);

    while(val >= 2) {
      val += 1;
      val /= 2;
    }

    __CPROVER_mut_output(val);
}
