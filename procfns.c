#include <u.h>
#include <libc.h>
#include <bio.h>
#include <thread.h>

#include "procfns.h"



void
pop(tocon* tc, int idx, uint listn){
	int i;
	for(i=idx;i<(listn-1);++i){
		tc[i] = tc[i+1];	
	}

}

void
timerproc(void* arg){
 	Channel* c;
	c = arg;

	for(;;){
		sleep(10);
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
	char* cmdrec;
	char adir[40];
	char buf[512];
	int afd;
	uint n, Key, ndt, i, listn;
	tocon* dtt = malloc(LISTENERS * sizeof(tocon));

	ulong ar, lr, tr;
	Biobuf* netprint;

	listn = 0;
	c = arg;
	service = recvp(c);
	v = recvp(c);
	afd = announce(service, adir);
 
	/*All references to servers must be completed before signalling ready*/
	sendul(v,1);

	la = chancreate(sizeof(ulong),0);



	tc = chancreate(sizeof(ulong), 0);
  	proccreate(timerproc, tc, 1024);
	
 	recvul(tc);
 	Alt a[] = {
		{v, &ar, CHANRCV},
		{la, &lr, CHANRCV},
		{tc, &tr, CHANRCV},
		{nil,nil,CHANEND},
	};


	for(;;){
		Key = alt(a);
		//fprint(2, "KEY: %d\n", Key);
		if(Key == 0){
			if(ar == 3){
				/*Send halts to channels which need/want to exit gracefuly (net/db access), others(timer etc) just kill*/
				sendul(la, 3);
				
 				close(afd);
			}


		}
		if(Key == 1){
			fprint(2, "PID: %d \n", lr);
			for(i=0;i<listn;++i){
				if(dtt[i].pid == lr){
					cmdrec = recvp(dtt[i].lc);
					dtt[i].time = 0;
					if(strncmp(cmdrec, "kill", 4) == 0){
						fprint(2, "INDEX: %d KILLED ON LISTEN: %d\n",i,dtt[i].pid);
						chanclose(dtt[i].lc);
	
						threadkill(dtt[i].pid);
						dtt[i].conn = 0;
						pop(dtt, i,listn);
						--listn;
					}
					else if(strncmp(cmdrec, "conn", 4) == 0){
						fprint(2, "INDEX: %d CONNECTED PID: %d\n", i, dtt[i].pid);
						dtt[i].conn = 1;
					}					
				}
			}

		}
		if(Key == 2){

			if(listn < LISTENERS  && (listn < 1 || dtt[listn-1].conn >0)){
				dtt[listn].lc = chancreate(sizeof(char*), 0);
				dtt[listn].pid = proccreate(dialthread, dtt[listn].lc, 2048);
				dtt[listn].time = 0;
				dtt[listn].conn = 0;
				sendp(dtt[listn].lc, la);
				sendp(dtt[listn].lc, &adir);
				send(dtt[listn].lc, &dtt[listn].pid);
				++listn;
			}
			for(i=0;i<listn;++i){
				//fprint(2, "PID: %d TIME: %d LISTN: %d\n", dtt[i].pid, dtt[i].time,listn);
				dtt[i].time += 10;
				if(dtt[i].time >= LISTENERTIMEOUT){
					fprint(2, "INDEX: %d TIMED OUT: %d TIME: %d\n",i,dtt[i].pid, dtt[i].time);
					chanclose(dtt[i].lc);

					threadkill(dtt[i].pid);
				//	fprint(2, "DEAD: %d\n", dtt[i].pid);
					dtt[i].conn = 0;
					pop(dtt, i,listn);
					--listn;
				}
			}
 		}

	}

}
void
dialthread(void* arg){
	Channel* c,*v;
	char* adir, ldir[40], *brdin;
	Biobuf* net;
	int lfd, dfd;
	uint pid;
	char* killcmd = "kill";
	char* recvcmd = "conn";

	c = arg;
	v = recvp(c);
 	adir = recvp(c);
	recv(c, &pid);

	lfd = listen(adir, ldir);
	if(lfd <= 0){
		send(v, &pid);
		sendp(c, killcmd);
		threadexits(nil);
	}
	dfd = accept(lfd, ldir);
	net = Bfdopen(dfd, OREAD);

	send(v,  &pid);
	sendp(c, recvcmd);

	for(;;){

		fprint(dfd, "LOL DONGS %d\n", dfd);
		brdin = Brdstr(net,'\n',0 );
		fprint(2, "READ: %s IN: %d ON: %d\n", brdin, Blinelen(net), pid);
		if( Blinelen(net) < 1){
			close(dfd);
			close(lfd);

			send(v, &pid);
			sendp(c, killcmd);
			/*This doesn't ever recieve anything, but it blocks keeping us from continuing to read from the closed network (and thus send repeated kill calls) */
			recvp(c);
		}
		free(brdin);
 	}
}
