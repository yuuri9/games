void
usage(void);

void
eresized(int New);


void
quit(Biobuf* savfile, Settings* Settings);

uint
loadsave( Settings* Settings, Biobuf* savfile);

void
save(Biobuf* savfile, Settings* Settings);

void
initui(UIface* ui, Display* display, Image* spritesheet);

void
drawui(UIface* ui, Image* screen, GEntity* entity,uint xoff,uint yoff);

void
initentity(GEntity* entity, uint type, uint mhp, uint mmp, uint mexp, Image* spritesheet, Display* display);

void
drawentity(GEntity* entity, Image* screen, uint xoff, uint yoff);
