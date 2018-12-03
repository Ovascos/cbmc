void main(int argc, char **argv){
    int result = 0;
    int max = 10;

    for(int i = 0; i < max; i++) {
      result += i;
    }

    __CPROVER_mut_output(result);
}
