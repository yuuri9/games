typedef struct Settings Settings;
typedef struct DTile DTile;
typedef struct Tile Tile;
typedef struct UIface UIface;
typedef struct GEntity GEntity;
typedef struct statmatrix statmatrix;
typedef struct Item Item;
typedef struct Player Player;
typedef struct status status;
typedef struct Inventory Inventory;
typedef struct Resistance Resistance;

struct Settings {
	ulong seed;
	ulong mapsize;
	uint trate;
};

/*Demo, to be unused*/
struct DTile {
	Image* tile;
	int offx;
	int offy;
};

struct Tile {
	Image* gfx;
	uint prop;
	uint did;
	ushort ie; /*|map for item or entity*/
	Item* it;
	GEntity* en;
	/*Relative locations for server world, use map for client*/
	Tile* north;
	Tile* west;
	Tile* east;
	Tile* south;

};

struct Player{
	GEntity* entities;
	uint maphb;
	Tile* base;
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
	
	ushort elmres[12];

	/*Valid range bitmap & with distance*/
	uint ran;
	
};
struct status{
	uint hp;
	uint mhp;
	uint mp;
	uint mmp;
	uint exp;
	uint mexp;
};
/*seperate out from entity for chests/buildings etc, item array will require being initialized, may require multiple bitmaps if larger than 64 */
struct Inventory{
	/*Slots available, slots filled*/
	uvlong invslots;
	uvlong fslots;
	Item* inventory;
};
struct Resistance{
	ushort ares;
	uint altres[4];
	ushort mares;
	uint maltres[4];

};
struct GEntity {
	Tile* pos;
	uint spriten;
	Image* sprites[4];
	uint type;

	statmatrix;
	status;
	Inventory;
	Resistance;


};

struct Item{
	uint id;
	ushort imap;
	Image* tileimg;
	Image* invimg;
};
