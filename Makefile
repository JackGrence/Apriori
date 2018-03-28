CC = gcc
CFLAGS = -Wall -O3
OBJECTS = main.o display.o linklist.o hash_tree.o

all: apriori

debug: CFLAGS = -ggdb

debug: apriori

windows: CC = x86_64-w64-mingw32-gcc # for windows

windows: apriori

apriori: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o $@

main.o: display.h linklist.h hash_tree.h

display.o: display.h linklist.h hash_tree.h

linklist.o: linklist.h hash_tree.h

hash_tree.o: hash_tree.h linklist.h

clean:
	-rm *.o
