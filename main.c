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

	DTile* GStack = (DTile*)calloc(100, sizeof(DTile));


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

	for(k=0,gclock=0;++gclock;){
		draw(screen, Rect(screen->r.min.x, screen->r.min.y, screen->r.max.x, screen->r.max.y), bground, nil, ZP);

		if(k>=100){
			for(i=0;i<k;++i){
				draw(screen, Rect(screen->r.min.x + GStack[i].offx, screen->r.min.y + GStack[i].offy, screen->r.min.x + GStack[i].offx + 32, screen->r.min.y + GStack[i].offy  + 32), GStack[i].tile, nil, ZP);

			}

		}

		draw(screen, Rect(screen->r.min.x, screen->r.min.y, screen->r.min.x+DSTATUSLX, screen->r.min.x+DSTATUSY ),UIface.viewport,nil,ZP);

		draw(screen, Rect(screen->r.min.x + 17, screen->r.min.y + 15, screen->r.min.x + 49, screen->r.min.y + 47), GStack[0].tile, nil, ZP);

		draw(screen, Rect(screen->r.min.x + DSTATUSLX + 1, screen->r.min.y + 4, screen->r.min.x + DSTATUSLX + 1 + DPIPLX, screen->r.min.y + 4 + DPIPLY), UIface.pip[0], nil, ZP);

		draw(screen, Rect(screen->r.min.x + DSTATUSLX + 1 + DPIPLX + 1, screen->r.min.y + 2, screen->r.min.x + DSTATUSLX + 1 + DPIPLX + 1 + DENDX, screen->r.min.y + 2 + DENDYL ), UIface.end[0], nil, ZP);
		draw(screen, Rect(screen->r.min.x + DSTATUSLX + 1, screen->r.min.y + 2 + DENDYL , screen->r.min.x + 2 + DSTATUSLX + 1 + DENDXL, screen->r.min.y  + DENDYL + DENDYL), UIface.end[1], nil, ZP);
		draw(screen, Rect(screen->r.min.x + DSTATUSLX + 1, screen->r.min.y + 2  + DENDYL + DENDYL, screen->r.min.x + DSTATUSLX + 1 + DENDXL, screen->r.min.y + 2 + DENDYL + DENDYL + DENDYL), UIface.end[2], nil, ZP);


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


		}
		if(Key == Etimer){
			if(k<100){
				i = ((rand() ) % (screen->r.max.x - screen->r.min.x - 32));
				j = ((rand() ) % (screen->r.max.y - screen->r.min.y - 32));


				l = ((rand()*32) % tileset->r.max.x);
				m = ((rand()*32) % tileset->r.max.y);

				draw(tile, Rect(tile->r.min.x - l, tile->r.min.y - m,32,32), tileset, nil, ZP);
				GStack[k].tile = tile;
				GStack[k].offx = i;
				GStack[k].offy = j;
 	
				tile = allocimage(display,Rect(0,0,32,32),RGBA32,0,DTransparent);
 

				++k;
			}
		}



	}

}
