#include <stdio.h>
#include "types.h"
#include "banker.h"

int is_safe(int safe_seq[MAX_P]){
    int i,j,k;

    //work[] is a copy of available: simulation done on this rather than the real sys state
    int work[MAX_R];
    for(j=0; j<sys.num_resources; j++)
        work[j] = sys.available[j];

    //finish[] tracks whether eahc process has "finished" in simulation
    int finish[MAX_P];
    for(i=0; i<sys.num_processes; i++)
        finish[i] = 0;
    
    //counts how many processes have been added to the safe sequence
    int count = 0;

    /*
    keep looping until we either
        finish all processes
    OR
        make a full pass without any runnable process
    */
   while(count < sys.num_processes){
    int found = 0;    //did we find atleast one runnable process this pass?

    for(i=0; i<sys.num_processes; i++){

        if(finish[i]==1) continue; //skip already-finished processes

        int can_run = 1;
        //check if need[i] <= work for ALL processes
        for(j=0; j<sys.num_resources; j++){
            if(sys.need[i][j] > work[j]){ can_run = 0; break; }
        }

        if(can_run){
            //release its allocation back into work
            for(j=0; j<sys.num_resources; j++){
                work[j] += sys.allocation[i][j];
            }

            finish[i] = 1;
            safe_seq[count] = i; //record processNo. into safe-seq
            count++;
            found = 1;
        }
    }

    if(!found) break;
   }
   return (count == sys.num_processes) ? 1 : 0;
}

void run_banker(){
    int safe_seq[MAX_P];
    int i;

    printf("\n-----------------------------------\n");
    printf("       BANKER'S ALGORITHM - SAFE CHECK       \n");
    printf("\n-----------------------------------\n");

    if(is_safe(safe_seq)){
        printf("  Result : SAFE STATE\n");
        printf("  Safe Sequence: ");
        for(i=0; i<sys.num_processes; i++){
            printf("P%d",safe_seq[i]);
            if(i<sys.num_processes-1) printf(" -> ");
        }
        printf("\n");
    }else{
        printf("  Result : UNSAFE STATE\n");
        printf("  No Safe Sequence Exists.\n");
        printf("  System is AT RISK of DEADLOCK.\n");
    }

    printf("\n-----------------------------------\n");
}