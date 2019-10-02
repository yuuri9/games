</$objtype/mkfile

TARG=game
game: main.$O fns.$O server
	$LD -o game main.$O fns.$O
server: server.$O fns.$O
	$LD -o server server.$O fns.$O
server.$O: server.c
	$CC $CFLAGS server.c
main.$O: main.c
	$CC $CFLAGS main.c 
fns.$O: fns.c
	$CC $CFLAGS fns.c

clean:V:
	rm -f *.[$OS] [$OS].out game server 
