lexer.o: lexer.c
	gcc  -c lexer.c

lexer: lexer.o
	gcc	lexer.o -o lexer

all: lexer

clean:
	rm -rf *.gch
	rm -rf *.out
	rm -rf *.o
