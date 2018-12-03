void main(int argc, char **argv){
    int val = 43;

    while(val >= 0) {
      val += 1;
      val /= 2;
    }

    __CPROVER_mut_output(val);
}
