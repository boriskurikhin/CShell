#Boris Skurikhin
#Operating Systems - Assignment 1

all: ish 
adder: adder.c adder.h
	gcc -Wall -g -c adder.c -o adder.o 
ish: ish.c adder
	gcc -Wall -g -Iadder.h ish.c adder.o -o ish.o 
clean:
	rm -f *.o