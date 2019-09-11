#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <bio.h>

#include "defs.h"
#include "fns.h"

void
eresized(int New){
	if(New && getwindow(display, Refmesg) < 0){
		
		exits("Getwindow Error");
	}
}

void
usage(void){
	fprint(2, "game [-t] tickrate [-f] filename [-s] seed\n");
	exits(nil);
}

void
quit(Biobuf* savfile, Settings* Settings){
	
	save(savfile, Settings);
	exits(nil);
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
	ui->vpoff->xoff = 0;
	ui->vpoff->yoff = 0;
	ui->vpoff->end = 0;
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

void
drawui(UIface* ui, Image* screen){


}
