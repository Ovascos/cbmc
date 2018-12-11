#define N 10

static inline void swap(char *data, int a, int b) {
    char t = data[a];
    data[a] = data[b];
    data[b] = t;
}

static inline int partition(char *data, int from, int to) {
    char pivotVal = data[to];
    int j = from;

    for(int i = from; i <= to-1; i++) {
        if(data[i] < pivotVal) {
            swap(data, i, j);
            j++;
        }
    }
    swap(data, to, j);
    return j;
}

static char quickselectR(char *data, int k, int from, int to) {
    if(from == to)
        return data[from];

    int pivotIdx = partition(data, from, to);
    if(k == pivotIdx)
        return data[pivotIdx];
    else if(k < pivotIdx)
        return quickselectR(data, k, from, pivotIdx-1);
    else
        return quickselectR(data, k, pivotIdx+1, to);
}

char quickselect(char *data, int k) {
    return quickselectR(data, k, 0, N-1);
}

void main(int argc, char **argv){
    int data[N];
    unsigned k;

    __CPROVER_assume(k < N);
    int val = quickselect(data, k);
    __CPROVER_mut_output(val);
}
