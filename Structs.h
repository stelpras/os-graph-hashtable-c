#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <stdbool.h>


typedef struct Edge {
    int weight;  
    char* date;  
    struct Node* dest;  // pointer to destination node
    struct Edge* next;  // pointer to next edge in list
} Edge;


typedef struct Node {
    char* name;  
    struct Edge* edges;  // pointer to list of edges that start from the node
    int index; //I use an index for quick reference(used in dfs's algorithms to find circles, paths etc.)
} Node;

typedef struct Graph {
    int numNodes;
    int capacity;
    Node** nodes; //I use a table that has pointers to every node of the graph
} Graph;




typedef struct HashNode {
    Node* node; //pointer to a graph node
    struct HashNode* next; //pointer to the next node of the same list
} HashNode;

typedef struct HashTable {
    int size;
    HashNode** table; //a table of pointers to HashNodes that represent hashtable
} HashTable;





