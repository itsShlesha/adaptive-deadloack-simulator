#ifndef SIMULATION_H
#define SIMULATION_H
 
#include "types.h"

// run N random resource requests, print approved/denied each
void run_random_simulation();

//run deadlock detection sweep
void run_periodic_sweep();

#endif