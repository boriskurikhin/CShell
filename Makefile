#Boris Skurikhin
#Operating Systems - Assignment 1

all: ish 
adder: adder.c adder.h
	gcc -std=gnu99 -Wall -g -c adder.c -o adder.o 
ish: ish.c adder
	gcc -std=gnu99 -Wall -g ish.c adder.o -o ish.o 
clean:
	rm -f *.o