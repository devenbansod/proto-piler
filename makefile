# Batch 74
# Deven  Bansod   2012B3A7316P
# Nirant Kasliwal 2012C6PS694P

all: fileBuffer.o lexer.o stack.o parser.o AST.o symbolTable.o
	gcc  -Wall       fileBuffer.o lexer.o stack.o parser.o AST.o symbolTable.o driver.c -o stage1exe

debug: fileBuffer.o lexer.o stack.o parser.o AST.o symbolTable.o
	gcc  -Wall       fileBuffer.o lexer.o stack.o parser.o AST.o symbolTable.o driver.c -g -o stage1exe

clean:
	rm -rf *.gch
	rm -rf *.out
	rm -rf *.o
	rm -rf stage1exe

fileBuffer.o: fileBuffer.c
	gcc  -Wall -c -g fileBuffer.c

lexer.o: lexer.c
	gcc  -Wall -c -g lexer.c

stack.o: stack.c
	gcc  -Wall -c -g stack.c

parser.o: parser.c
	gcc  -Wall -c -g parser.c

AST.o: AST.c
	gcc  -Wall -c -g AST.c

symbolTable.o: symbolTable.c
	gcc  -Wall -c -g symbolTable.c