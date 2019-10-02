#include <u.h>
#include <libc.h>
#include <bio.h>

/*Unused, req'd for defs/fns */
#include <draw.h>

#include <thread.h>

#include "defs.h"
#include "fns.h"


void
consfn(void* arg){

		int p[2];
		int consfd;
		Biobuf *console;
		char* consfil;
		consfil = (char*)arg;
		char* consinbuf;

		pipe(p);
	
		if((consfd = create(consfil, ORCLOSE|OWRITE, 0660)) < 0){
			remove(consfil);
			if((consfd = create(consfil, ORCLOSE|OWRITE, 0660)) < 0)
				threadexitsall("create failed");
		}
		fprint(consfd, "%d", p[0]);
	
		/*These Close commands are recommended by the srv (3) manpage, but seem to break*/
		/*
		close(consfd);
		close(p[0]);
		*/

		fprint(p[1], "Server initialized %ulld\n", nsec());
	
	
	
		console = Bfdopen(p[1], OREAD);

		/*For some reason the loop runs once before any data is read with a nil pointer here if we don't alloc some memory, because on plan9 malloc(0) doesn't fail this fixes it*/
		consinbuf = malloc(0);
		for(;;consinbuf = Brdstr(console, '\n', 1)){
			if(Blinelen(console) > 3 && cistrncmp("halt", consinbuf, 4) == 0){
				threadexitsall(nil);
			}

			/*fprint(p[1], "ADDR: %ulx\n", (ulong)consinbuf);*/
			free(consinbuf);
		}
}


void
threadmain(int argc, char** argv){
	uint i,j,k;


	Settings Settings;

	char* pfil;
	char* wdir;
	char* conffil;
	char* consfil;

	Settings.seed = 0;



	consfil = "/srv/server.cmd";
	ARGBEGIN{
		case 'p':
			pfil = EARGF(susage());
			break;
		case 'w':
			wdir = EARGF(susage());
			break;
		case 'c':
			conffil = EARGF(susage());
			break;
		case 'o':
			consfil = EARGF(susage());
			break;
		case 's':
			Settings.seed = strtoull(EARGF(susage()), nil, 10);
			break;
	}ARGEND

	proccreate(consfn,consfil, 2048 );


	

}
