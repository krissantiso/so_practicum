CC=gcc
FLAGS=-g -Wall -Wextra
.PHONY: clean

main: main.c historicList.c historicList.h fileList.c fileList.h
	$(CC) $(FLAGS) historicList.c -o hlist.o -c
	$(CC) $(FLAGS) fileList.c -o flist.o -c
	$(CC) $(FLAGS) main.c -o main.o -c
	$(CC) main.o hlist.o flist.o -o shell
	rm -r *.o
static: main.c historicList.c historicList.h
	$(CC) $(FLAGS) historicList.c -o list.o -c
	$(CC) $(FLAGS) fileList.c -o flist.o -c
	$(CC) $(FLAGS) main.c -o main.o -c
	$(CC) -static main.o list.o flist.o -o shell
	rm -r *.o
clean:
	rm -r *.o
exec:
	./shell
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./shell