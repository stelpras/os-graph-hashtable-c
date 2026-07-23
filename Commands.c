#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <stdbool.h>
#include "Structs.h"
#include "Graph.h"
#include "Hash_table.h"




void readInputFile(const char* inputFile, Graph* graph, HashTable* ht) {
    FILE *file = fopen(inputFile, "r"); //file open with read rights
    if (!file) {
        perror("Failed to open input file");
        exit(-1);
    }

    char line[256];
    //we use sscanf to seperate the string - command line of our file and the we add the nodes and the edges in our graph 
    while (fgets(line, sizeof(line), file)) {
        char srcName[50], destName[50], date[20];
        int weight;
        if (sscanf(line, "%s %s %d %s", srcName, destName, &weight, date) == 4) {
            if (search(ht, srcName) == NULL) { // Search returned null instead of a found node, so node not exists
                addNode(graph, ht, srcName);
            }
            if (search(ht, destName) == NULL) { //again with the same practise, we do the insertion if node not exists
                addNode(graph, ht, destName);
            }
            addEdge(graph, srcName, destName, weight, date);
        } else {
            fprintf(stderr, "Invalid line format: %s", line);
        }
    }

    fclose(file);
}


void writeOutputFile(const char* outputFile, Graph* graph) {
    FILE *file = fopen(outputFile, "w");//file open or create with write rights
    if (!file) {
        perror("Failed to open output file");
        exit(-1);
    }


    //we make a new output file and we print there our graph after the commands
    for (int i = 0; i < graph->capacity; i++) {
        if (graph->nodes[i] != NULL) {
            fprintf(file, "Node %s:\n", graph->nodes[i]->name);
            Edge* current = graph->nodes[i]->edges;
            while (current != NULL) {
                fprintf(file, "    Edge to: %s, Amount: %d, Date: %s\n", current->dest->name, current->weight, current->date);
                current = current->next;
            }
        }
    }

    fclose(file);
}





void processInsertCommand(Graph* graph, HashTable* ht, char* command) {
    char* token = strtok(command, " ");
    char* nodes[256];
    int count = 0;

    // divide the input into tokens and insert the nodes
    while (token != NULL) {
        if (strcmp(token, "i") != 0 && strcmp(token, "insert") != 0) {
            nodes[count++] = token;
        }
        token = strtok(NULL, " ");
    }

    char success[2000] = "Succ:";
    char issue[2000] = "IssueWith:";
    bool hasIssue = false;
    bool suc=false;

    //to merge un/successfull nodes strings I use strcat and I print it in the terminal after the command
    for (int i = 0; i < count; i++) {
        if (search(ht, nodes[i]) != NULL) { //node exists in graph
            strcat(issue, " ");
            strcat(issue, nodes[i]);
            hasIssue = true;
        } 
        if (search(ht, nodes[i]) == NULL) { //node not exist in graph
            addNode(graph, ht, nodes[i]);
            strcat(success, " ");
            strcat(success, nodes[i]);
            suc=true;
        }
    }

    if (hasIssue) {
        printf("%s\n", issue);
    } 
    if (suc){
        printf("%s \n", success);
    }
}

void processAddEdgeCommand(Graph* graph, HashTable* ht, char* command) {
    char* token = strtok(command, " ");
    char* srcNode;
    char* destNode;
    int weight;
    char date[20];

    // divide the input of the command after seperation with a space
    if (token != NULL && (strcmp(token, "n") == 0 || strcmp(token, "insert2") == 0)) {
        srcNode = strtok(NULL, " ");
        destNode = strtok(NULL, " ");
        // I make weight an integer (from string) with atoi
        weight = atoi(strtok(NULL, " "));
        strcpy(date, strtok(NULL, " "));
    }

    if (srcNode == NULL || destNode == NULL || weight == 0 || strlen(date) == 0) {
        printf("IssueWith: %s %s\n", srcNode, destNode);
        return;
    }

    //if nodes aren't in our hash table(this also means in our graph) we add them
    if (search(ht, srcNode) == NULL) {
        addNode(graph, ht, srcNode);
    }
    if (search(ht, destNode) == NULL) {
        addNode(graph, ht, destNode);
    }
    addEdge(graph, srcNode, destNode, weight, date);
    // printf("Added edge from %s to %s with weight %d and date %s\n", srcNode, destNode, weight, date);
}

