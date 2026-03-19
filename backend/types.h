#ifndef TYPES_H
#define TYPES_H

#define MAX_P 5 //maximum number of processes
#define MAX_R 5 //maximum number of resources
#define SWEEP_INTERVAL 3 //run detection sweep every N allocs

typedef enum{
    SINGLE_INSTANCE = 0, //all resources have exactly 1 copy
    MULTI_INSTANCE = 1 //atleast once resource has >1 copy
} SystemMode;

typedef struct{
    //dimensions of matrices used further
    int num_processes;
    int num_resources;

    //resource instance counts
    int total[MAX_R]; //total instances of each resource
    int available[MAX_R]; //currently free resources
    //relationship observed: total = allocated + available

    //matrices [process][resources]
    int allocation[MAX_P][MAX_R];
    int max_need[MAX_P][MAX_R];
    int need[MAX_P][MAX_R];
    //relationship observed: need = max_need - allocation

    int rag[MAX_P + MAX_R][MAX_P + MAX_R];

    //adaptive mode flag
    SystemMode mode;

    //allocation counter for periodic sweep
    int alloc_counter;

    //intialised flag
    int initialised;
} SystemState;

extern SystemState sys;

#endif