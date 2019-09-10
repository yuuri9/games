</$objtype/mkfile

TARG=game
game: main.$O fns.$O
	$LD -o game main.$O fns.$O
main.$O: main.c
	$CC $CFLAGS main.c 
fns.$O: fns.c
	$CC $CFLAGS fns.c
clean:V:
	rm -f *.[$OS] [$OS].out game
