void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubble_sort(int arr[], int n) 
{ 
  int i, j;
  for (i = 0; i < n-1; i++)
    for (j = 0; j < n-i-1; j++)
      if (arr[j] > arr[j+1])
        swap(&arr[j], &arr[j+1]);
}

#define N 5

void main(int argc, char **argv){
	int a[N];

	bubble_sort(a, N);

	__CPROVER_assert(a[0] <= a[1], "foo");
	__CPROVER_assert(a[1] <= a[2], "foo");
	__CPROVER_assert(a[2] <= a[3], "foo");
	__CPROVER_assert(a[3] <= a[4], "foo");
	for(int i = 0; i < N; i++)
	{
		int x = a[i];
  	__CPROVER_mut_output(x);
  }
}
