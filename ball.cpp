#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "vector.h"
#include "ball.h"

#if DEBUGMOD==1
	#include <stdio.h>
	extern FILE *fpdebug;
#endif

extern HINSTANCE g_hInst;

extern int ball_holes[];

void copy_qm (struct qtd_mov *to, struct qtd_mov *src)
{
	to->m = src->m;
	to->speed.copy(&src->speed);
}

ball::ball (HWND hwnd, int bid, char *bpic, char *bmask, pixel br, float bm, pixel xpos, pixel ypos)
{
	this->pic = LoadBitmap(g_hInst, bpic);
	this->mask = LoadBitmap(g_hInst, bmask);
	if (!this->pic || !this->mask) {
    	MessageBox(hwnd, "Load of resources failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	this->id = bid;
	this->r = (pixel)br;
	this->qm.m = bm;
	this->x = xpos;
	this->y = ypos;
}

ball::~ball ()
{
	DeleteObject(this->pic);
	DeleteObject(this->mask);
}

void ball::update_pos()
{
	vector ds;
	#if DEBUGMOD==1
		fprintf(fpdebug, "Updating position from x=%f, y=%f to", this->x, this->y);
	#endif
	ds.copy(&this->qm.speed);
	ds.multiply((pixel)TEMPDELAYMS);
	this->x += ds.i;
	this->y += ds.j;
	#if DEBUGMOD==1
		fprintf(fpdebug, " x=%f y=%f\n", this->x, this->y);
	#endif
}

void ball::reduce_speed()
{
	#if DEBUGMOD==1
		fprintf(fpdebug, "Updating speed from %f", this->qm.speed.module());
	#endif
	this->qm.speed.multiply((pixel)0.99);
	#if DEBUGMOD==1
		fprintf(fpdebug, " to %f\n", this->qm.speed.module());
	#endif
}

void ball::stop()
{
	#if DEBUGMOD==1
		fprintf(fpdebug, "\n*************\nball stopped\n\n");
	#endif
	this->qm.speed.set(0, 0, 0);
}

void ball::check_coll_table()
{
	vector normal, proj;
	normal.set((pixel)0, (pixel)0, 0);
	if (this->x < LIMIT_X0+this->r) {
		normal.set((pixel)10, (pixel)0, 0);
		this->x = LIMIT_X0+this->r;
    }
	else if (this->x > LIMIT_XF-this->r) {
		normal.set((pixel)-10, (pixel)0, 0);
		this->x = LIMIT_XF-this->r;
    }
	else if (this->y < LIMIT_Y0+this->r) {
		normal.set((pixel)0, (pixel)10, 0);
		this->y = LIMIT_Y0+this->r;
    }
	else if (this->y > LIMIT_YF-this->r) {
		normal.set((pixel)0, (pixel)-10, 0);
		this->y = LIMIT_YF-this->r;
    }
		
	if (normal.module() != 0) {
		#if DEBUGMOD==1
			fprintf(fpdebug, "\n*************\nColision with wall detected\n\n");
		#endif
		proj.proj_vec( &normal, &this->qm.speed );
		proj.multiply((pixel)2);
		this->qm.speed.add(&proj);
	}
}
	
boolean ball::check_if_dropped ()
{
	int *p;
	p = ball_holes;
	while (*p) {
		if ( sqrt( (this->x - *p)*(this->x - *p) + (this->y - *(++p))*(this->y - *p) )  < this->r+DROP_BALL_FACTOR )
			return TRUE;
		p++;
	}
	return FALSE;
}
	
struct qtd_mov * ball::shock (vector *impulse, ball *from)
{	
	struct qtd_mov *qm_before_impact = NULL;
	
	this->kick_speed.copy(&this->qm.speed);
	this->kick_speed.add(impulse);

	// não posso alocar na pilha, pois não quero perder o dado
	qm_before_impact = (struct qtd_mov *)malloc( sizeof(struct qtd_mov) );
	copy_qm(qm_before_impact, &this->qm);
	
	this->check_collision (from, qm_before_impact);
		
//	if (qm_before_impact->m == this->qm.m) {
		
		#if DEBUGMOD==1
			fprintf(fpdebug, "shocked of %f in ball %i\n", this->qm.speed.module(), this->id);
		#endif
//	}
	
	if (from) { // now i have to update from speed
		vector v, v2;
		v.set(from->x - this->x, from->y - this->y, 0);
		if (this->qm.speed.module() > 0 && vector::cos_ang(&this->qm.speed, &v) > 0) {
//		if (this->qm.speed.module() > 0) {
			v2.proj_vec( &v, &this->qm.speed );
			v2.multiply(this->qm.m / from->qm.m);
		}
		from->qm.speed.subtract(impulse);
		from->qm.speed.add(&v2);
		this->qm.speed.subtract(&v2);
	}
	else { // i'm the first element
		free(qm_before_impact);
		qm_before_impact = NULL;
	}
	this->qm.speed.add(impulse);
	return qm_before_impact;	
}

int ball::check_collision_noimpact ()
{
	ball *l;
	vector v, v2;
	struct qtd_mov *qm_others;
	int bid;
	// *v is the vector that points from a center to another
	// *v2 is the projection
	
	this->coll_checked = TRUE;
	bid = -1;
	
	for (l=this->next; l!=this; l=l->next) {
		#if DEBUGMOD==1
			fprintf(fpdebug, "\n*************\nChecking colision with ball %i and %i - %f - %f\n\n", this->id, l->id, DISTANCE(this, l), this->r+l->r);
		#endif
		if ( DISTANCE(this, l) <= this->r+l->r &&         \
		   l->coll_checked == FALSE ) {
			v.set(l->x - this->x, l->y - this->y, 0);
			bid = l->id;
			
			if (this->qm.speed.module() > 0 && vector::cos_ang(&this->qm.speed, &v) > 0) {
//			if (this->qm.speed.module() > 0) {
				v2.proj_vec( &v, &this->qm.speed );
				v2.multiply(this->qm.m / l->qm.m);
			}
			
		/*	if (l->qm.speed.module() > 0 && vector::cos_ang(&l->qm.speed, &v) < 0) {
				v3.proj_vec( &v, &l->qm.speed );
				v3.multiply(l->qm.m / this->qm.m);
			}*/
			
			//vt.add(&v3);
			//vt.add(&v2);
			#if DEBUGMOD==1
				fprintf(fpdebug, "\n*************\nColision with ball %i and %i detected\n\n", this->id, l->id);
			#endif
			qm_others = l->shock(&v2, this);
			free(qm_others);
		}
	}
	return bid;
}

void ball::check_collision (ball *from, struct qtd_mov *qm_before_impact)
{
	ball *l;
	vector v, v2;
	struct qtd_mov *qm_others=NULL;
	// *v is the vector that points from a center to another
	// *v2 is the projection
	
	this->coll_checked = TRUE;
	
	for (l=this->next; l!=this; l=l->next)
		if ( DISTANCE(this, l) <= this->r+l->r &&         \
		   l->coll_checked == FALSE ) {
			v.set(l->x - this->x, l->y - this->y, 0);
			v2.proj_vec( &v, &this->kick_speed );
			qm_others = l->shock(&v2, this);
			qm_before_impact->m += qm_others->m;
			qm_before_impact->speed.add( &qm_others->speed );
			free(qm_others);
		}
}
