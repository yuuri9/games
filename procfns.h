typedef struct tocon tocon;

struct tocon {
	Channel* lc;
	Channel* la;
	uint pid;
	uint time;
	uint conn;
};

enum{
	LISTENERS = 5,
	LISTENERTIMEOUT = 50000,
};

void
pop(tocon* tc, int idx, uint listn);

void
consfn(void* arg);

void
arbiter(void* arg);

void
dialarbiter(void* arg);
void
dialthread(void* arg);

void
timerproc(void* arg);

