fileBuffer.o: fileBuffer.c
	gcc  -c -g fileBuffer.c

lexer.o: lexer.c
	gcc  -c -g lexer.c

stack.o: stack.c
	gcc  -c -g stack.c

keywordTrie.o: keywordTrie.c
	gcc  -c -g keywordTrie.c

symbolTable.o: symbolTable.c
	gcc  -c -g symbolTable.c

all: fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o
	gcc fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o compiler.c -o compiler.out

debug: fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o
	gcc fileBuffer.o lexer.o stack.o keywordTrie.o symbolTable.o compiler.c -g -o compiler.out

clean:
	rm -rf *.gch
	rm -rf *.out
	rm -rf *.o
