#ifndef __snukeiaheader__
#define __snukeiaheader__

#include "config.h"
#include "snuke.h"
#include "vector.h"
#include "ball.h"

class snuke;

class snuke_ia
{
	public:
	snuke *game;
	int id;
	
	snuke_ia(snuke *mygame, int myid);
	void select_play();
	void domyplay(vector *way, pixel dist, float bestang);
	boolean check_ifcan_beat (ball *b, vector *way);
	void put_bigball();
};

#endif
