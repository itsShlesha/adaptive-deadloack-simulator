#ifndef INIT_H
#define INIT_H

#include "types.h"

//prompting user for all inputs, building matrices, detecting modes
void init_system();

/*
calculating need[i][j] = max_need[i][j] - allocation[i][j]
also recalculates available from total and allocation 
(available = total - allocation)
*/
void calculate_need();

/*
inspect total[] - if any resource has >1 instance
set sys.mode = MULTI_INSTANCE, else SINGLE_INSTANCE
*/
void detect_mode();

#endif