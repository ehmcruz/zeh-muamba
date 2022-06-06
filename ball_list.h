#ifndef __balllistheader__
#define __balllistheader__

#include "ball.h"

class ball_list
{
	private:
	ball *bl;
	
	public:
	ball_list();
	~ball_list();

	boolean add(HWND hwnd, int id, char *pic, char *mask, pixel r, float m, pixel xpos, pixel ypos);
	ball * first_element ();
	void exclude (ball *b, ball *ant);
	//boolean remove ();
};

#endif
