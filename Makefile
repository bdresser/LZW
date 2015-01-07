CC=gcc
CFLAGS=-g3 -Wall -pedantic -std=c99

decode: encode
	ln -f encode decode
encode: lzw.c trie.o /c/cs323/Hwk2/code.o
	${CC} ${CFLAGS} -o $@ trie.o /c/cs323/Hwk2/code.o lzw.c
trie.o: trie.h trie.c
	${CC} ${CFLAGS} -c trie.c

