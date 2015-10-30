CC = g++
GLOBAL = global.h
test: btree.o test.o
	$(CC) btree.o test.o test
test.o: global.h test/test.c btree.o
	$(CC) -c test/test.c
btree.o: global.h btree.h btree.c buffer.o 
	$(CC) -c btree.c
buffer.o: global.h 
	$(CC) -c buffer.cpp