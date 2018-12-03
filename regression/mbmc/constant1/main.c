void main(int argc, char **argv){
    int val;

    val = 42;
    
    val = val+1;
    if(val != 42)
      val = 42;
    
    __CPROVER_mut_output(val);
}
