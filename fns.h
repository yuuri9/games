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
