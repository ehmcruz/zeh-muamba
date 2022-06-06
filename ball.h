#ifndef __ballheader__
#define __ballheader__

#include <windows.h>
#include "config.h"
#include "vector.h"

#define DISTANCE(ball1, ball2)     sqrt( (ball1->x - ball2->x) * (ball1->x - ball2->x) \
                                   + (ball1->y - ball2->y) * (ball1->y - ball2->y) )

struct qtd_mov {
	vector speed;
	float m;	
};

void copy_qm (struct qtd_mov *to, struct qtd_mov *src);

class ball
{
	public:
	ball *next;
	
	int id;
	//owner *myowner;
	boolean coll_checked;
	
	// my position
	pixel x;
	pixel y;
	//pixel z;
	
	pixel r; // raio
	
	HBITMAP pic, mask;
	
	struct qtd_mov qm;
	vector kick_speed;
	
	ball(HWND hwnd, int bid, char *bpic, char *bmask, pixel br, float bm, pixel xpos, pixel ypos);
	~ball();
	
	void update_pos();
	void reduce_speed();
	void stop();
	void check_coll_table();
	boolean check_if_dropped ();
	
	struct qtd_mov * shock (vector *impulse, ball *from);
	void check_collision (ball *from, struct qtd_mov *qm_before_impact);
	int check_collision_noimpact ();
};

#endif
