#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <bio.h>

#include "defs.h"
#include "fns.h"

void
eresized(int New){
	if(New && getwindow(display, Refmesg) < 0){
		
		exits("Getwindow Error");
	}
}

void
usage(void){
	fprint(2, "game [-t] tickrate [-f] filename [-s] seed\n");
	exits(nil);
}

void
quit(Biobuf* savfile, Settings* Settings){
	
	save(savfile, Settings);
	exits(nil);
}

/*You must open the file before every save and it will be closed in the process*/
void
save(Biobuf* savfile, Settings* Settings){
	Bprint(savfile, "seed=%uld\n",Settings->seed);
	Bterm(savfile);
}

uint
loadsave(Settings* Settings, Biobuf* savfile){
	char* line;
	for(;(line=Brdstr(savfile,'\n',1)) != 0;free(line)){
		if(Blinelen(savfile) > 5 && strncmp("seed=",line,5) == 0){
			Settings->seed = strtoul(&line[5],nil,10);
			return 1;
		}
	}
	return 0;
}
