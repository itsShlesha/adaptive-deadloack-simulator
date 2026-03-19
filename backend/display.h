#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

//print all four matrices in a clean tabular manner
void display_system_state();

//individual printers
void print_allocation();
void print_max();
void print_need();
void print_available();

#endif