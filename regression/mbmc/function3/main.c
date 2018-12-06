void func(int i) {
  i %= 42;
  i += 1;
  __CPROVER_mut_output(i);
}

void main(int argc, char **argv){
    int val;
    int foo;

    __CPROVER_input("val", val);
    __CPROVER_input("foo", foo);

    foo += 1;
    if(foo % 2)
      func(val);
}