void processDeleteNodesCommand(Graph* graph, HashTable* ht, char* command) {
    char* token = strtok(command, " ");
    char* nodes[256];
    int count = 0;

    // divide the command an save the nodes to be deleted in nodes table
    while (token != NULL) {
        if (strcmp(token, "d") != 0 && strcmp(token, "delete") != 0) {
            nodes[count++] = token;
        }
        token = strtok(NULL, " ");
    }

    char nonExistingNodes[256] = "Non-existing node(s):";
    bool hasNonExisting = false;

    for (int i = 0; i < count; i++) {
        if (search(ht, nodes[i]) == NULL) {
            strcat(nonExistingNodes, " ");
            strcat(nonExistingNodes, nodes[i]);
            hasNonExisting = true;
        } 
        if (search(ht, nodes[i]) != NULL) {
            deleteNodeFromGraph(graph, ht, nodes[i]);
        }
    }

    if (hasNonExisting) {
        printf("%s\n", nonExistingNodes);
    }
}

void processDeleteEdgeCommand(Graph* graph, HashTable* ht, char* command) {
    char srcName[50], destName[50];
    sscanf(command, "%*s %s %s", srcName, destName);

    if (search(ht, srcName) == NULL || search(ht, destName) == NULL) {
        char issue[256] = "Non-existing node(s):";
        if (search(ht, srcName) == NULL) {
            strcat(issue, " ");
            strcat(issue, srcName);
        }
        if (search(ht, destName) == NULL) {
            strcat(issue, " ");
            strcat(issue, destName);
        }
        printf("%s\n", issue);
        return;
    }

    deleteEdgeFromGraph(graph, ht, srcName, destName);
    //printf("Deleted edge from %s to %s\n", srcName, destName);
}

void processModifyEdgeCommand(Graph* graph, HashTable* ht, char* command) {
    char srcName[50], destName[50], date[20], newDate[20];
    int weight, newWeight;
    sscanf(command, "%*s %s %s %d %d %s %s", srcName, destName, &weight, &newWeight, date, newDate);

    if (search(ht, srcName) == NULL || search(ht, destName) == NULL) {
        char issue[256] = "Non-existing node(s):";
        if (search(ht, srcName) == NULL) {
            strcat(issue, " ");
            strcat(issue, srcName);
        }
        if (search(ht, destName) == NULL) {
            strcat(issue, " ");
            strcat(issue, destName);
        }
        printf("%s\n", issue);
        return;
    }

    Node* srcNode = search(ht, srcName);
    Edge* current = srcNode->edges;
    bool edgeFound = false;

    while (current != NULL) {
        if (strcmp(current->dest->name, destName) == 0 && current->weight == weight && strcmp(current->date, date) == 0) {
            current->weight = newWeight;
            free(current->date);
            current->date = strdup(newDate); //I use strdup to make a copy of newDate in memory
            edgeFound = true;
            //printf("Modified edge from %s to %s with new weight %d and new date %s\n", srcName, destName, newWeight, newDate);
            break;
        }
        current = current->next;
    }

    if (!edgeFound) {
        printf("Non-existing edge: %s %s %d %s\n", srcName, destName, weight, date);
    }
}

void processFindEdgesCommand(Graph* graph, HashTable* ht, char* command) {
    char nodeName[50];
    sscanf(command, "%*s %s", nodeName);

    if (search(ht, nodeName) == NULL) {
        printf("Non-existing node: %s\n", nodeName);
        return;
    }

    Node* node = search(ht, nodeName);
    Edge* current = node->edges;

    while (current != NULL) {
        printf("%s %s %d %s\n", nodeName, current->dest->name, current->weight, current->date);
        current = current->next;
    }
}

void processReceivingEdgesCommand(Graph* graph, HashTable* ht, char* command) {
    char nodeName[50];
    sscanf(command, "%*s %s", nodeName);

    if (search(ht, nodeName) == NULL) {
        printf("Non-existing node: %s\n", nodeName);
        return;
    }

    //we interate through the graph to find receiving edges of the node(edges that the node is a destination)
    for (int i = 0; i < graph->numNodes; i++) {
        Node* currentNode = graph->nodes[i];
        if (currentNode != NULL) {
            Edge* current = currentNode->edges;
            while (current != NULL) {
                if (strcmp(current->dest->name, nodeName) == 0) {
                    printf("%s %s %d %s\n", currentNode->name, nodeName, current->weight, current->date);
                }
                current = current->next;
            }
        }
    }

}


