// This is a modified version of: https://www.sanfoundry.com/c-program-implement-rabin-miller-primality-test-check-number-prime/
/* 
 * modular exponentiation
 */
long modulo(long base, long exponent, long mod)
{
		long x = 1;
		long y = base;
		while (exponent > 0)
		{
				if (exponent % 2 == 1)
						x = (x * y) % mod;
				y = (y * y) % mod;
				exponent = exponent / 2;
		}
		return x % mod;
}
 
/*
 * Miller-Rabin Primality test, iteration signifies the accuracy
 */
int Miller(long p, int iteration)
{
		int i;
		long s;
		if (p < 2)
				return 0;
		if (p != 2 && p % 2==0)
				return 0;
		s = p - 1;
		while (s % 2 == 0)
				s /= 2;
		for (i = 0; i < iteration; i++)
		{
        int rand;
        __CPROVER_mut_input(rand);
				long a = rand % (p - 1) + 1, temp = s;
				long mod = modulo(a, temp, p);
				while (temp != p - 1 && mod != 1 && mod != p - 1)
				{
						mod = (mod * mod) % p;
						temp *= 2;
				}
				if (mod != p - 1 && temp % 2 == 0)
						return 0;
		}
		return 1;
}

void main(int argc, char **argv){
    int num, iter;
		int result;

    __CPROVER_input("num", num);
    __CPROVER_input("iter", iter);
    __CPROVER_assume(iter <= 5);
    __CPROVER_assume(num < 128);

		result = Miller(num, iter);
    __CPROVER_assert(result == 1 || result == 0, "result is bool");

    __CPROVER_mut_output(result);
}
