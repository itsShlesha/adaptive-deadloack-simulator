#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "banker.h"
#include "rag.h"
#include "deadlock.h"
#include "simulation.h"

/*
run_periodic_sweep() is called in 2 situations: 
1. Automatically from run_random_simulation() every SWEEP_INTERVAL confirmed allocations
2. Manually if the user wants a standalone sweep check

Banker's Algorithm checks each request in isolation.
A series of individually safe aloactions, especially after the dynamic Max update, can altogether
drift the system towards a dangerous state. This sweep catches that.
*/

void run_periodic_sweep(){
    printf("\n-----------------------------------\n");
    printf(" [SWEEP] Periodic Deadlock Detection Running...\n");

    //rebuild RAG from current state and then run DFS
    build_rag();
    
    if(has_cycle()){
        if(sys.mode == SINGLE_INSTANCE) printf(" [SWEEP] WARNING: Cycle detected - DEADLOCK CONFIRMED.\n");
        else printf(" [SWEEP] WARNING: Cycle detected - POTENTIAL DEADLOCK.\n"
        " [SWEEP] Run Banker's Algorithm to confirm.\n");
    }else{
        printf(" [SWEEP] Clean - no cycle detcted in RAG.\n");
    }

    //also run Banker's in multi-instance mode for full accuracy
    if(sys.mode==MULTI_INSTANCE){
        int safe_seq[MAX_P];
        if(is_safe(safe_seq)) printf(" [SWEEP] Banker's confirms: SAFE STATE.\n");
        else printf(" [SWEEP] Banker's confirms: UNSAFE STATE detected.\n");
    }
    printf("\n-----------------------------------\n");
}

/*
run_random_simulation(), for the sake of simplicity, keeps random within Need.
*/

void run_random_simulation(){
    int i, j;
    int n; //no. of simulation rounds
    int round; //current round counter
    int pid; //randomly chosen process
    int request[MAX_R]; //randomly generated request vector

    printf("\n-----------------------------------\n");
    printf("      RANDOM REQUEST SIMULATION\n");
    printf("\n-----------------------------------\n");

    //seed random number generator once
    srand((unsigned int)time(NULL));

    printf("Enter number of simulation rounds: ");
    scanf("%d", &n);

    if(n<=0){
        printf("Invalid Number of Rounds.\n");
        return;
    }

    printf("\nSimulating %d rounds...\n",n);
    printf("  (Sweep runs every %d confirmed allocations)\n\n", SWEEP_INTERVAL);

    for(round = 1; round<=n; round++){
        printf("  Round %d:\n",round);

        pid = rand()%sys.num_processes;
        printf("   Process Selected: P%d\n",pid);

        int all_zero = 1; //tracks if random request vector is all zeroes
        for(j=0; j<sys.num_resources; j++){
            if(sys.need[pid][j] > 0) {
                request[j] = rand() %(sys.need[pid][j]+1);
                if(request[j] > 0) all_zero = 0;
            }else{
                request[j] = 0;
            }
        }

        printf("    Request vector   : [ ");
        for (j = 0; j < sys.num_resources; j++)
            printf("%d ", request[j]);
        printf("]\n");

        if (all_zero) {
            printf("    Result           : SKIPPED (zero request)\n\n");
            continue;
        }

        //check request <= available
        int enough = 1;
        for(j=0; j<sys.num_resources; j++){
            if(request[j]>sys.available[j]){ enough = 0; break; }
        }

        if(!enough){
            printf("    Result           : DENIED (insufficient available)\n\n");
            continue;
        }

        //temporary allocation
        for (j = 0; j < sys.num_resources; j++) {
            sys.available[j]       -= request[j];
            sys.allocation[pid][j] += request[j];
            sys.need[pid][j]       -= request[j];
        }

        //running banker's
        int safe_seq[MAX_P];
        int safe = is_safe(safe_seq);
 
        if (safe) {
            printf("    Result           : APPROVED\n");
            printf("    Safe sequence    : ");
            for (i = 0; i < sys.num_processes; i++) {
                printf("P%d", safe_seq[i]);
                if (i < sys.num_processes - 1) printf(" → ");
            }
            printf("\n\n");
 
            /* increment counter and check if sweep is due */
            sys.alloc_counter++;
            if (sys.alloc_counter % SWEEP_INTERVAL == 0)
                run_periodic_sweep();
 
        } else {
            /* rollback */
            for (j = 0; j < sys.num_resources; j++) {
                sys.available[j]       += request[j];
                sys.allocation[pid][j] -= request[j];
                sys.need[pid][j]       += request[j];
            }
            printf("    Result           : DENIED (unsafe state)\n\n");
        }
    }
    printf("Simulation complete.\n");
    printf("Total confirmed allocations this session: %d\n", sys.alloc_counter);
    printf("\n-----------------------------------\n");
}