#include <math.h>
#include "config.h"
#include "snuke.h"
#include "vector.h"
#include "ball.h"
#include "snuke_ia.h"

extern int ball_holes[];

snuke_ia::snuke_ia (snuke *mygame, int myid)
{
	this->game = mygame;
	this->id = myid;
}

void snuke_ia::select_play ()
{
	ball *b;
	int *hole;
	vector tohole, proj, toball, way, bestway;
	pixel x, y, dist, bestdist;
	float ang, bestang;

	bestang = -2; // flag
	
	for (b=this->game->bigball->next; b!=this->game->bigball; b=b->next) {
		if ((b->id%2) == this->id) {
			for (hole=ball_holes; *hole; hole+=2) {
				tohole.set(hole[0] - b->x, hole[1] - b->y, 0);
				proj.copy(&tohole);
				proj.divide( tohole.module() );
				proj.multiply( b->r + this->game->bigball->r);
				x = b->x - proj.i;
				y = b->y - proj.j;
				dist = tohole.module();
				way.set(x - this->game->bigball->x, y - this->game->bigball->y, 0);
				ang = vector::cos_ang(&tohole, &way);
				if (check_ifcan_beat(b, &way) || bestang==-2) {
					if (ang > bestang) {
						bestway.copy(&way);
						bestdist = dist;
						bestang = ang;
					}
				}
			}
		}
	}
	this->domyplay(&bestway, bestdist, bestang);
}

void snuke_ia::domyplay (vector *way, pixel dist, float bestang)
{
	pixel dist2;
	dist2 = way->module();
	way->divide(dist2);
	way->multiply(dist+dist2);
	way->multiply(1/bestang);
	way->multiply(2);

	if (way->module() > MAXIMPULSE)
		way->set_module(MAXIMPULSE);
	this->game->bigball->shock(way, NULL);
}

boolean snuke_ia::check_ifcan_beat (ball *b, vector *way)
{
	ball *b2;
	pixel dist, maxdist;
	vector toball, prodv;
		
	// i can beat the front of the ball
		
	maxdist = sqrt( (b->r + this->game->bigball->r)*(b->r + this->game->bigball->r) \
	           + way->module()*way->module() ) + b->r;
		
	for (b2=b->next; b2!=b; b2=b2->next) {
		if (b2 != this->game->bigball) {
			toball.set(b2->x - this->game->bigball->x, b2->y - this->game->bigball->y, 0);
			prodv.prod_vec(way, &toball);
			dist = prodv.module() / way->module();
				
			if (dist<(b->r + this->game->bigball->r) && \
			    DISTANCE(b2, b)<maxdist && DISTANCE(b2, this->game->bigball)<maxdist)
				return FALSE; // b2 is between b and bigball
		}
	}
	return TRUE; // no balls in the way
}

void snuke_ia::put_bigball ()
{
	#define DFACTOR   (this->game->bigball->r*3)
	pixel x, y;
	for (x = LIMIT_X0+DFACTOR; x<LIMIT_XF-DFACTOR; x+=DFACTOR)
		for (y = LIMIT_Y0+DFACTOR; y<LIMIT_YF-DFACTOR; y+=DFACTOR) {
			this->game->bigball->x = x;
			this->game->bigball->y = y;	
			if (this->game->check_bb_inter())
				return;
		}
}
