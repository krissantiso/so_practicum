CC=gcc
FLAGS=-g -Wall
.PHONY: clean

main: p1.c historicList.c historicList.h fileList.c fileList.h help.c help.h commands.c commands.h
	$(CC) $(FLAGS) commands.c -o commands.o -c
	$(CC) $(FLAGS) historicList.c -o hlist.o -c
	$(CC) $(FLAGS) fileList.c -o flist.o -c
	$(CC) $(FLAGS) help.c -o help.o -c
	$(CC) $(FLAGS) p1.c -o p1.o -c
	$(CC) p1.o hlist.o flist.o help.o commands.o -o p1
	rm -r *.o
clean:
	rm -r *.o
exec:
	./p1
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./p1
