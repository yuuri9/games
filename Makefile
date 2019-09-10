CFLAGS=-I ${PLAN9}/include/  -L ${PLAN9}/lib/ 
LDFLAGS= -L ${PLAN9}/lib/   -lbio  -l9  -l9p  -l9pclient -ldraw   -lmux -lregexp9  -lthread 

all : fns.o main.o game

game : fns.o main.o
	cc ${CFLAGS} -o game main.o fns.o ${LDFLAGS}

main.o :  main.c defs.h fns.h
	cc ${CFLAGS} -c main.c ${LDFLAGS}

fns.o : fns.c defs.h fns.h
	cc ${CFLAGS} -c fns.c ${LDFLAGS}

clean : 
	rm game main.o fns.o
