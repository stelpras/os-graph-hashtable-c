unsigned long hash(const char *key, int size);
void insert(HashTable* ht, char* id, Node* node);
Node* search(HashTable* ht, char* id);
void deleteNodeFromHashTable(HashTable* ht, char* id);
size_t freeHashTable(HashTable* ht);
