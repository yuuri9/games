#include <u.h>
#include <libc.h>
#include <bio.h>
#include <thread.h>

#include "procfns.h"


void
timerproc(void* arg){
	Channel* c;
	c = arg;

	for(;;){
		if(recvul(c) == -1)
			threadexits(nil);
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
				sendul(c,4);
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


				if(conscod > 3 && strncmp(conscmd, "halt", 4) == 0){

					/*Send halts to all threads here*/
					sendul(dialalt, -1);
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
	uint n, Key;
	ulong ar, lr, tr;
	Biobuf* netprint;


	c = arg;
	service = recvp(c);
	v = recvp(c);
	afd = announce(service, adir);

	/*All references to servers must be completed before signalling ready*/
	sendul(v,1);

	la = chancreate(sizeof(ulong),0);
	lc = chancreate(sizeof(char*), 0);
	proccreate(dialproc, lc, 2048);
	sendp(lc, la);

	sendp(lc, &adir);

	tc = chancreate(sizeof(ulong), 0);
	proccreate(timerproc, tc, 1024);
	sendul(tc, 1);

	Alt a[] = {
		{v, &ar, CHANRCV},
		{la, &lr, CHANRCV},
		{tc, &tr, CHANRCV},
		{nil,nil,CHANEND},
	};
	for(;;){
		Key = alt(a);
		if(Key == 0){
			if(ar == -1){
				/*Send halts to channels*/
				sendul(la, -1);
				sendul(tc, -1);
				close(afd);
			}


		}
		if(Key == 2){
			sendul(la, 1);


			sendul(tc, 1);
		}

	}

}
void
dialproc(void* arg){
	Channel* c,*v;
	char* adir, ldir[40];
	
	int lfd, dfd;

	c = arg;
	v = recvp(c);
	adir = recvp(c);
	lfd = listen(adir, ldir);
	if(lfd <= 0)
		threadexits(nil);

	dfd = accept(lfd, ldir);

	for(;;){
		if(recvul(v) == -1){
			close(dfd);
			close(lfd);
			threadexits(nil);
		}

		
		fprint(dfd, "LOL DONGS\n");
		
	}
}
