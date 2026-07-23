#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include "Structs.h"
#include "Graph.h"
#include "Hash_table.h"
#include "Commands.h"


int main(int argc, char* argv[]) {
    char *inputFile = NULL, *outputFile = NULL;

    
    // we read the parameters(here -i for input -o for output) of the miris execution command
    // the order is random
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            inputFile = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0) {
            outputFile = argv[++i];
        }
    }


    // The function should be called only with input and output file
    if (inputFile == NULL || outputFile == NULL) {
        fprintf(stderr, "Format: %s -i {inputfile} -o {outputfile}\n", argv[0]); //right format 
        exit(-1);
    }


    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    
    // made hash table initial size 100 because I didin't implemented rehasing/resizing 
    // This way I avoid two many big list of hashnodes in a table position after hash function calculation 
    // but either way, my programm works fine ,with lets say 5 initial size, but with enormous collision list that aftect complexity
    ht->size = 100;
    ht->table = (HashNode**)calloc(1000, sizeof(HashNode*));

    Graph* graph = createGraph(5);


    readInputFile(inputFile, graph, ht);


    char command[256];
    while (1) {
        printf("miris> "); //our miris prompt
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // if user gives  EOF (Ctrl+D), we terminate the programm
        }

        // delete new line \n in order not to proccess it
        command[strcspn(command, "\n")] = '\0';

        processCommand(graph, ht,outputFile, command);
    }

    return 0;
}

