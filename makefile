fileBuffer.o: fileBuffer.c
	gcc  -Wall -c -g fileBuffer.c

lexer.o: lexer.c
	gcc  -Wall -c -g lexer.c

stack.o: stack.c
	gcc  -Wall -c -g stack.c

keywordTrie.o: keywordTrie.c
	gcc  -Wall -c -g keywordTrie.c

symbolTable.o: symbolTable.c
	gcc  -Wall -c -g symbolTable.c

parser.o: parser.c
	gcc  -Wall -c -g parser.c

all: fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o parser.o
	gcc  -Wall       fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o parser.o driver.c -o stage1exe

debug: fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o parser.o
	gcc  -Wall       fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o parser.o driver.c -g -o stage1exe

clean:
	rm -rf *.gch
	rm -rf *.out
	rm -rf *.o
	rm -rf stage1exe
