CC=gcc
FLAGS=-g -Wall -Wextra
.PHONY: clean

main: p1.c historic_list.c historic_list.h open_list.c open_list.h p0.c p0.h p1.c p1.h
	$(CC) $(FLAGS) p0.c -o p0.o -c
	$(CC) $(FLAGS) p1.c -o p1.o -c
	$(CC) $(FLAGS) historic_list.c -o hlist.o -c
	$(CC) $(FLAGS) open_list.c -o flist.o -c
	$(CC) $(FLAGS) main.c -o main.o -c
	$(CC) main.o hlist.o flist.o p0.o p1.o -o shell
	rm -r *.o
clean:
	rm -r *.o
exec:
	./shell
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./shell