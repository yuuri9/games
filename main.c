#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <bio.h>

#include "defs.h"
#include "fns.h"



void
main(int argc, char** argv){

	Event Event;
	ulong Key;
	uint i,j,k,l,m;
	uint gclock;
	int ifd;
	ulong trate = 20;
	ulong Etimer = 8;
	Settings Settings;

	Image* bground;
	Image* tileset;
	Image* tile;
	Image* interface;

	char* filename = 0;
	Biobuf* savfile;

	UIface UIface;

	GEntity GEntity;

	Settings.seed = 0;
	/*PRECEDENCE: File seed overwrites cli seed*/
	ARGBEGIN{
		case 't':
			trate = strtoul(EARGF(usage()),nil,10);
			break;
		case 'f':
			filename = EARGF(usage());
			break;
		case 's':
			Settings.seed = strtoull(EARGF(usage()),nil,10);
			break;
	}ARGEND

	if(filename == 0)
		filename = "game.sav";

	if(initdraw(nil, nil, "game") < 0){
		exits("Initdraw Error");
	}

	if(getwindow(display, Refmesg) < 0){
		
		exits("Getwindow Error");
	}

	bground = allocimagemix(display, DPaleyellow, DDarkgreen);

	tile = allocimage(display,Rect(0,0,32,32),RGBA32,0,DTransparent);
	
 

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

	if((ifd=open("./ass/ProjectUtumno_full_0.bit", OREAD)) > 0)
		tileset = readimage(display, ifd, 0);
	else
		exits("readimage error");
	close(ifd);

	if((ifd=open("./ass/ui_big_pieces.bit", OREAD)) > 0)
		interface = readimage(display, ifd, 0);
	else
		exits("readimage error");
	close(ifd);

	initui(&UIface, display, interface);
	

	einit(Ekeyboard|Emouse);
	etimer(Etimer, trate);

 	initentity(&GEntity, 240, 2, 2, 2, tileset, display);
 
	for(gclock=0;++gclock;){
		draw(screen, Rect(screen->r.min.x, screen->r.min.y, screen->r.max.x, screen->r.max.y), bground, nil, ZP);
 		drawentity(&GEntity, screen, 128, 128);
 
		drawui(&UIface, screen, &GEntity, 32 , 32);
		/*draw(screen, Rect(screen->r.min.x + 17, screen->r.min.y + 15, screen->r.min.x + 49, screen->r.min.y + 47), GStack[0].tile, nil, ZP);*/





		/*This blocks*/
		Key = event(&Event);
		if(Key == Ekeyboard){
			if(Event.kbdc == 'q' || Event.kbdc == 'Q' || Event.kbdc == 127){
				if((savfile = Bopen(filename,OWRITE|OTRUNC)) != 0){
					quit(savfile, &Settings);
				} else {
					fprint(2,"SAVE FAILED COULD NOT OPEN %s\n", filename);
					savfile = Bfdopen(1, OWRITE);
					quit(savfile, &Settings);
				}

			}
			if(Event.kbdc == 'E' || Event.kbdc == 'e'){
				++GEntity.hp;
				++GEntity.mhp;
				++GEntity.mp;
				++GEntity.mmp;
				++GEntity.exp;
				++GEntity.mexp;
			}


		}
		if(Key == Etimer){

		}
		if(Key == Emouse){

		}



	}

}
