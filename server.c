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

	Settings Settings;

	char* pfil;
	char* wdir;
	char* conffil;
	char* consfil;

	ulong conscod,id;
	uint Key;

	Channel* consch;
	char* conscmd;


	in = arg;
	ind = recvp(arg);

	Settings.seed = recvul(ind);
	id = recvul(ind);
	pfil = malloc(sizeof(char*) * (id+1));
	strncpy(pfil,recvp(in), id);
	pfil[id] = '\0';
	
	id = recvul(ind);
	wdir = malloc(sizeof(char*) * (id+1));
	strncpy(wdir, recvp(in),id);
	wdir[id] = '\0';
	
	id = recvul(ind);
	conffil = malloc(sizeof(char*) * (id+1));
	strncpy(conffil, recvp(in),id);
	conffil[id] = '\0';
	
	id = recvul(ind);
	consfil = malloc(sizeof(char*) * (id+1));
	strncpy(consfil,recvp(in), id);
	consfil[id] = '\0';
	
	
	sendul(ind, 1);
	chanclose(in);
	chanclose(ind);

	Alt a[] = {
		{nil,&conscod,CHANRCV},
		{nil,nil,CHANEND},

	};

	consch = chancreate(sizeof(char*),0);
	a[0].c = chancreate(sizeof(ulong), 0);
	proccreate(consfn,consch, 2048 );

	sendp(consch, consfil);
	sendp(consch, a[0].c);

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
	rfork(RFNOTEG);
	Settings Settings;
	Channel* nch, *tch;

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
	sendul(tch, Settings.seed);
	/*Strlen returns long, don't think strings can be longer than the longest long, if argv is not null terminated we have a significantly bigger issue than buffer overflow*/
	sendul(tch, strlen(pfil));
	sendp(nch, pfil);
	sendul(tch, strlen(wdir));
	sendp(nch, wdir);
	sendul(tch, strlen(conffil));
	sendp(nch, conffil);
	sendul(tch, strlen(consfil));
	sendp(nch, consfil);
	recvul(tch);
	threadexits(nil);

}
