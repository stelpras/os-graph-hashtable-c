void readInputFile(const char* inputFile, Graph* graph, HashTable* ht);
void writeOutputFile(const char* outputFile, Graph* graph);

void processInsertCommand(Graph* graph, HashTable* ht, char* command);
void processAddEdgeCommand(Graph* graph, HashTable* ht, char* command);
void processDeleteNodesCommand(Graph* graph, HashTable* ht, char* command);
void processDeleteEdgeCommand(Graph* graph, HashTable* ht, char* command);
void processModifyEdgeCommand(Graph* graph, HashTable* ht, char* command);
void processFindEdgesCommand(Graph* graph, HashTable* ht, char* command);
void processReceivingEdgesCommand(Graph* graph, HashTable* ht, char* command);
void dfsFindCycles(Node* startNode, Node* currentNode, bool* visited, Node** path, int pathIndex);
void processFindCyclesCommand(Graph* graph, HashTable* ht, char* command);
void dfsFindCirclesWithMinimumSum(Node* startNode, Node* currentNode, bool* visited, Node** path, int pathIndex, bool* foundCycle, int minSum);
void processFindCirclesWithMinimumSumCommand(Graph* graph, HashTable* ht, char* nodeName, int minSum);
void traceFlow(Node* currentNode, bool* visited, Node** path, int pathIndex, int currentDepth, int maxDepth);
void processTraceFlowCommand(Graph* graph, HashTable* ht, char* nodeName, int maxDepth);
bool findPath(Node* currentNode, Node* targetNode, bool* visited, Node** path, int* pathIndex);
void printPath(Graph* graph, Node* startNode, Node* targetNode);
void processConnectedCommand(Graph* graph, HashTable* ht, char* srcName, char* destName);

void processCommand(Graph* graph, HashTable* ht,char* outputfile, char* command);