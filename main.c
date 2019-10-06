#include <u.h>
#include <libc.h>
#include <draw.h>
#include <bio.h>

#include <thread.h>
#include <keyboard.h>
#include <mouse.h>
#include <control.h>

#include "defs.h"
#include "fns.h"

void
utimer(void* arg){
	Channel* c;
	c = arg;
	for(;;){
		sleep(10);
		sendul(c, 1);
	}
}
void
kbdfsio(void* arg){
	Channel* c;
	Biobuf* buf;
	char* strn;
	char* strpt;
	uint strptn;
	uint i;
	uint t;
	c = arg;
	/*TODO, this can probably be implemented through a proxy program on linux/OSX, will need access to linux/OSX box to develop*/
	buf = Bopen("/dev/kbd", OREAD);

	/*The dance t does here is required since the first keydown sends both hit and repeat signal*/
	for(i=0,strptn=0,t=0;;strn = Brdstr(buf, '\0', 1)){
		if(Blinelen(buf) > 0 ){
			if(strn[0] == 'k' || strn[0] == 'K'){
				strpt = strn;
				strptn = Blinelen(buf);
				t = 0;
			}
			if(t==0 && strn[0] == 'c')
				t = 1;
			if(t == 1){
				for(i=1;i<strptn;++i){
					sendul(c, strpt[i]);
				}
			}
		}
	}

}
void
threadmain(int argc, char** argv){

 	uint i,j,k,l,m;
	uint diskbd;
	uint gclock;

	int ifd;
	char* filename = 0;
	Biobuf* savfile;


  	Settings Settings;

	Image* bground;
	Image* tileset;
	Image* tile;
	Image* interface;

	uint Key;
	uint tick;
	Rune buf;
	Mouse Mouse;
	Mousectl* mc;
	Keyboardctl* kc;
 
	UIface UIface;
	GEntity GEntity, FEntity, *selentity;

	/*PRECEDENCE: File seed overwrites cli seed*/
	Settings.seed = 0;
	diskbd = 0;


	ARGBEGIN{
		case 't':
			Settings.trate = strtoul(EARGF(usage()),nil,10);
			break;
		case 'f':
			filename = EARGF(usage());
			break;
		case 's':
			Settings.seed = strtoull(EARGF(usage()),nil,10);
			break;
		case 'm':
			Settings.mapsize = strtoul(EARGF(usage()), nil, 10);
			break;
		case 'k':
			diskbd = 1;
			break;
	}ARGEND



	
	if(filename == 0)
		filename = "game.sav";



	if(initdraw(nil, nil, "game") < 0)
		threadexitsall("Initdraw Error");

	if(getwindow(display, Refmesg) < 0)		
		threadexitsall("Getwindow Error");


	mc = initmouse(nil, screen);
	if(diskbd == 1 || (ifd = open("/dev/kbd", OREAD)) <= 0 ){
		kc = initkeyboard(nil);
		diskbd = 1;
	}
	else 
		close(ifd);

	Alt a[] = {
		{mc->c, &Mouse, CHANRCV},
		{mc->resizec, nil, CHANRCV},
		{nil, &buf, CHANRCV},
		{nil, &tick, CHANRCV},
		{nil,nil,CHANEND},
	};

	if(diskbd == 1){
		a[2].c = kc->c;
	}

	a[3].c = chancreate(sizeof(ulong), 0);
	proccreate(utimer,a[3].c,2048);

 	if(diskbd != 1){
		a[2].c = chancreate(sizeof(ulong), 0);
		proccreate(kbdfsio, a[2].c, 2048);
	}

	/*File save load*/
	if((savfile= Bopen(filename, OREAD)) != 0){
 		if(loadsave(&Settings, savfile) == 0)
			Settings.seed = 0;
		Bterm(savfile);
	}

	if(Settings.seed == 0){
		srand(time(nil));
		Settings.seed = lrand();
	}

	srand(Settings.seed);

	/* Image/entity loads */
	if((ifd=open("./ass/ProjectUtumno_full_0.bit", OREAD)) > 0)
		tileset = readimage(display, ifd, 0);
	else
		threadexitsall("readimage error");
	close(ifd);

	if((ifd=open("./ass/ui_big_pieces.bit", OREAD)) > 0)
		interface = readimage(display, ifd, 0);
	else
		threadexitsall("readimage error");
	close(ifd);

	bground = allocimagemix(display, DPaleyellow, DDarkgreen);

	tile = allocimage(display,Rect(0,0,32,32),RGBA32,0,DTransparent);
	initui(&UIface, display, interface);
	
 
 	initentity(&GEntity, 2569, 2, 2, 2, tileset, display);
 	initentity(&FEntity, 2570, 2,2,2,tileset, display);

	selentity = &GEntity;

	display->locking = 1;
	unlockdisplay(display);

	for(;;){


 
		Key = alt(a);





  		if(Key == 2){
			if(buf == 'q' || buf == 'Q' || buf == 127){
				if((savfile = Bopen(filename,OWRITE|OTRUNC)) != 0){
					quit(savfile, &Settings);
				} else {
					fprint(2,"SAVE FAILED: COULD NOT OPEN %s\n", filename);
					savfile = Bfdopen(1, OWRITE);
					quit(savfile, &Settings);
				}

			}
			if(buf == 'E' || buf == 'e'){
				++selentity->hp;
				++selentity->mhp;
				++selentity->mp;
				++selentity->mmp;
				++selentity->exp;
				++selentity->mexp;
			}
			if(buf == 'f' || buf == 'F'){
				++selentity->mhp;
				++selentity->mmp;
				++selentity->mexp;


			}
			if(buf == 'g' || buf == 'G'){
				++selentity->hp;
				++selentity->mp;
				++selentity->exp;


			}
			if(buf == 'K' || buf == 'k'){
				--selentity->hp;
				--selentity->mhp;
				--selentity->mp;
				--selentity->mmp;
				--selentity->exp;
				--selentity->mexp;
			}
			if(buf == 'h' || buf == 'H'){
				--selentity->mhp;
				--selentity->mmp;
				--selentity->mexp;


			}
			if(buf == 's' || buf == 'S'){
				--selentity->hp;
				--selentity->mp;
				--selentity->exp;


			}

		}
		/*Timer <probably we should define with enums ?>*/
		if(Key == 3){
	 		lockdisplay(display);
			draw(screen, Rect(screen->r.min.x, screen->r.min.y, screen->r.max.x, screen->r.max.y), bground, nil, ZP);
	 		drawentity(&GEntity, screen, 128, 128);
	 		drawentity(&FEntity, screen, 400,400);
	
			drawui(&UIface, screen, selentity, 32 , 32);
			flushimage(display, 1);
			unlockdisplay(display);
		}
		if(Key == 1){
			eresized(1);
		}
		if(Key == 0){
			if((Mouse.buttons & 1) == 1 && Mouse.xy.x > screen->r.min.x + 128 && Mouse.xy.y > screen->r.min.y + 128 && Mouse.xy.y < (screen->r.min.y + 128 + 32) && Mouse.xy.x < (screen->r.min.x + 128+32)){
				selentity = &GEntity;
			}
			if((Mouse.buttons & 1) == 1 && Mouse.xy.x > screen->r.min.x + 400 && Mouse.xy.y > screen->r.min.y + 400 && Mouse.xy.y < (screen->r.min.y + 400 + 32) && Mouse.xy.x < (screen->r.min.x + 400+32)){
				selentity = &FEntity;
			}
		}

		

	}

}
