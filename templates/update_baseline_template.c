// update baseline:
reward_var += ((reward-reward_exp)*(reward-reward_exp) - reward_var) / BASELINE_AVG_TIMESCALE;
reward_exp += (reward - reward_exp) / BASELINE_AVG_TIMESCALE;
