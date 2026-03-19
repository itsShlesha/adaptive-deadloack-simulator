#include <stdio.h>
#include "types.h"
#include "init.h"
#include "banker.h"
#include "request.h"

void request_resource(void){
    int i,j;
    int pid;
    int request[MAX_R];

    printf("\n-----------------------------------\n");
    printf("       RESOURCE REQUEST SIMULATION\n");
    printf("\n-----------------------------------\n");

    //Step 1: get pid
    printf("Enter Process ID (0 to %d): ",sys.num_processes-1);
    scanf("%d", &pid);

    if(pid<0 || pid>=sys.num_processes){
        printf("Invalid Process ID.\n"); return;
    }

    //step 2: get request vector
    printf("Enter Request Vector for P%d:\n",pid);
    for(j=0; j<sys.num_resources; j++){
        printf("  R%d: ",j);
        scanf("%d", &request[j]);
    }

    printf("\n  Request by P%d: [ ",pid);
    for(j=0; j<sys.num_resources; j++) printf("%d ",request[j]);
    printf(" ]\n");

    //step 3: check request vs need
    /*
    TWO POSSIBILITIES HERE:
    1) New Process with known max_need where
        request <= need -> NORMAL BANKER'S VALIDATION
    2) Dynamic Process with updated max_need where
        request > need -> DYNAMIC MAX UPDATE
    */

    //check if ANY resource exceeds need
    int exceed_need = 0;
    for(j=0; j<sys.num_resources; j++){
        if(request[j] > sys.need[pid][j]){
            exceed_need = 1; break;
        }
    }

    //save original Max and Need for rollback
    int old_max[MAX_R];
    int old_need[MAX_R];
    for(j=0; j<sys.num_resources; j++){
        old_max[j] = sys.max_need[pid][j];
        old_need[j] = sys.need[pid][j];
    }


    if(exceed_need){
        //2) DYNAMIC MAX UPDATE
        printf("\n  Request exceeds declared Need. \n");
        printf("  Attempting dynamic Max Update....\n");
            
        //update Max and Need to respond to the new request
        for(j=0; j<sys.num_resources; j++){
            if(request[j]>sys.need[pid][j]){
                sys.max_need[pid][j] = sys.allocation[pid][j] + request[j];
                sys.need[pid][j] = request[j];
            }
        }

        printf(" Max updated for P%d: [ ", pid);
        for (j = 0; j < sys.num_resources; j++)
            printf("%d ", sys.max_need[pid][j]);
        printf("]\n");
    }else{
        //1) NORMAL - REQUEST WITHIN NEED
        printf("  Request is within declared Need. Proceeding...\n");
    }

    for(j=0; j<sys.num_resources; j++){
    if(request[j] > sys.available[j]){
        printf("  Not enough resources available. Process must wait.\n");
        return;
    }
}

    //step 4: temporary allocation
    for(j=0; j<sys.num_resources; j++){
        sys.available[j] -= request[j];
        sys.allocation[pid][j] += request[j];
        sys.need[pid][j] -= request[j];
    }

    //step 5: run Banker's on new state
    printf("\n  Running Banker's Algorithm on new state...\n");

    int safe_seq[MAX_P];
    int safe = is_safe(safe_seq);

    //step 6: confirm or rollback
    if (safe) {
        printf("\n  Result  : SAFE — Allocation confirmed.\n");
        printf("  Safe Sequence: ");
        for (i = 0; i < sys.num_processes; i++) {
            printf("P%d", safe_seq[i]);
            if (i < sys.num_processes - 1)
                printf(" → ");
        }
        printf("\n");
    
        //incrementing allocation counter for periodic sweep
        sys.alloc_counter++;
        printf("Allocation counter: %d / %d\n", sys.alloc_counter, SWEEP_INTERVAL);
    }else{
        printf("\n  Result  : UNSAFE — Rolling back.\n");

        //rollback alloacation
        for (j = 0; j < sys.num_resources; j++) {
            sys.available[j] += request[j];
            sys.allocation[pid][j] -= request[j];
        }

        //rollback Max and Need to saved originals
        for (j = 0; j < sys.num_resources; j++) {
            sys.max_need[pid][j] = old_max[j];
            sys.need[pid][j] = old_need[j];
        }

        printf("  All matrices restored to pre-request state.\n");
        printf("  Process P%d must wait.\n", pid);
    }
    printf("\n-----------------------------------\n");
}