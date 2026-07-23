# os-graph-hashtable-c

Lesson : Operating Systems

Student: Stylianos Prasianakis

Programming assignment 1

## What this is

This is a small C program called miris that models transactions between accounts as a graph. Every account is a node and every transaction is an edge going from one account to another, with an amount and a date attached to it. There's also a hash table on the side so we don't have to loop through the whole graph every time we want to check if an account exists.

You can insert and delete accounts, add and delete transactions, look for cycles (loops of transactions that go back to where they started), trace paths a few steps out from an account, and check if two accounts are connected. All of this happens through a simple command prompt once the program is running.

## Build and run

Just run make in the folder and then start it with an input and output file:

```
make
./miris -i input_file -o output_file
```

The two flags can be given in either order, but both are required or the program will complain and quit.

To get rid of the compiled files afterwards run:

```
make clean
```

## Commands

Every command has a short version and a longer, more readable version, and both work the same way.

i or insert followed by one or more account names adds new accounts.

n or insert2 followed by two accounts, an amount and a date adds a transaction between them.

d or delete followed by account names removes those accounts along with their transactions.

l or delete2 followed by two accounts removes the transaction between them.

m or modify takes two accounts plus the old amount/date and the new amount/date, and updates that transaction.

f or find followed by an account shows everything going out of it.

r or receiving followed by an account shows everything coming into it.

c or circlefind followed by an account looks for cycles that start and end there.

fi or findcircles takes an account and a minimum weight, and only looks for cycles where every edge in them is at least that big.

t or traceflow takes an account and a depth, and prints every path you can reach going that many steps out.

o or connect takes two accounts and tells you if there's a path between them, printing it if so.

e or exit writes everything to the output file, frees all the memory and closes the program.

## Testing

There's a test script, test_miris.sh, that I put together so I wouldn't have to manually type commands every time I changed something in the code. You run it with:

```
chmod +x test_miris.sh
./test_miris.sh
```

It builds the project itself, then feeds it a sequence of commands and checks whether the output matches what it should be. At the very end it also compares the output file the program writes against a known correct version, so a passing run means the whole thing actually works end to end, not just that it compiles.

To make this useful I built a small graph from scratch inside the test instead of using the sample input file, because the sample data didn't really have any cycles in it and cycles are basically the whole point of half these commands.

The test graph has six accounts, A through F, with two cycles that share some nodes: A to B to C back to A, and B to C to D to E back to B. There's also one extra transaction from D to F that only exists so I have something to modify and delete without touching the cycles.

I ran through every single command against this graph by hand first to work out what the correct output should be, then ran the same thing against the actual compiled program to make sure I wasn't wrong about any of it, and that's what the test script checks automatically now.

A couple of things about the results that aren't obvious if you're just reading the code. When you ask for the edges going out of a node, the most recently added one shows up first, because new edges get stuck onto the front of the list instead of the back.

When I run circlefind on A it only finds one of the two cycles, because the second cycle never actually passes through A.

findcircles with a minimum weight ends up finding both cycles and tells you the smallest edge weight in each one.

traceflow with a depth of 2 stops after two hops in every direction, which is just what the depth number means.

And connect just returns whatever path it finds first, not the shortest one, since it's not doing anything fancy like BFS.

## How the data is stored

A node holds the account name, a linked list of its outgoing edges, and an index number that gets used later on for the DFS-based commands.

An edge holds the destination node, the weight, the date, and a pointer to the next edge in that node's list, which is how one node ends up being able to have several transactions going out of it.

The graph itself is just an array of node pointers that doubles in size whenever it fills up. The hash table is an array of linked lists, where accounts that hash to the same spot get chained together instead of overwriting each other.

Whenever a node gets added to the graph it also gets added to the hash table at the same time, and same with deletion, so checking whether an account already exists is fast without having to search through the whole graph.

## Complexity, roughly

Hash table insert, search and delete are all O(1) on average, assuming the hash function spreads accounts out reasonably evenly.

Adding a node is also O(1) unless the array needs to resize, in which case that one operation costs O(n).

Adding an edge is O(n) though, because it has to loop through the node array to find both accounts involved instead of using the hash table — I know that's an obvious optimization I didn't get around to.

Deleting a node is the worst one, roughly O(n squared) in the worst case, since it has to find the node, remove its outgoing edges, and then check every other node in the graph for edges pointing at the one being deleted.

Deleting a single edge is O(n) since it just walks through one node's edge list.

## Things I didn't get to

addEdge searches for both accounts with a plain loop through the node array instead of using the hash table, which would obviously be faster. I ran out of time to change it.

deleteNodeFromGraph has the same issue, and I actually tried switching it to use the hash table at one point but kept getting a segfault I couldn't figure out, so I left it as is.

The hash table also doesn't resize or rehash, its size is just fixed at 100 from the start. That's fine for the graphs I tested with, but if you threw a huge number of accounts at it the chains would get long and searching wouldn't really stay O(1).

## Requirements

gcc and a Linux/POSIX environment, since the code uses unistd.h.
