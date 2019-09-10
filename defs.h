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
struct UIface {

};


enum {
	DSTATUSX = 448,
	DSTATUSY = 180,
	DPIPX = 533,
	DPIPY = 182,
	DRENDX = 551,
	DRENDY = 182,
};
