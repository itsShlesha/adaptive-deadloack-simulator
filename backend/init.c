#include <stdio.h>
#include "types.h"
#include "init.h"

void calculate_need(){
    int i,j;

    //need matrix
    for(i=0; i<sys.num_processes; i++)
        for(j=0; j<sys.num_resources; j++)
            sys.need[i][j] = sys.max_need[i][j] - sys.allocation[i][j];

    //rebuild available: total-allocation
    for(j=0; j<sys.num_resources; j++){
        sys.available[j]  = sys.total[j];
        for(i=0; i<sys.num_processes; i++)
            sys.available[j] -= sys.allocation[i][j];
    }
}

void detect_mode(){
    int j;
    sys.mode = SINGLE_INSTANCE;  //assuming single first

    for(j=0; j<sys.num_resources; j++){
        if(sys.total[j] > 1){
            sys.mode = MULTI_INSTANCE;
            break;
        }
    }

    if(sys.mode == SINGLE_INSTANCE)
        printf("\n[MODE] Single Instance Resources detected: RAG + DFS is primary\n");
    else
        printf("\n[MODE] Multi-Instance Resources detected: Banker's algorithm is primary.\n");
}

void init_system(){
    int i,j;

    printf("\n-----------------------------------\n");
    printf("       SYSTEM INITIALISATION       ");
    printf("\n-----------------------------------\n");


    //total number of processes user wants to initialise in the system
    do{
        printf("Enter the num of processes (1 - %d): ", MAX_P);
        scanf("%d", &sys.num_processes);
        
        if(sys.num_processes < 1 || sys.num_processes > MAX_P)
            printf("INVALID. Must be between 1 and %d.\n",MAX_P);
    }while(sys.num_processes < 1 || sys.num_processes > MAX_P);


    //total number of resources user wants to initialise in the system
    do{
        printf("Enter the num of resources (1 - %d)",MAX_R);
        scanf("%d", &sys.num_resources);
        
        if(sys.num_resources < 1 || sys.num_resources > MAX_R)
            printf("INVALID. Must be between 1 and %d.\n",MAX_R);
    }while(sys.num_resources < 1 || sys.num_resources > MAX_R);


    //total number of instances for each initialise resource in the system
    printf("\nEnter total instances for each resource: \n");
    for(j=0; j<sys.num_resources; j++){
        do{
            printf("R%d total instances: ", j);
            scanf("%d", &sys.total[j]);

            if(sys.total[j] < 1) printf("Must be atleast 1.\n");
        }while(sys.total[j]<1);
    }


    //allocation matrix
    printf("\nEnter Allocation Matrix (Processes x Resources):\n");
    printf("(How many of each resource each process currently hold)\n");
    for(i=0; i<sys.num_processes; i++){
        printf("P%d: ", i);
        for(j=0; j<sys.num_resources; j++){
            scanf("%d", &sys.allocation[i][j]);
        }
    }


    //max matrix
    printf("\nEnter Max Matrix (Processes x Resources): \n");
    printf("(Maximum each process may ever request)\n");
    for(i=0; i<sys.num_processes; i++){
        printf("P%d: ",i);
        for(j=0; j<sys.num_resources; j++){
            scanf("%d", &sys.max_need[i][j]);
        }
    }

    calculate_need();

    detect_mode();

    sys.alloc_counter = 0;

    sys.initialised = 1;

    printf("\nSystem initialised successfully.\n");
    printf("\n-----------------------------------\n");
}