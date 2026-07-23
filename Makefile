#
# In order to execute this "Makefile" just type "make"
#	A. Delis (ad@di.uoa.gr)
#

OBJS 	= main.o Graph.o Hash_table.o Commands.o
SOURCE	= main.c Graph.c Hash_table.c Commands.c
HEADER  = Header.h Graph.h Hash_table.h Commands.h
OUT  	= miris
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

Graph.o: Graph.c
	$(CC) $(FLAGS) Graph.c

Hash_table.o: Hash_table.c
	$(CC) $(FLAGS) Hash_table.c

Commands.o: Commands.c
	$(CC) $(FLAGS) Commands.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)
	rm -f build.log diff.log
	rm -f empty_input_difficult.txt
	rm -f difficult_test_output.txt difficult_test_expected_output.txt difficult_test_run.log
	rm -f valgrind-miris.log

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)