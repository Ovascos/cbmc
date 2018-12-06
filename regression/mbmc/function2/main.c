int func(void) {
  int ret;
  __CPROVER_mut_input(ret);
  ret = ret % 10;
  return ret;
}

void main(int argc, char **argv){
    int val;
    int foo;

    __CPROVER_input("val", val);
    __CPROVER_input("foo", foo);

    foo += 1;
    if(foo % 2)
      val += func();

    __CPROVER_mut_output(val);
}
