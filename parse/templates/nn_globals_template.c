static float input/*S_INDEX*/[/*NB_INPUTS*/];
static float weight_u/*S_INDEX*/[/*NB_HIDDEN*/][/*NB_INPUTS*/];    
static float active_h/*S_INDEX*/[/*NB_HIDDEN*/];       static float dlossd_h/*S_INDEX*/[/*NB_HIDDEN*/];
static float active_z/*S_INDEX*/[/*NB_HIDDEN*/];       static float dlossd_z/*S_INDEX*/[/*NB_HIDDEN*/];
static float weight_v/*S_INDEX*/[/*NB_OUTPUTS*/][/*NB_HIDDEN*/];    
static float active_hh/*S_INDEX*/[/*NB_OUTPUTS*/];      static float dlossd_hh/*S_INDEX*/[/*NB_OUTPUTS*/];
static float active_exphh/*S_INDEX*/[/*NB_OUTPUTS*/];   
static float partition/*S_INDEX*/;
static int   sample/*S_INDEX*/;

static float input_history/*S_INDEX*/[HISTORY_CAPACITY][/*NB_INPUTS*/-1];
static int   sample_history/*S_INDEX*/[HISTORY_CAPACITY];
static int   history_len/*S_INDEX*/;
