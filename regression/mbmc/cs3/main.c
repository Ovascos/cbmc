// Function to return  
// gcd of a and b 
int gcd(int a, int b) 
{ 
    if (a == 0) 
        return b; 
    return gcd(b % a, a); 
} 

void main(int argc, char **argv){
    int a, b;

    __CPROVER_input("a", a);
    __CPROVER_input("b", b);
		__CPROVER_assume(a > 0);
		__CPROVER_assume(b > 0);
		
		int ret = gcd(a,b);

    __CPROVER_mut_output(ret);
}