//I use recursion dfs to find circles of a node in the graph
void dfsFindCycles(Node* startNode, Node* currentNode, bool* visited, Node** path, int pathIndex) {
    visited[currentNode->index] = true;  // I use index in nodes instead of searching with the name of each node
    path[pathIndex] = currentNode; //add node in the path
    pathIndex++;

    //we take first edge of current node
    Edge* edge = currentNode->edges;
    while (edge != NULL) { //as long as there are edges we return
        
        Node* nextNode = edge->dest; //we take the destination node
        if (nextNode == startNode) { //if nextNode == startNode we found a circle
            for (int i = 0; i < pathIndex; i++) { //we return to the path and print the circle
                printf("%s ", path[i]->name);
            }
            printf("%s\n", startNode->name);
        } else if (!visited[nextNode->index]) { //or if next node hasn't visited yet, we call dfsFindCycles recursively
            dfsFindCycles(startNode, nextNode, visited, path, pathIndex);
        }
        edge = edge->next;
    }

    //we make current node as not visited in order to participate in other paths
    pathIndex--;
    visited[currentNode->index] = false;  
}


void processFindCyclesCommand(Graph* graph, HashTable* ht, char* nodeName) {
    Node* startNode = search(ht, nodeName);
    if (startNode == NULL) {
        printf("Non-existing node: %s\n", nodeName);
        return;
    }

    // me make a bool visited table for dfs using calloc and a node table path using malloc
    bool* visited = (bool*)calloc(graph->numNodes, sizeof(bool));
    Node** path = (Node**)malloc(graph->numNodes * sizeof(Node*));

    dfsFindCycles(startNode, startNode, visited, path, 0);

    free(visited);
    free(path);
}



//again I use recursive dfs to find circles of a node with a minimum amount of weight in an edge.
void dfsFindCirclesWithMinimumSum(Node* startNode, Node* currentNode, bool* visited, Node** path, int pathIndex, bool* foundCycle, int minSum) {
    visited[currentNode->index] = true;
    path[pathIndex] = currentNode;
    pathIndex++;

    Edge* edge = currentNode->edges;
    while (edge != NULL) {
        Node* nextNode = edge->dest;
        if (nextNode == startNode && edge->weight >= minSum) {
            *foundCycle = true;
            int minEdgeWeight = edge->weight;

            // we find the least amount in the circle
            for (int i = 0; i < pathIndex - 1; i++) {
                Edge* e = path[i]->edges;
                while (e != NULL) {
                    if (e->dest == path[i + 1] && e->weight < minEdgeWeight) {
                        minEdgeWeight = e->weight;
                    }
                    e = e->next;
                }
            }

            // print circle with min amount
            for (int i = 0; i < pathIndex; i++) {
                printf("%s ", path[i]->name);
            }
            printf("%s %d\n", startNode->name, minEdgeWeight);
        } else if (!visited[nextNode->index] && edge->weight >= minSum) {
            dfsFindCirclesWithMinimumSum(startNode, nextNode, visited, path, pathIndex, foundCycle, minSum);
        }
        edge = edge->next;
    }

    pathIndex--;
    visited[currentNode->index] = false;
}



void processFindCirclesWithMinimumSumCommand(Graph* graph, HashTable* ht, char* nodeName, int minSum) {
    Node* startNode = search(ht, nodeName);
    if (startNode == NULL) {
        printf("Non-existing node: %s\n", nodeName);
        return;
    }

    bool foundCycle = false;
    bool* visited = (bool*)calloc(graph->numNodes, sizeof(bool));
    Node** path = (Node**)malloc(graph->numNodes * sizeof(Node*));

    dfsFindCirclesWithMinimumSum(startNode, startNode, visited, path, 0, &foundCycle, minSum);

    free(visited);
    free(path);
}


//function to find paths of lenght m starting of the given node
void traceFlow(Node* currentNode, bool* visited, Node** path, int pathIndex, int currentDepth, int maxDepth) {
    visited[currentNode->index] = true;
    path[pathIndex] = currentNode;
    pathIndex++;
    currentDepth++;

    if (currentDepth > maxDepth) {
        pathIndex--;
        visited[currentNode->index] = false;
        return;
    }

    Edge* edge = currentNode->edges;
    while (edge != NULL) {
        //we take the destination on the current node
        Node* nextNode = edge->dest;

        //if the next node hasn't been visited yes
        if (!visited[nextNode->index]) {
            //we print the path until current node
            for (int i = 0; i < pathIndex; i++) {
                printf("%s ", path[i]->name);
            }
            printf("%s\n", nextNode->name);

            //we continue searching with a recursive call of traceflow with next node and updated path
            traceFlow(nextNode, visited, path, pathIndex, currentDepth, maxDepth);
        }
        edge = edge->next;
    }

    pathIndex--;
    visited[currentNode->index] = false;
}

