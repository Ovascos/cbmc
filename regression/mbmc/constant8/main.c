void main(){
    int val;

    __CPROVER_input("val", val);

    int max = 2;
    max = max + 1;

    for(int j = 0; j < 5-max; j++)
      for(int i = 0; i < max; i++)
        __CPROVER_mut_output(val);
}
