for (int i=0; i!=/*NB_HIDDEN*/; ++i) {
    for (int j=0; j!=/*NB_INPUTS*/; ++j) {
        weight_u/*S_INDEX*/[i][j] = WEIGHT_INIT_SCALE * laplace();
    }
}
for (int i=0; i!=/*NB_OUTPUTS*/; ++i) {
    for (int j=0; j!=/*NB_HIDDEN*/; ++j) {
        weight_v/*S_INDEX*/[i][j] = WEIGHT_INIT_SCALE * laplace();
    }
}
