void main(int argc, char **argv){
    int val;

    __CPROVER_input("val", val);



    __CPROVER_mut_output(val);
}
