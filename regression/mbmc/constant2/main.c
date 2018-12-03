void main(int argc, char **argv){
    int val;

    val = 42;
    
    val = val+1;
    
    __CPROVER_assert(val > 10, "val>10");
    __CPROVER_mut_output(val);
}