void processTraceFlowCommand(Graph* graph, HashTable* ht, char* nodeName, int maxDepth) {
    Node* startNode = search(ht, nodeName);
    if (startNode == NULL) {
        printf("Non-existing node: %s\n", nodeName);
        return;
    }

    bool* visited = (bool*)calloc(graph->numNodes, sizeof(bool));
    Node** path = (Node**)malloc(graph->numNodes * sizeof(Node*));

    traceFlow(startNode, visited, path, 0, 0, maxDepth);

    free(visited);
    free(path);
}


//function to identify if there is a path from a given node to another
bool findPath(Node* currentNode, Node* targetNode, bool* visited, Node** path, int* pathIndex) {
    visited[currentNode->index] = true;
    path[(*pathIndex)++] = currentNode;

    if (currentNode == targetNode) { //path found, return true
        return true;
    }

    Edge* edge = currentNode->edges;
    while (edge != NULL) {
        Node* nextNode = edge->dest;
        if (!visited[nextNode->index] && findPath(nextNode, targetNode, visited, path, pathIndex)) { 
            //if next node isn't visited and find path(with nextnode) returns true,
            // the current call of find path also return true
            return true;
        }
        edge = edge->next;
    }

    (*pathIndex)--;
    visited[currentNode->index] = false;
    return false; //no path found
}


void printPath(Graph* graph, Node* startNode, Node* targetNode) {
    bool* visited = (bool*)calloc(graph->numNodes, sizeof(bool));
    Node** path = (Node**)malloc(graph->numNodes * sizeof(Node*));
    int pathIndex = 0;
    if (findPath(startNode, targetNode, visited, path, &pathIndex)) { //if path found, print it
        for (int i = 0; i < pathIndex; i++) {
            printf("%s ", path[i]->name);
        }
        printf("\n");
    } 
    free(visited);
    free(path);
}


void processConnectedCommand(Graph* graph, HashTable* ht, char* srcName, char* destName) {
    Node* startNode = search(ht, srcName);
    Node* targetNode = search(ht, destName);

    if (startNode == NULL) {
        printf("Non-existing node: %s\n", srcName);
        return;
    }
    if (targetNode == NULL) {
        printf("Non-existing node: %s\n", destName);
        return;
    }

    printPath(graph, startNode, targetNode);
}




