#include <u.h>
#include <libc.h>
#include <bio.h>

/*Unused, req'd for defs/fns */
#include <draw.h>

#include <thread.h>

#include "defs.h"
#include "fns.h"
#include "procfns.h"



void
threadmain(int argc, char** argv){

	/*This, specifically, is magic number bullshit. Sort of, note group semantics are not really clear from the fork (2) page and "fork()" doesn't have RFNOTEG by default, I had to read the CWFS code to get tipped off.*/
	if(rfork(RFNOTEG) != 0)
		threadexits(nil);
	Settings Settings;
	Channel* nch, *tch, *c;

	char* pfil;
	char* wdir;
	char* conffil;
	char* consfil;
	char* service;



	/*Defaults*/
	Settings.seed = 0;
	consfil = "/srv/server.cmd";
	pfil = "db/players.db";
	wdir = "./";
	conffil = "./conf.ini";
	service = "/srv/server";

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
		case 'i':
			service = EARGF(susage());
			break;
		case 's':
			Settings.seed = strtoull(EARGF(susage()), nil, 10);
			break;
	}ARGEND


	nch = chancreate(sizeof(char*),0);
	tch = chancreate(sizeof(ulong), 0);

	proccreate(arbiter,nch, 2048 );
	sendp(nch, tch);


	c = chancreate(sizeof(char*),0);
	proccreate(consfn,c, 2048 );
	sendp(c, consfil);
	sendp(nch, c);

	c = chancreate(sizeof(ulong), 0);
	sendp(nch, c);

	/*Block until args are not needed*/
	recvul(tch);
	chanclose(tch);
	chanclose(nch);
	threadexits(nil);

}
