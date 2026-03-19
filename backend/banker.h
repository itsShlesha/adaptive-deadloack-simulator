#ifndef BANKER_H
#define BANKER_H

#include "types.h"

/*
safety check - worls on current sys state
return 1 if safe, 0 is unsafe
also fills safe_seq[] with the safe sequence, if found
*/
int is_safe(int safe_seq[MAX_P]);

//run safety check and print result with sequence
void run_banker();

#endif