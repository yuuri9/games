#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <bio.h>

#include "defs.h"
#include "fns.h"



void
eresized(int New){
	if(New && getwindow(display, Refmesg) < 0){
		
		threadexitsall("Getwindow Error");
	}
}

void
usage(void){
	fprint(2, "game [-k] [-t] tickrate [-f] filename [-s] seed [-m] mapsize\n");
	threadexitsall(nil);
}

void
susage(void){
	fprint(2, "server [-p] playerdb [-w] world/ [-s] seed [-c] config [-i] service [-o] console \n");
	threadexitsall(nil);
}

void
quit(Biobuf* savfile, Settings* Settings){
	
	save(savfile, Settings);
	threadexitsall(nil);
}

/*You must open the file before every save and it will be closed in the process*/
void
save(Biobuf* savfile, Settings* Settings){
	Bprint(savfile, "seed=%uld\n",Settings->seed);
	Bterm(savfile);
}

uint
loadsave(Settings* Settings, Biobuf* savfile){
	char* line;
	for(;(line=Brdstr(savfile,'\n',1)) != 0;free(line)){
		if(Blinelen(savfile) > 5 && strncmp("seed=",line,5) == 0){
			Settings->seed = strtoul(&line[5],nil,10);
			return 1;
		}
	}
	return 0;
}
void
initui(UIface* ui, Display* display, Image* spritesheet){
	uint i;
	ui->viewport = allocimage(display, Rect(0,0,DSTATUSLX, DSTATUSLY), RGBA32, 0, DTransparent);
	draw(ui->viewport, Rect(-DSTATUSX, -DSTATUSY,DSTATUSLX, DSTATUSLY), spritesheet, nil, ZP);
	ui->etd = 0x701;
	
	for(i=0;i<7;++i){
		/*Init endpoints*/
		if(i<3){
			ui->end[i] = allocimage(display, Rect(0,0,DENDLX, DENDLY), RGBA32, 0, DTransparent);
			draw(ui->end[i], Rect(-DENDX, -DRENDY - (i * (DBENDY - DRENDY)), DENDLX, DENDLY), spritesheet, nil, ZP);
		}
		ui->pip[i] = allocimage(display, Rect(0,0,DPIPLX, DPIPLY), RGBA32, 0, DTransparent);
	}
	draw(ui->pip[0], Rect(-DPIPX, -DPIPY, DPIPLX, DPIPLY), spritesheet, nil, ZP);
	draw(ui->pip[1], Rect(-DPIP2X, -DPIP2Y, DPIPLX, DPIPLY), spritesheet, nil, ZP);
	draw(ui->pip[2], Rect(-DPIP3X, -DPIP3Y, DPIPLX, DPIPLY), spritesheet, nil, ZP);
	draw(ui->pip[3], Rect(-DPIP4X, -DPIP4Y, DPIPLX, DPIPLY), spritesheet, nil, ZP);
	draw(ui->pip[4], Rect(-DPIP5X, -DPIP5Y, DPIPLX, DPIPLY), spritesheet, nil, ZP);
	draw(ui->pip[5], Rect(-DPIP6X, -DPIP6Y, DPIPLX, DPIPLY), spritesheet, nil, ZP);
	draw(ui->pip[6], Rect(-DPIP7X, -DPIP7Y, DPIPLX, DPIPLY), spritesheet, nil, ZP);





}

/*Initialization for single-tile entities*/
void
initentity(GEntity* entity, uint type, uint mhp, uint mmp, uint mexp, Image* spritesheet, Display* display){
	entity->spriten = 1;
 	entity->sprites[0] = allocimage(display, Rect(0,0,32,32), RGBA32, 0, DTransparent);
 	draw(entity->sprites[0], Rect(-1 * (32 * (type%64)), -1 * (32*(type/64)), 32, 32),spritesheet, nil, ZP); 

 
	entity->type = type;
	entity->hp = mhp;
	entity->mhp = mhp;
	entity->mmp = mmp;
	entity->mp = mmp;
	entity->mexp = mexp;
	entity->exp = 0;
	entity->ares = 0;
	entity->mares = 0;
	entity->wis = 0;
	entity->str = 0;
	entity->dex = 0;
	entity->itg = 0;
	entity->luk = 0;
	entity->spd = 0;
	entity->def = 0;
	entity->mdef = 0;
	entity->att = 0;
	entity->matt = 0;
	entity->aelm = 0;
	entity->aelmp = 0;
	entity->melm = 0;
	entity->melmp = 0;
 
}

