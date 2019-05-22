if (history_len/*S_INDEX*/ != 0) {
    for (k=0; k!= history_len/*S_INDEX*/; ++k) {
        // forward pass from history:
        for (i=0; i!=/*NB_INPUTS*/-1; ++i) {
            input/*S_INDEX*/[i] = input_history/*S_INDEX*/[k][i]; 
        }
        input/*S_INDEX*/[/*NB_INPUTS*/-1] = 1.0;
        /*FORWARD_PASS*/
        sample/*S_INDEX*/ = sample_history/*S_INDEX*/[k];

        // backward pass:
        signal = (reward - reward_exp) / history_len/*S_INDEX*/; 
        for (i=0; i!=/*NB_OUTPUTS*/; ++i) {
            dlossd_hh/*S_INDEX*/[i] = - signal * active_exphh/*S_INDEX*/[i] / partition/*S_INDEX*/;
        }
        dlossd_hh/*S_INDEX*/[sample/*S_INDEX*/] += signal;
        
        for (j=0; j!=/*NB_HIDDEN*/; ++j) {
            dlossd_z/*S_INDEX*/[j] = 0.0;
            for (i=0; i!=/*NB_OUTPUTS*/; ++i) {
                dlossd_z/*S_INDEX*/[j] += weight_v/*S_INDEX*/[i][j] * dlossd_hh/*S_INDEX*/[i];
                weight_v/*S_INDEX*/[i][j] += LEARNING_RATE * dlossd_hh/*S_INDEX*/[i] * active_z/*S_INDEX*/[j]; 
                weight_v/*S_INDEX*/[i][j] = clip(weight_v/*S_INDEX*/[i][j]);
            }
            dlossd_h/*S_INDEX*/[j] = dlossd_z/*S_INDEX*/[j] * dlrelu(active_h/*S_INDEX*/[j]); 
        }
        for (j=0; j!=/*NB_INPUTS*/; ++j) {
            for (i=0; i!=/*NB_HIDDEN*/; ++i) {
                weight_u/*S_INDEX*/[i][j] += LEARNING_RATE * dlossd_h/*S_INDEX*/[i] * input/*S_INDEX*/[j]; 
                weight_u/*S_INDEX*/[i][j] = clip(weight_u/*S_INDEX*/[i][j]);
            }
        }
    }
    
    // clear history:
    history_len/*S_INDEX*/ = 0;

}
