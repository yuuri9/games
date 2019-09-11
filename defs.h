typedef struct Settings Settings;
typedef struct DTile DTile;
typedef struct Tile Tile;
typedef struct UIface UIface;

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
	Tile* north;
	Tile* west;
	Tile* east;
	Tile* south;
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
	Image* viewport;
	Image* pip[7];
	Image* end[3];
};

