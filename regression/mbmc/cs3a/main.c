unsigned gcd(unsigned a, unsigned b) {
  unsigned t;
  while (!(b == 0)) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}

void main(int argc, char **argv){
    unsigned a, b;

    __CPROVER_input("a", a);
    __CPROVER_input("b", b);
    __CPROVER_assume(b < 100);

		unsigned ret = gcd(a, b);

    __CPROVER_mut_output(ret);
}