//for each command we have a diffirent if with the appropriate format errors
//we use strncmp to indentify if our commands start with the disired input
//we use sscanf to extract from the command the desired inputs after the first word (i,insert2,d,modify etc.)
void processCommand(Graph* graph, HashTable* ht,char* outputfile, char* command) {
    if (strncmp(command, "i ", 2) == 0 || strcmp(command, "i") == 0 || strncmp(command, "insert ", 7) == 0 || strcmp(command, "insert") == 0) {
        if (strcmp(command, "i") == 0 || strcmp(command, "insert") == 0) {
            printf("Format error: i/insert Ni Nj ...\n");
            return;
        }
        processInsertCommand(graph, ht, command);
    } else if (strncmp(command, "n ", 2) == 0 || strcmp(command, "n") == 0 || strncmp(command, "insert2 ", 8) == 0 || strcmp(command, "insert2") == 0) {
        char srcName[50], destName[50], date[20];
        int weight;
        char extra[50];

        if (sscanf(command, "%*s %s %s %d %s %s", srcName, destName, &weight, date, extra) != 4) {
            printf("Format error: n/insert2 Ni Nj sum date\n");
            return;
        }
        processAddEdgeCommand(graph, ht, command);
    } else if (strncmp(command, "d ", 2) == 0 || strcmp(command, "d") == 0 || strncmp(command, "delete ", 7) == 0 || strcmp(command, "delete") == 0) {
        char dummy[50];
        if ( strcmp(command, "d") == 0 || strcmp(command, "delete") == 0) {
            printf("Format error: d/delete Ni Nj ...\n");
            return;
        }
        processDeleteNodesCommand(graph, ht, command);
    } else if (strncmp(command, "l ", 2) == 0 || strcmp(command, "l") == 0 || strncmp(command, "delete2 ", 8) == 0 || strcmp(command, "delete2") == 0) {
        char srcName[50], destName[50], extra[50];
        if (sscanf(command, "%*s %s %s %s", srcName, destName, extra) != 2) {
            printf("Format error: l/delete2 Ni Nj\n");
            return;
        }
        processDeleteEdgeCommand(graph, ht, command);
    } else if (strncmp(command, "m ", 2) == 0 || strcmp(command, "m") == 0 || strncmp(command, "modify ", 7) == 0 || strcmp(command, "modify") == 0) {
        char srcName[50], destName[50], date[20], newDate[20], extra[50];
        int weight, newWeight;
        if (sscanf(command, "%*s %s %s %d %d %s %s %s", srcName, destName, &weight, &newWeight, date, newDate , extra) != 6) {
            printf("Format error: m/modify Ni Nj sum sum1 date date1\n");
            return;
        }
        processModifyEdgeCommand(graph, ht, command);
    } else if (strncmp(command, "f ", 2) == 0 || strcmp(command, "f") == 0 || strncmp(command, "find ", 5) == 0 || strcmp(command, "find") == 0) {
        char nodeName[50], extra[50];
        if (sscanf(command, "%*s %s %s", nodeName, extra) != 1) {
            printf("Format error: f/find Ni\n");
            return;
        }
        processFindEdgesCommand(graph, ht, command);
    } else if (strncmp(command, "r ", 2) == 0 || strcmp(command, "r") == 0 || strncmp(command, "receiving ", 10) == 0 || strcmp(command, "receiving") == 0) {
        char nodeName[50], extra[50];
        if (sscanf(command, "%*s %s %s", nodeName, extra) != 1) {
            printf("Format error: r/receiving Ni\n");
            return;
        }
        processReceivingEdgesCommand(graph, ht, command);
    } else if (strncmp(command, "c ", 2) == 0 || strcmp(command, "c") == 0 || strncmp(command, "circlefind ", 11) == 0 || strcmp(command, "circlefind") == 0) {
        char nodeName[50], extra[50];
        if (sscanf(command, "%*s %s %s", nodeName, extra) != 1) {
            printf("Format error: c/circlefind Ni\n");
            return;
        }
        processFindCyclesCommand(graph, ht, nodeName);
    } else if (strncmp(command, "fi ", 3) == 0 || strcmp(command, "fi") == 0 || strncmp(command, "findcircles ", 12) == 0 || strcmp(command, "findcircles") == 0) {
        char nodeName[50], extra[50];
        int minSum;
        if (sscanf(command, "%*s %s %d %s", nodeName, &minSum, extra) != 2) {
            printf("Format error: fi/findcircles Ni k\n");
            return;
        }
        processFindCirclesWithMinimumSumCommand(graph, ht, nodeName, minSum);
    } else if (strncmp(command, "t ", 2) == 0 || strcmp(command, "t") == 0 || strncmp(command, "traceflow ", 10) == 0 || strcmp(command, "traceflow") == 0) {
        char nodeName[50], extra[50];
        int maxDepth;
        if (sscanf(command, "%*s %s %d %s", nodeName, &maxDepth, extra) != 2) {
            printf("Format error: t/traceflow Ni m\n");
            return;
        }
        processTraceFlowCommand(graph, ht, nodeName, maxDepth);
    } else if (strncmp(command, "o ", 2) == 0 || strcmp(command, "o") == 0 || strncmp(command, "connect ", 8) == 0 || strcmp(command, "connect") == 0) {
        char srcName[50], destName[50], extra[50];
        if (sscanf(command, "%*s %s %s %s", srcName, destName, extra) != 2) {
            printf("Format error: o/connected Ni Nj\n");
            return;
        }
        processConnectedCommand(graph, ht, srcName, destName);
    } else if (strcmp(command, "exit" ) == 0 || strncmp(command, "exit ", 5) == 0 || strncmp(command, "e ", 2) == 0 || strcmp(command, "e" ) == 0) {
        char extra[50];
        if (sscanf(command, "%*s %s", extra)== 1){
            printf("Format error: e/exit\n");
            return;
        }
        writeOutputFile(outputfile, graph);
        // free the memory with the use o size_t to find the bytes released
        size_t bytesFreedGraph = freeGraph(graph);
        size_t bytesFreedHashTable = freeHashTable(ht);
        size_t totalBytesFreed = bytesFreedGraph + bytesFreedHashTable;
        printf("%zu Bytes released\n", totalBytesFreed);
        exit(0);
    } else{
        printf("Unrecognized command\n");
    }
}

