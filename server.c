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
		Channel* c;
		Channel* v;

		Biobuf *console;

		char* consfil;
		v = arg;

		consfil = (char*)recvp(v);
		c = (Channel*)recvp(v);

		char* consinbuf;


		pipe(p);
	
		if((consfd = create(consfil, ORCLOSE|OWRITE, 0660)) < 0){
			remove(consfil);
			if((consfd = create(consfil, ORCLOSE|OWRITE, 0660)) < 0)
				threadexitsall("create failed");
		}

		sendul(c, 1);

		fprint(consfd, "%d", p[0]);

		/*These Close commands are recommended by the srv (3) manpage, but seem to break*/
		/*
		close(consfd);
		close(p[0]);
		*/

		fprint(p[1], "Server console initialized %ulld\n", nsec());

	
		console = Bfdopen(p[1], OREAD);

		for(;;){
			consinbuf = Brdstr(console, '\n', 1);
			if(Blinelen(console) > 3 && cistrncmp("halt", consinbuf, 4) == 0){
				threadexitsall(nil);
			}

			/*fprint(p[1], "ADDR: %ulx\n", (ulong)consinbuf);*/
			sendul(c,1);
			sendp(v,consinbuf);

		}
}


void
arbiter(void* arg){

	Channel* in;
	Channel* ind;

	ulong conscod;
	uint Key;

	Channel* consch, *consalt;
	char* conscmd;


	in = arg;
	ind = recvp(in);

	consch = recvp(in);
	consalt = recvp(in);
	sendp(consch, consalt);

	recvul(consalt);
	sendul(ind, 1);
	chanclose(in);
	chanclose(ind);

	Alt a[] = {
		{consalt,&conscod,CHANRCV},
		{nil,nil,CHANEND},

	};



	for(;;){
		Key = alt(a);

		if(Key == 0){
			if(conscod == 1){
				conscmd = recvp(consch);
				fprint(2,"%s\n",conscmd);
				free(conscmd);

			}
		}

	}
}


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




	/*Defaults*/
	Settings.seed = 0;
	consfil = "/srv/server.cmd";
	pfil = "db/players.db";
	wdir = "./";
	conffil = "./conf.ini";

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
