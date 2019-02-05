all: ish 
ish: ish.c
	gcc -Wall -g ish.c -o ish.o
clean:
	rm -f *.o
	