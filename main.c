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
	int ifd;
	ulong trate = 20;
	ulong Etimer = 8;
	Settings Settings;

	Image* bground;
	Image* tileset;
	Image* tile;
	Image* interface;

	Image* uiblk;
	Image* uiport;
	Image* uiend[3];
	Image* pipnl;

	char* filename = 0;
	Biobuf* savfile;

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
	uiblk = allocimage(display,Rect(0,0,32,32),RGBA32,0,DTransparent);
	uiport = allocimage(display, Rect(0,0,85,65),RGBA32,0,DTransparent);
	for(i=0;i<3;++i)
		uiend[i] = allocimage(display,Rect(0,0,33,21), RGBA32, 0,DTransparent);
	pipnl = allocimage(display, Rect(0,0,8,32), RGBA32,0,DTransparent);

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


	draw(uiport, Rect((1 - DSTATUSX), (1 - DSTATUSY), 85,65), interface, nil, ZP);
	draw(uiend[0], Rect(1 - DRENDX, 1- DRENDY, 33,21), interface, nil, ZP);
	draw(uiend[1], Rect(1 - DRENDX, 1- 202, 33,21), interface, nil, ZP);
	draw(uiend[2], Rect(1 - DRENDX, 1- 222, 33,21), interface, nil, ZP);

	draw(pipnl, Rect((1 - (DSTATUSX + 85)), (1 - DSTATUSY), 8,32), interface, nil, ZP);

	einit(Ekeyboard|Emouse);
	etimer(Etimer, trate);

	for(k=0;;){
		draw(screen, Rect(screen->r.min.x, screen->r.min.y, screen->r.max.x, screen->r.max.y), bground, nil, ZP);
		if(k>=100){
			for(i=0;i<k;++i){
				draw(screen, Rect(screen->r.min.x + GStack[i].offx, screen->r.min.y + GStack[i].offy, screen->r.min.x + GStack[i].offx + 32, screen->r.min.y + GStack[i].offy  + 32), GStack[i].tile, nil, ZP);

			}

		}
		draw(screen, Rect(screen->r.min.x, screen->r.min.y, screen->r.min.x + 85, screen->r.min.y + 65),uiport,nil,ZP);

		draw(screen, Rect(screen->r.min.x + 17, screen->r.min.y + 15, screen->r.min.x + 49, screen->r.min.y + 47), GStack[0].tile, nil, ZP);

		draw(screen, Rect(screen->r.min.x + 85 , screen->r.min.y , screen->r.min.x + 85 +8, screen->r.min.y + 32), pipnl, nil, ZP);

		draw(screen, Rect(screen->r.min.x + 85 + 8, screen->r.min.y + 2, screen->r.min.x + 85 + 33 + 8, screen->r.min.y + 22), uiend[0], nil, ZP);
		draw(screen, Rect(screen->r.min.x + 85, screen->r.min.y +22, screen->r.min.x + 85 + 33, screen->r.min.y + 43), uiend[1], nil, ZP);
		draw(screen, Rect(screen->r.min.x + 85, screen->r.min.y+42, screen->r.min.x + 85 + 33, screen->r.min.y + 63), uiend[2], nil, ZP);


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
