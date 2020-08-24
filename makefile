prog : main.o first_run.o second_run.o print.o
	gcc -ansi -Wall -pedantic -g main.o first_run.o second_run.o print.o -o prog
	
main.o : main.c	prog.h
	gcc -ansi -Wall -pedantic -c main.c -o main.o
	
first_run.o : first_run.c prog.h
	gcc -ansi -Wall -pedantic -c first_run.c -o first_run.o
	
second_run.o : second_run.c prog.h
	gcc -ansi -Wall -pedantic -c second_run.c -o second_run.o

print.o : print.c prog.h
	gcc -ansi -Wall -pedantic -c print.c -o print.o
