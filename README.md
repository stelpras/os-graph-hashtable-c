# os-graph-hashtable-c

Lesson : Operating Systems

Student: Stylianos Prasianakis

Lecturer : Alexios Delis

Programming assignment 1

# Transaction Graph Analyzer (miris)

A C program that models financial transactions as a directed graph — nodes are accounts, edges are transactions carrying an amount and a date — backed by a custom hash table for fast lookups. Supports cycle detection, path finding, flow tracing, and other graph queries via an interactive command prompt.

## Features

- **Graph modeling** — accounts are graph nodes; each transaction is a directed, weighted edge (`amount`, `date`) from source to destination account.
- **Custom hash table** — accounts are indexed by name using a DJB2-based hash function, with separate chaining to resolve collisions, giving average O(1) insert/search/delete.
- **Dynamic graph resizing** — the node table doubles in capacity whenever it fills up.
- **Interactive command prompt** (`miris>`) supporting:
  - Inserting nodes and edges
  - Deleting nodes and edges
  - Modifying existing edges
  - Querying outgoing/incoming edges for a node
  - Cycle detection (plain, and with a minimum-weight threshold)
  - Flow tracing to a configurable depth
  - Path finding and connectivity checks between two nodes
- **File I/O** — reads an initial set of transactions from an input file and writes results to an output file.

## Build & Run

```bash
make
./miris -i <input_file> -o <output_file>
```

Flags can be given in either order (`-i`/`-o`). Both are required.

Clean build artifacts:

```bash
make clean
```

## Commands

Once running, `miris` reads commands from its `miris>` prompt. Each command has both a short and a long form:

| Short | Long | Syntax | Description |
|---|---|---|---|
| `i` | `insert` | `i N1 N2 ...` | Insert one or more new accounts (nodes) |
| `n` | `insert2` | `n Ni Nj sum date` | Add a transaction (edge) from `Ni` to `Nj` |
| `d` | `delete` | `d N1 N2 ...` | Delete one or more accounts (and their edges) |
| `l` | `delete2` | `l Ni Nj` | Delete the transaction from `Ni` to `Nj` |
| `m` | `modify` | `m Ni Nj sum newSum date newDate` | Update the amount/date of an existing transaction |
| `f` | `find` | `f Ni` | List outgoing transactions from `Ni` |
| `r` | `receiving` | `r Ni` | List incoming transactions to `Ni` |
| `c` | `circlefind` | `c Ni` | Find all cycles that start and end at `Ni` |
| `fi` | `findcircles` | `fi Ni k` | Find cycles through `Ni` where every edge weight ≥ `k` |
| `t` | `traceflow` | `t Ni m` | Trace all paths from `Ni` up to depth `m` |
| `o` | `connect` | `o Ni Nj` | Check whether a path exists from `Ni` to `Nj`, and print it |
| `e` | `exit` | `e` | Write the graph to the output file, free all memory, and exit |

## Testing

A sample dataset is included: `input.txt` (loaded at startup) and `output.txt` (a prior run's result, for reference).

```bash
make
./miris -i input.txt -o output.txt
```

Try a few commands against the loaded sample data:

```
miris> f 1
1 15 4000 2024-02-13
```
(node `1` has a single outgoing transaction to node `15`, matching the `input.txt` record `1 15 4000 2024-02-13`)

```
miris> o 1 15
1 15
```
(a direct path exists from `1` to `15`)

```
miris> i 100
Succ: 100
```
(inserts a new account `100`)

```
miris> e
<N> Bytes released
```
(writes the final graph state to `output.txt`, frees all memory, and exits)

To verify a full run against the reference output, diff your generated file against the provided one:

```bash
diff output.txt output_reference.txt
```

## Data Structures

- **`Node`** — account name (`strdup`'d for a stable, independent copy), a linked list of outgoing `Edge`s, and an `index` used for fast array-based reference during DFS traversals.
- **`Edge`** — destination node pointer, transaction weight, date (`strdup`'d), and a pointer to the next edge in the source node's list — allowing each node to have multiple outgoing edges.
- **`Graph`** — a dynamic array of `Node*` plus current size/capacity, resized (doubled) on demand.
- **`HashTable`** — an array of singly-linked `HashNode` chains, each holding a pointer to a `Node`. New entries are inserted at the head of their chain.

Every node inserted into the graph is mirrored in the hash table, giving fast existence checks and lookups for the rest of the command set.

## Complexity Notes

| Operation | Complexity | Notes |
|---|---|---|
| Hash table insert/search/delete | O(1) average | Assumes reasonably distributed keys and a large enough table to avoid long chains |
| Add node | O(1) amortized | O(n) only on the (occasional) resize |
| Add edge | O(n) | Looks up both endpoints via linear scan of the node array — see *Known Limitations* |
| Delete node | O(n²) worst case | O(n) to find the node, O(n) to clear its outgoing edges, O(n·k) to remove incoming edges referencing it (k = average edges per node) |
| Delete edge | O(n) | Traverses the full edge list of the source node |

## Known Limitations / Future Improvements

- **`addEdge` uses a linear scan** to find its source/destination nodes rather than the hash table's O(1) `search`, which would be the natural optimization — not completed in this version.
- **`deleteNodeFromGraph` searches linearly** rather than via the hash table, for the same reason; an attempted hash-table-based approach hit a segmentation fault that wasn't tracked down in time.
- **No rehashing/resizing for the hash table** — its size is fixed at initialization (100 buckets). This keeps collision chains short in practice but means search complexity isn't guaranteed O(1) as the dataset grows arbitrarily large.

## Requirements

- GCC (C99 or later)
- POSIX environment (uses `unistd.h`)