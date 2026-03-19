#include <stdio.h>
#include "types.h"
#include "display.h"


//prints "      R0   R1   R2 ..." aligned to matrix columns
static void print_header(){
    int j;
    printf("       ");
    for(j=0; j<sys.num_resources;j++) printf(" R%d",j);
    printf("\n");
}

//prints allocation matrix row by row
void print_allocation(){
    int i, j;
    printf("\n  ALLOCATION MATRIX:  \n");
    print_header();
    for(i=0; i<sys.num_processes; i++){
        printf("   P%d", i);
        for(j=0; j<sys.num_resources; j++)
            printf(" %2d ",sys.allocation[i][j]);
        printf("\n");
    }
}

//prints max matrix row by row
void print_max(){
    int i, j;
    printf("\n  MAX MATRIX:\n");
    print_header();
    for (i = 0; i < sys.num_processes; i++) {
        printf("   P%d  ", i);
        for (j = 0; j < sys.num_resources; j++)
            printf(" %2d ", sys.max_need[i][j]);
        printf("\n");
    }
}

//prints need matrix row by row
void print_need(){
    int i, j;
    printf("\n  Need Matrix:\n");
    print_header();
    for (i = 0; i < sys.num_processes; i++) {
        printf("   P%d  ", i);
        for (j = 0; j < sys.num_resources; j++)
            printf(" %2d ", sys.need[i][j]);
        printf("\n");
    }
}


//prints Available as a single row, and mode indicator
void print_available(){
    int j;
    printf("\n    AVAILABLE RESOURCES: \n");
    print_header();
    printf("        ");
    for(j=0; j<sys.num_resources; j++)
        printf(" %2d ",sys.available[j]);
    printf("\n");

    printf("\n   TOTAL INSTANCES: \n");
    print_header();
    printf("        ");
    for (j = 0; j < sys.num_resources; j++)
        printf(" %2d ", sys.total[j]);
    printf("\n");
}

//displays system_state
void display_system_state(){
    printf("\n-----------------------------------\n");
    printf("       CURRENT SYSTEM STATE       \n");
    printf("\n-----------------------------------\n");

    printf("  Processes   : %d\n", sys.num_processes);
    printf("  Resources   :%d\n", sys.num_resources);
    printf("  Mode        : %s\n", 
        sys.mode == SINGLE_INSTANCE ? "Single-Instance" : "Multi-Instance");
    
    print_allocation();
    print_max();
    print_need();
    print_available();

    printf("\n-----------------------------------\n");
}