void
drawentity(GEntity* entity,Image* screen, uint xoff, uint yoff){
	uint i;
	for(i=2; i< entity->spriten && i < 4;++i){
		draw(screen, Rect(screen->r.min.x + xoff, screen->r.min.y + yoff, screen->r.min.x + xoff + 32, screen->r.min.y + yoff + 32), entity->sprites[i], nil, ZP);
	}
	draw(screen, Rect(screen->r.min.x + xoff, screen->r.min.y + yoff, screen->r.min.x + xoff + 32, screen->r.min.y + yoff + 32), entity->sprites[0], nil, ZP);
	if(entity->spriten >= 2){
		draw(screen, Rect(screen->r.min.x + xoff, screen->r.min.y + yoff - 32, screen->r.min.x + xoff + 32, screen->r.min.y +yoff), entity->sprites[1], nil, ZP);
	}
	/*We will have to add a draw-item loop too*/


}
void
drawui(UIface* ui, Image* screen, GEntity* entity, uint xoff,uint yoff){
		uint i;
		draw(screen, Rect(screen->r.min.x +xoff, screen->r.min.y +yoff, screen->r.min.x +xoff + DSTATUSLX, screen->r.min.y +yoff + DSTATUSLY ),ui->viewport,nil,ZP);

		for(i=0;i<entity->mhp;++i){
			draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i), screen->r.min.y +yoff +4 , screen->r.min.x +xoff + DSTATUSLX + DPIPLX + (DPIPLX * i), screen->r.min.y +yoff + 4 + DPIPLY), ui->pip[0], nil, ZP);
			if( i<(entity->hp)){
				draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i), screen->r.min.y +yoff + 6, screen->r.min.x +xoff + DSTATUSLX + DPIPLX + (DPIPLX * i), screen->r.min.y +yoff + 6 + DPIPLY),ui->pip[1], nil, ZP);
			}


		}
		draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX   + (DPIPLX * i), screen->r.min.y +yoff + 2, screen->r.min.x +xoff + DSTATUSLX + DPIPLX  + DENDLX + (DPIPLX * i), screen->r.min.y +yoff + 2 + DENDLY ), ui->end[0], nil, ZP);

		for(i=0;i<entity->mmp;++i){
				draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i), screen->r.min.y +yoff + 8 + DPIPLY , screen->r.min.x +xoff + DSTATUSLX + DPIPLX + (DPIPLX * i),screen->r.min.y +yoff + 8 + DPIPLY + DPIPLY), ui->pip[0], nil, ZP);

			if(i<entity->mp){
				draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i), screen->r.min.y +yoff + 9 + DPIPLY , screen->r.min.x +xoff + DSTATUSLX + DPIPLX + (DPIPLX * i),screen->r.min.y +yoff + 9 + DPIPLY + DPIPLY), ui->pip[3], nil, ZP);
			}
		}
		draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i), screen->r.min.y +yoff + 2 + DENDLY , screen->r.min.x +xoff + 4 + DSTATUSLX + DENDLX + (DPIPLX * i), screen->r.min.y +yoff + 6 + DENDLY + DENDLY), ui->end[1], nil, ZP);
		
		for(i=0;i<entity->mexp;++i){

			draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i),  screen->r.min.y +yoff + 12  + DPIPLY + DPIPLY , screen->r.min.x +xoff + DSTATUSLX + DPIPLX + (DPIPLX * i),screen->r.min.y +yoff + 12  + DPIPLY + DPIPLY + DPIPLY ),ui->pip[0], nil, ZP);
			if(i<entity->exp){
		
				draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i),  screen->r.min.y +yoff + 13  + DPIPLY + DPIPLY , screen->r.min.x +xoff + DSTATUSLX + DPIPLX + (DPIPLX * i),screen->r.min.y +yoff + 13  + DPIPLY + DPIPLY + DPIPLY ), ui->pip[5], nil, ZP);
			}
		}
		draw(screen, Rect(screen->r.min.x +xoff + DSTATUSLX + (DPIPLX * i), screen->r.min.y +yoff + 2  + DENDLY + DENDLY, screen->r.min.x +xoff + DSTATUSLX + DENDLX + (DPIPLX * i), screen->r.min.y +yoff + 2 + DENDLY + DENDLY + DENDLY), ui->end[2], nil, ZP);


		drawentity(entity, screen, 16 + xoff, 15 + yoff);

}


