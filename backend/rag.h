#ifndef RAG__H
#define RAG_H

#include "types.h"

//build RAG adjacency matrix from current allocation + need
void build_rag();

//print rag
void print_rag();

//print a human-readable list of all edges in the graph
void print_rag_edges();

#endif