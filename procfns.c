#include <u.h>
#include <libc.h>
#include <bio.h>
#include <thread.h>



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
				threadexits(nil);
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
			if(conscod > 0){
				conscmd = recvp(consch);


				if(conscod > 3 && strncmp(conscmd, "halt", 4) == 0){
					/*Send halts to all threads here*/
					sendul(consalt,1);
					threadexits(nil);

				}

				free(conscmd);

			}
		}

	}
}
