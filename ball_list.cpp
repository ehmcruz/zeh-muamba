#include "ball.h"
#include "ball_list.h"

ball_list::ball_list()
{
	this->bl = NULL;
}

ball_list::~ball_list()
{
	ball *b, *p;
	if (this->bl != NULL) {
		for (b=this->bl->next; b!=this->bl; b=p) {
			p = b->next;
			delete b;
		}
		delete this->bl;
	}
}

boolean ball_list::add(HWND hwnd, int id, char *pic, char *mask, pixel r, float m, pixel xpos, pixel ypos)
{
	ball *nb = new ball(hwnd, id, pic, mask, r, m, xpos, ypos);
	
	if (this->bl == NULL) {
		nb->next = nb;
		this->bl = nb;
	}
	else {
		nb->next = this->bl->next;
		this->bl->next = nb;
	}
	return TRUE;
}

ball * ball_list::first_element ()
{
	return this->bl;
}

void ball_list::exclude (ball *b, ball *ant)
{
	ant->next = b->next;
	delete b;
}
