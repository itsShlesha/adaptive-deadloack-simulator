#include <stdio.h>
#include "types.h"
#include "rag.h"

void build_rag(){
    int i,j;
    int total_nodes = sys.num_processes + sys.num_resources;

    //clear entire matrix first
    for(i=0; i<total_nodes; i++){
        for(j=0; j<total_nodes; j++){
            sys.rag[i][j] = 0;
        }
    }

    for(i=0; i<sys.num_processes; i++){
        for(j=0; j<sys.num_resources; j++){

            int p_node = i; //process node index
            int r_node = sys.num_processes + j; //resource node index

            //request edge: P->R (rag[p][r])
            if(sys.need[i][j] > 0) sys.rag[p_node][r_node] = 1;

            //assignment edge: R->P (rag[r][p])
            if(sys.allocation[i][j] > 0) sys.rag[r_node][p_node] = 1;
        }
    }
}

void print_rag(){
    int i,j;
    int total_nodes = sys.num_processes + sys.num_resources;

    printf("\n-----------------------------------\n");
    printf("       RESOURCE ALLOCATION GRAPH (RAG)\n");
    printf("\n-----------------------------------\n");

    if(sys.mode == MULTI_INSTANCE)
    printf(" [NOTE] Multi-Instance mode - RAG is for visualisation only.\n");
    printf("        Use BANKER'S ALGORITHM for accuracy.\n\n");

    //column headers
    printf("       ");
    for(j=0; j<total_nodes; j++){
        if(j<sys.num_processes) printf(" P%d ",j);
        else printf(" R%d", j-sys.num_processes);
    }
    printf("\n");

    //separator line
    printf("       ");
    for (j = 0; j < total_nodes; j++)
        printf("----");
    printf("\n");

    /* rows */
    for (i = 0; i < total_nodes; i++) {
        if (i < sys.num_processes)
            printf("  P%d | ", i);
        else
            printf("  R%d | ", i - sys.num_processes);
 
        for (j = 0; j < total_nodes; j++)
            printf(" %d  ", sys.rag[i][j]);
        printf("\n");
    }

    printf("\n-----------------------------------\n");
}

void print_rag_edges(void)
{
    int i, j;
    int has_edges = 0;
 
    printf("\n  Edge List:\n");
 
    // request edges: P → R 
    for (i = 0; i < sys.num_processes; i++) {
        for (j = 0; j < sys.num_resources; j++) {
            if (sys.need[i][j] > 0) {
                printf("  P%d  →  R%d  \n",
                       i, j);
                has_edges = 1;
            }
        }
    }
 
    // assignment edges: R → P 
    for (i = 0; i < sys.num_processes; i++) {
        for (j = 0; j < sys.num_resources; j++) {
            if (sys.allocation[i][j] > 0) {
                printf("  R%d  →  P%d \n",
                       j, i);
                has_edges = 1;
            }
        }
    }
 
    if (!has_edges)
        printf("  No edges — graph is empty.\n");
 
    printf("\n-----------------------------------\n");
}