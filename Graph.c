#include <stdio.h>
#include "Structs.h"
#include "Hash_table.h"


Graph* createGraph(int initialCapacity) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numNodes = 0;
    graph->capacity = initialCapacity;
    graph->nodes = (Node**)malloc(initialCapacity * sizeof(Node*));
    for (int i = 0; i < initialCapacity; i++) {
        graph->nodes[i] = NULL;
    }
    return graph;
}

//we resize the graph everytime it gets full
void resizeGraph(Graph* graph) {
    int newCapacity = graph->capacity * 2;
    //printf("Resizing graph from capacity %d to capacity %d\n", graph->capacity, newCapacity);
    
    Node** newNodes = (Node**)realloc(graph->nodes, newCapacity * sizeof(Node*));
    if (newNodes == NULL) {
        exit(-1);
    }
    
    graph->nodes = newNodes;
    graph->capacity = newCapacity;

    for (int i = graph->numNodes; i < graph->capacity; i++) {
        graph->nodes[i] = NULL;
    }
    //printf("Resize complete: new capacity is %d\n", graph->capacity);
}

Node* createNode(char* name, int index) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->name = strdup(name);  // make a copy of string name in memory
    node->edges = NULL;
    node->index = index;  
    return node;
}

void addNode(Graph* graph, HashTable* ht, char* name) {
    if (search(ht, name) != NULL) { //if node is in the hash table, it will also be in the graph
        //printf("Node %s already exists\n", name);
        return;
    }
    if (graph->numNodes >= graph->capacity) {
        resizeGraph(graph);
    }
    Node* node = createNode(name, graph->numNodes);  // each node will have the (int)index of the total number of nodes in graph
    graph->nodes[graph->numNodes++] = node; //add node in table Node** nodes; of the graph. 
    // After that numNodes increases, so next addition will be to the next place in the graph

    // we insert the new node in hash table also
    insert(ht, name, node);
    //printf("Added node %s, total nodes now %d\n", name, graph->numNodes);
}




Edge* createEdge(Node* dest, int weight, char* date) {
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    edge->dest = dest;
    edge->weight = weight;
    edge->date = strdup(date); 
    edge->next = NULL;
    return edge;
}

void addEdge(Graph* graph, char* srcName, char* destName, int weight, char* date) {
    Node* srcNode = NULL;
    Node* destNode = NULL;
    
    for (int i = 0; i < graph->numNodes; i++) {
        if (graph->nodes[i] != NULL && strcmp(graph->nodes[i]->name, srcName) == 0) {
            srcNode = graph->nodes[i];
        }
        if (graph->nodes[i] != NULL && strcmp(graph->nodes[i]->name, destName) == 0) {
            destNode = graph->nodes[i];
        }
    }
    
    if (srcNode == NULL || destNode == NULL) {//two or one nodes not found
        return; 
    }
    
    Edge* edge = createEdge(destNode, weight, date);
    edge->next = srcNode->edges;
    srcNode->edges = edge;
    //printf("Edge from %s to %s added \n",srcNode->name,destName);

}


void deleteEdge(Node* srcNode, char* destName) {
    if (!srcNode || !destName) return;

    Edge* current = srcNode->edges;
    Edge* prev = NULL;
    bool edge_found=false;

    while (current != NULL) {
        if (strcmp(current->dest->name, destName) == 0) {
            if (prev == NULL) {
                srcNode->edges = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->date);  
            Edge* temp = current;
            current = current->next;
            free(temp);  
            edge_found=true;

        } else {
            prev = current;
            current = current->next;
        }
    }
    if (edge_found==false)
    {
        //printf("Edge not found\n");    
    }
}

void deleteEdgeFromGraph(Graph* graph, HashTable* ht, char* srcName, char* destName) {
    for (int i = 0; i < graph->numNodes; i++) {
        if (graph->nodes[i] != NULL && strcmp(graph->nodes[i]->name, srcName) == 0) {
            deleteEdge(graph->nodes[i], destName);
            return;
        }
    }

    
}



size_t deleteOutgoingEdges(Node* node) {
    if (!node) return 0; 
    Edge* current = node->edges;
    Edge* next;
    size_t bytesFreed = 0;
    while (current != NULL) {
        next = current->next;
        bytesFreed += strlen(current->date) + 1;
        free(current->date);  // we free the date because we did a strdup copy at the beginning
        bytesFreed += sizeof(*current); 
        free(current);  //free the edge
        current = next;
    }
    node->edges = NULL;  // make edges null
    return bytesFreed;
}


//here I search in every node of the graph if there is an outgoing edge to the one I'm deleting
//complexity is bad but I didn't had the time to implemented it otherwise
void deleteIncomingEdges(Graph* graph, char* nodeName) {
    for (int i = 0; i < graph->numNodes; i++) {
        Node* currentNode = graph->nodes[i];
        if (currentNode != NULL) {
            Edge* current = currentNode->edges;
            Edge* prev = NULL;
            while (current != NULL) {
                if (strcmp(current->dest->name, nodeName) == 0) {
                    
                    if (prev == NULL) {
                        currentNode->edges = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    free(current->date); 
                    Edge* temp = current;
                    current = current->next;
                    free(temp);  
                } else {
                    prev = current;
                    current = current->next;
                }
            }
        }
    }
}



void deleteNodeFromGraph(Graph* graph, HashTable* ht, char* name) {
    for (int i = 0; i < graph->numNodes; i++) {
        if (graph->nodes[i] != NULL && strcmp(graph->nodes[i]->name, name) == 0) {

            deleteOutgoingEdges(graph->nodes[i]);

            deleteIncomingEdges(graph, name);

            deleteNodeFromHashTable(ht, name);

            //fre the name of the node because we used strdup
            free(graph->nodes[i]->name);
            graph->nodes[i]->name = NULL;

            free(graph->nodes[i]);
            graph->nodes[i] = NULL;
        }
    }
}

size_t freeGraph(Graph* graph) {
    size_t bytesFreed = 0;
    
    for (int i = 0; i < graph->numNodes; i++) {
        if (graph->nodes[i] != NULL) {
            bytesFreed += deleteOutgoingEdges(graph->nodes[i]);
            bytesFreed += strlen(graph->nodes[i]->name) + 1; //+1 for the terminate character /0
            free(graph->nodes[i]->name);  // we free the name of the node because we did strdup at the start
            bytesFreed += sizeof(*graph->nodes[i]);
            free(graph->nodes[i]); 
            graph->nodes[i] = NULL;
        }
    }
    
    bytesFreed += sizeof(*graph->nodes) * graph->capacity;
    free(graph->nodes);  // free memory in nodes table and then the graph
    bytesFreed += sizeof(*graph);
    free(graph);  

    return bytesFreed;
}