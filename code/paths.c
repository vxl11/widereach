int are_consistent(int *index, double *a, double *b) {
    int loc;    
    for (int j = 0; index[j] != 0; j++) {
        loc = index[j];
        if (a[loc] != b[loc]) {
            return 0;
        }
    }
    return 1;
}
