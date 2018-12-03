void main(int argc, char **argv){
    int result = 0;
    int max;

    __CPROVER_input("max", max);
    __CPROVER_assume(max == 5);

    for(int i = 0; i < max; i++) {
      result += i;
    }

    __CPROVER_mut_output(result);
}
