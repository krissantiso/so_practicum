CC=gcc
FLAGS=-g -Wall -Wextra
.PHONY: clean

main: p1.c historicList.c historicList.h fileList.c fileList.h commands.c commands.h
	$(CC) $(FLAGS) commands.c -o commands.o -c
	$(CC) $(FLAGS) historicList.c -o hlist.o -c
	$(CC) $(FLAGS) fileList.c -o flist.o -c
	$(CC) $(FLAGS) p1.c -o p1.o -c
	$(CC) p1.o hlist.o flist.o commands.o -o shell
	rm -r *.o
clean:
	rm -r *.o
exec:
	./shell
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./shell