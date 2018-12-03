void main(int argc, char **argv){
    int i;

    __CPROVER_input("i", i);
    
    i += 1;
    i *= 2;

    __CPROVER_mut_output(i);
    __CPROVER_assert(i % 2, "i is odd");
}
