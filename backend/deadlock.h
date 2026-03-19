#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "types.h"

//runs full deadlock detecttion - builds RAG then runs DFS
void detect_deadlock();

//runs DFS based cycle detection on the RAG and returns 1 if cycle found, else 0
int has_cycle();

#endif