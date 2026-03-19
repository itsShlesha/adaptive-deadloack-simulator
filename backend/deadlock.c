#include <stdio.h>
#include "types.h"
#include "rag.h"
#include "deadlock.h"

static int visited[MAX_P + MAX_R]; //node has been atleast seen once in DFS sequence
static int rec_stack[MAX_P + MAX_R]; //node is present on the current path of DFS
/*
visited alone just tells us that node has been seen.
rec_stack tracks the CURRENT PATH, meaning: 
        if we found a node that's already on the current path
            we found a back edge -> cycle existence
*/

static int dfs(int node, int total_nodes){
    int neighbor;

    //mark node visted and add to current path
    visited[node] = 1; rec_stack[node] = 1;

    //explore all neighbors
    for(neighbor = 0; neighbor < total_nodes; neighbor++){
        //no edge to neighbor? skip.
        if(sys.rag[node][neighbor]==0) continue;

        //neighbor not visited yet? recurse.
        if(!visited[neighbor]){
            if(dfs(neighbor, total_nodes)) return 1; //cycle found deeper in recursion
        }

        //neighbor IS on current path? cycle exists.
        else if(rec_stack[neighbor]) return 1;
    }
}

int has_cycle(){
    int i; 
    int total_nodes = sys.num_processes + sys.num_resources;

    //reset state for fresh run
    for(i=0; i<total_nodes; i++){
        visited[i] = 0;
        rec_stack[i] = 0;
    }

    //start dfs from every unvisited node
    for(i=0; i<total_nodes; i++){
        if(!visited[i]){
            if(dfs(i,total_nodes)) return 1;
        }
    }
    return 0;
    //necessary because the graph may not be fully connected (isolated nodes would be missed otherwise)
}

void detect_deadlock(){
    printf("\n-----------------------------------\n");
    printf("       DEADLOCK DETECTION - DFS\n");
    printf("\n-----------------------------------\n");

    //always rebuild rag to reflect latest state
    build_rag();

    if(has_cycle()){
        if(sys.mode == SINGLE_INSTANCE){
            printf("  Cycle detected in RAG.\n");
            printf("  Mode    : Single-Instance\n");
            printf("  Result  : DEADLOCK CONFIRMED\n");
            printf("  Reason  : In single-instance systems,\n");
            printf("            a cycle in RAG guarantees deadlock.\n");
        }else{
            printf("  Cycle detected in RAG.\n");
            printf("  Mode    : Multi-Instance\n");
            printf("  Result  : POTENTIAL DEADLOCK (not guaranteed)\n");
            printf("  Reason  : In multi-instance systems, a cycle\n");
            printf("            is necessary but not sufficient.\n");
            printf("  Action  : Run Banker's Algorithm to confirm.\n");
        }
    }else{
        printf("  No cycle detected in RAG.\n");
        printf("  Result  : NO DEADLOCK\n");
        printf("  System appears to be in a safe state.\n");
    }
    printf("\n-----------------------------------\n");
}