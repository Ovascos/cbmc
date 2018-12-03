int fib_loop(unsigned n);
int fib_rec(unsigned n);

void main(int argc, char **argv){
    int val, ret;
    unsigned max;

    __CPROVER_input("val", val);
    __CPROVER_assume(max < 5);

    val = val + 1;
    if(val % 2) {
      ret = fib_rec(max);
    } else {
      ret = fib_loop(max);
    }

    __CPROVER_mut_output(ret);
}

int fib_loop(unsigned n) {
  int first = 0, second = 1, next;
  for(unsigned c = 0; c <= n; c++) {
    if(c <= 1)
      next = c;
    else {
      next = first + second;
      first = second;
      second = next;
    }
  }
  return next;
}

int fib_rec(unsigned n) {
  if(n == 0 || n == 1)
    return n;
  else
    return (fib_rec(n-1) + fib_rec(n-2));
}
