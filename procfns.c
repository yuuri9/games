#include <u.h>
#include <libc.h>
#include <bio.h>
#include <thread.h>

#include "procfns.h"



void
pop(tocon** tc, int idx){
	int i;
	for(i=idx;i<(LISTENERS - 1);++i){
		tc[i]->pid = tc[i+1]->pid;
		tc[i]->time = tc[i+1]->time;
	}

}

void
timerproc(void* arg){
	fprint(2, "dong");
	Channel* c;
	c = arg;

	for(;;){
		sleep(10);
		fprint(2, "bong");
		sendul(c, 1);
	}
}

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
				fprint(p[1], "Halting...\n");
				sendul(c,3);
				sendp(v,"halt");
				recvul(c);
				fprint(p[1], "done\n");
				/*Remaning connections die now.*/
				threadexitsall(nil);
			}

			/*fprint(p[1], "ADDR: %ulx\n", (ulong)consinbuf);*/
			sendul(c,Blinelen(console));
			sendp(v,consinbuf);

		}
}


void
arbiter(void* arg){

	Channel* in;
	Channel* ind;

	ulong conscod, dialcod;
	uint Key;

	Channel* consch, *consalt, *dialch, *dialalt;
	char* conscmd;


	in = arg;
	ind = recvp(in);


	consch = recvp(in);
	consalt = recvp(in);
	sendp(consch, consalt);
	recvul(consalt);

	dialch = recvp(in);
	dialalt = recvp(in);
	sendp(dialch, dialalt);
	recvul(dialalt);

	/*Close channel spawn inputs*/
	sendul(ind, 1);
	chanclose(in);
	chanclose(ind);

	Alt a[] = {
		{consalt,&conscod,CHANRCV},
		{dialalt, &dialcod, CHANRCV},
		{nil,nil,CHANEND},

	};



	for(;;){
		Key = alt(a);

		if(Key == 0){
			if(conscod > 0){
				conscmd = recvp(consch);


				if(conscod == 3 && strncmp(conscmd, "halt", 4) == 0){

					/*Send halts to all threads here*/
					sendul(dialalt, 3);


					sendul(consalt,1);
					threadexits(nil);

				}

				free(conscmd);

			}
		}

	}
}
void
dialarbiter(void* arg){
 	Channel* c, *v;

	Channel* lc, *la, *tc;

	char* service;
	char adir[40];
	char buf[512];
	int afd;
	uint n, Key, ndt, i, listn;
	tocon* dtt = malloc(LISTENERS * sizeof(tocon));

	ulong ar, lr, tr;
	Biobuf* netprint;

	listn = 1;
	c = arg;
	service = recvp(c);
	v = recvp(c);
	afd = announce(service, adir);
 
	/*All references to servers must be completed before signalling ready*/
	sendul(v,1);

	la = chancreate(sizeof(ulong),0);
	dtt[0].lc = chancreate(sizeof(char*), 0);

	dtt[0].pid = threadcreate(dialthread, dtt[0].lc, 2048);
	dtt[0].time = 0;
	sendp(dtt[0].lc, la);

	sendp(dtt[0].lc, &adir);

	tc = chancreate(sizeof(ulong), 0);
 	proccreate(timerproc, tc, 1024);
	
	recvul(tc);
	fprint(2, "g");
	Alt a[] = {
		{v, &ar, CHANRCV},
		{la, &lr, CHANRCV},
		{tc, &tr, CHANRCV},
		{nil,nil,CHANEND},
	};


	for(;;){
		Key = alt(a);
		fprint(2, "KEY: %d\n", Key);
		if(Key == 0){
			if(ar == 3){
				/*Send halts to channels which need/want to exit gracefuly (net/db access), others(timer etc) just kill*/
				sendul(la, 3);
				
 				close(afd);
			}


		}
		if(Key == 2){

			if(listn < LISTENERS){
				dtt[listn].lc = chancreate(sizeof(char*), 0);
				dtt[listn].pid = threadcreate(dialthread, dtt[listn].lc, 2048);
				dtt[listn].time = 0;
				sendp(dtt[listn].lc, la);
				sendp(dtt[listn].lc, &adir);
				++listn;
			}
			for(i=0;i<listn;++i){
				fprint(2, "PID: %d TIME: %d \n", dtt[i].pid, dtt[i].time);
				dtt[i].time += 10;
				if(dtt[i].time >= LISTENERTIMEOUT){
					fprint(2, "TIMED OUT: %d\n",dtt[i].pid);
					chanclose(dtt[i].lc);

					threadkill(dtt[i].pid);
					pop(&dtt, i);
					--listn;
				}
			}
 		}

	}

}
void
dialthread(void* arg){
	Channel* c,*v;
	char* adir, ldir[40];
	Biobuf* net;
	int lfd, dfd;

	c = arg;
	v = recvp(c);
	adir = recvp(c);

	lfd = listen(adir, ldir);
	if(lfd <= 0)
		threadexits(nil);
	fprint(2, "chong\n");
	dfd = accept(lfd, ldir);

	net = Bfdopen(dfd, OREAD);

	for(;;){
		if(recvul(v) == 3){
			close(dfd);
			close(lfd);
			threadexits(nil);
		}

		
		fprint(dfd, "LOL DONGS %d\n", dfd);
		Brdstr(net,'\n',0 );
	}
}
