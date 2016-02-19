lexer.o: lexer.c
	gcc  -c lexer.c

stack.o: stack.c
	gcc  -c stack.c

keywords_trie.o: keywords_trie.c
	gcc  -c keywords_trie.c

all: lexer.o stack.o keywords_trie.o
	gcc lexer.o stack.o keywords_trie.o compiler.c -o compiler.out

debug: lexer.o stack.o keywords_trie.o
	gcc lexer.o stack.o keywords_trie.o compiler.c -g -o compiler.out

clean:
	rm -rf *.gch
	rm -rf *.out
	rm -rf *.o
