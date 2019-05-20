// forward pass:
for (i=0; i!=/*NB_HIDDEN*/; ++i) {
    active_h/*S_INDEX*/[i] = 0.0;
    for (j=0; j!=/*NB_INPUTS*/; ++j) {
        active_h/*S_INDEX*/[i] += weight_u/*S_INDEX*/[i][j] * input/*S_INDEX*/[j];
    }
    active_z/*S_INDEX*/[i] = lrelu(active_h/*S_INDEX*/[i]);
}
partition/*S_INDEX*/ = 0.0;
for (i=0; i!=/*NB_OUTPUTS*/; ++i) {
    active_hh/*S_INDEX*/[i] = 0.0;
    for (j=0; j!=/*NB_HIDDEN*/; ++j) {
        active_hh/*S_INDEX*/[i] += weight_v/*S_INDEX*/[i][j] * active_z/*S_INDEX*/[j];
    }
    active_exphh/*S_INDEX*/[i] = exp(active_hh/*S_INDEX*/[i]);
    partition/*S_INDEX*/+= active_exphh/*S_INDEX*/[i];
}

// sample based on distribution:
randval = uniform();
cumulative = 0.0;
for (i=0; i!=/*NB_OUTPUTS*/; ++i) {
    cumulative += active_exphh/*S_INDEX*/[i] / partition/*S_INDEX*/;
    if (cumulative >= randval) { break; }
}
sample/*S_INDEX*/ = i;
