// update history:
if (history_len/*S_INDEX*/ == HISTORY_CAPACITY) {
    i = rand() % HISTORY_CAPACITY; 
} else {
    i = history_len/*S_INDEX*/;
    history_len/*S_INDEX*/ += 1;
}
for (j=0; j!=/*NB_INPUTS*/-1; ++j) {
    input_history/*S_INDEX*/[i][j] = input/*S_INDEX*/[j];
}
sample_history/*S_INDEX*/[i] = sample/*S_INDEX*/;
