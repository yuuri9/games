typedef struct tocon tocon;

struct tocon {
	Channel* lc;
	Channel* la;
	uint pid;
	uint time;
};

enum{
	LISTENERS = 1,
	LISTENERTIMEOUT = 50,
};

void
pop(tocon** tc, int idx);

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

