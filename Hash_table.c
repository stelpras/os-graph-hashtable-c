#include <stdio.h>
#include "Structs.h"


// calculation of hash function. Found it here: https://stackoverflow.com/questions/2571683/djb2-hash-function
//This gives us the position in our the table we should insert or search or delete the key on our hash table
unsigned long hash(const char *key, int size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // shift and reduce
    }

    return hash % size; //modulo with size for hash value to be inside the size limit of the hash table 
}


void insert(HashTable* ht, char* id, Node* node) {
    unsigned int index = hash(id, ht->size); //calculation of hash value of the new id. 
    

    HashNode* newHashNode = (HashNode*)malloc(sizeof(HashNode));
    newHashNode->node = node;
    newHashNode->next = ht->table[index]; //To avoid collision we insert newnode at the start of the list
    ht->table[index] = newHashNode; //insert the new node in position [index]
}

Node* search(HashTable* ht, char* id) {
    unsigned int index = hash(id, ht->size);
    HashNode* current = ht->table[index];

    //we interate through the list until we find the node(or not) in position [index] after hash
    while (current != NULL) {
        if (strcmp(current->node->name, id) == 0) {
            return current->node; // node found

        }
        current = current->next;
    }

    return NULL; // node not found
}




void deleteNodeFromHashTable(HashTable* ht, char* id) {
    unsigned int index = hash(id, ht->size);
    HashNode* current = ht->table[index];
    HashNode* prev = NULL;

    //we search the node through the list in [index] position
    //if current node isn't the desired one, 
    //we make prev node to point to current and current to the next
    while (current != NULL && strcmp(current->node->name, id) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return; // node not found
    }

    //else delete it
    if (prev == NULL) {
        ht->table[index] = current->next;
    } else {
        prev->next = current->next;
    }

    free(current);
}




size_t freeHashTable(HashTable* ht) {
    size_t bytesFreed = 0;

    for (int i = 0; i < ht->size; i++) {
        HashNode* current = ht->table[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            bytesFreed += sizeof(*temp);
            free(temp);
        }
    }

    bytesFreed += sizeof(*ht->table) * ht->size;
    free(ht->table);
    bytesFreed += sizeof(*ht);
    free(ht);

    return bytesFreed;
}
