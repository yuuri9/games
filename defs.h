typedef struct Settings Settings;
typedef struct DTile DTile;
typedef struct Tile Tile;
typedef struct UIface UIface;
typedef struct GEntity GEntity;
typedef struct statmatrix statmatrix;
typedef struct Item Item;

struct Settings {
	ulong seed;
};

/*Demo, to be unused*/
struct DTile {
	Image* tile;
	int offx;
	int offy;
};

struct Tile{
	Image* gfx;
	uint prop;
	uint did;
};
enum {
	DSTATUSX = 448,
	DSTATUSY = 180,
	DSTATUSLX = 84,
	DSTATUSLY = 64,
	DPIPX = 533,
	DPIPY = 184,
	DPIP2X = 592,
	DPIP2Y = 186,
	DPIP3X = 601,
	DPIP3Y = 186,
	DPIP4X = 592,
	DPIP4Y = 206,
	DPIP5X = 601,
	DPIP5Y = 206,
	DPIP6X = 592,
	DPIP6Y = 226,
	DPIP7X = 601,
	DPIP7Y = 226,
	DPIPLX = 8,
	DPIPLY = 16,
	DPIPL2Y = 12,
	DENDX = 551,
	DRENDY = 182,
	DENDLX = 32,
	DENDLY = 20,
	DBENDY = 202,
	DGENDY = 222,
	
};
struct UIface {

	uint etd;
	Image* viewport;
	Image* pip[7];
 	Image* end[3];
};

struct statmatrix{
	uint wis;
	uint str;
	uint dex;
	uint itg;
	uint luk;
	

	uint spd;
	uint def;
	uint mdef;
	uint att;
	uint aelm;
	uint aelmp;

	uint matt;
	uint melm;
	uint melmp;

	/*Valid range bitmap & with distance*/
	uint ran;
	
};
struct GEntity {
	uint spriten;
	Image* sprites[4];
	statmatrix stt;
	uint type;
	uint hp;
	uint mhp;
	uint mp;
	uint mmp;
	uint exp;
	uint mexp;
	ushort ares;
	uint altres[4];
	ushort mares;
	uint maltres[4];

	/*Slots available, slots filled*/
	uint invslots;
	uint fslots;
	Item* inventory[50];

};

struct Item{
	uint id;
	ushort imap;
	Image* tileimg;
	Image* invimg;
};
