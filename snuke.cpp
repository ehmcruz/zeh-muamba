#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "config.h"
#include "snuke.h"
#include "backbuffer.h"
#include "ball_list.h"
#include "ball.h"
#include "vector.h"
#include "snuke_ia.h"
#include "highscore.h"
#include "resources.h"

extern void *data_exchange;
extern HINSTANCE g_hInst;
int ball_holes[] = { STARTX+36, STARTY+36,
                      STARTX+36, STARTY+364,
					  STARTX+664, STARTY+36,
					  STARTX+664, STARTY+364,
					  STARTX+350, STARTY+36,
					  STARTX+350, STARTY+364,
					  0 };
char textofm[50];
#if DEBUGMOD==1
	extern FILE *fpdebug;
#endif

snuke::snuke (HWND hwnd, int xsize, int ysize, char *filename)
{
	this->tablebmp = LoadBitmap(g_hInst, "TABLEBMP");
	this->nobigball = LoadBitmap(g_hInst, "NOBIGBALLBMP");
	if (!(this->tablebmp && this->nobigball)) {
    	MessageBox(hwnd, "Load of resources failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	this->bf = new backbuffer(hwnd, xsize, ysize);
	this->list = new ball_list;
	this->w = xsize;
	this->h = ysize;
	this->clicked = FALSE;
	this->game_status = WAITING_PLAYER;
	this->player = IMPAR;
	this->winner = NOWINNER;
	this->mult_factor = 4;
	this->setime = GetTickCount();
	this->hs = new highscore;
	strcpy(textofm, "Fator de multiplicação: 4");
	
	if (filename == NULL) {
		if (MessageBox(hwnd, "Você quer jogar contra o Zeh Muamba?", "Oponente", MB_YESNO)==IDYES)  {
			this->opponent = ZEHMUAMBA;
			this->bot = new snuke_ia(this, PAR);
		}
		else {
			this->opponent = HUMANPLAYER;
		}
		this->list->add(hwnd, 0, "BIGBALLBMP", "BIGBALLMASKBMP", BIGBALL_R, BIGBALL_M, STARTX+200, STARTY+TABLE_YSIZE/2);
		this->bigball = this->list->first_element();
	
		this->list->add(hwnd, 1, "BALL1_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+100, STARTY+100);
		this->list->add(hwnd, 2, "BALL2_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+300, STARTY+173);
		this->list->add(hwnd, 3, "BALL3_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+277, STARTY+161);
		this->list->add(hwnd, 4, "BALL4_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+180, STARTY+320);
		this->list->add(hwnd, 5, "BALL5_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+300, STARTY+210);
		this->list->add(hwnd, 6, "BALL6_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+500, STARTY+300);
		this->list->add(hwnd, 7, "BALL7_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+550, STARTY+161);
		this->list->add(hwnd, 8, "BALL8_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+600, STARTY+320);
		this->list->add(hwnd, 9, "BALL9_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+620, STARTY+210);
		this->list->add(hwnd, 10, "BALL10_BMP", "BALL1_MASKBMP", SMALLBALL_R, SMALLBALL_M, STARTX+450, STARTY+300);
	}
}

snuke::~snuke ()
{
    delete this->hs;
	if (this->opponent == ZEHMUAMBA)
		delete this->bot;
	delete this->list;
	DeleteObject(this->tablebmp);
	DeleteObject(this->nobigball);
	delete this->bf;
}

void snuke::print_rendered (HWND hwnd, int mx, int my)
{
	if (this->render_status == RENDERED) {
		#if DEBUGMOD==1
			fprintf(fpdebug, "printed rendered frame\n");
		#endif
		this->bf->print(hwnd);
		this->render_next_frame(hwnd, mx, my);
	}
	else
		this->render_status = RENDER_TIMED_OUT;
}

void snuke::render_next_frame (HWND hwnd, int mx, int my)
{
	this->render_status = RENDERING;
	this->bf->clear();
	#if DEBUGMOD==1
		fprintf(fpdebug, "rendering frame\n");
	#endif

	if (this->opponent == ZEHMUAMBA)
		this->bf->draw_text("Jogando contra o Zeh Muamba", 0, (int)(STARTY/2), SCREEN_XSIZE, STARTY);
	else
		this->bf->draw_text("Humano vs Humano", 0, (int)(STARTY/2), SCREEN_XSIZE, STARTY);
   	this->bf->draw_text(textofm, 0, 0, SCREEN_XSIZE, (int)(STARTY/2));
	this->bf->draw_picture(this->tablebmp, STARTX, STARTY, this->w, this->h);

	if (this->game_status == WAITING_PLAYER) {
		if (this->opponent == HUMANPLAYER || this->player==IMPAR) {
			vector line;
			int xf, yf;
			
			textofm[24] = this->mult_factor + ASCII_N_START;
       	
			line.set((pixel)(this->bigball->x - (pixel)mx), (pixel)(this->bigball->y - (pixel)my), 0);
			line.divide( line.module() );
			line.multiply(LINESIZE);
			xf = (int)(this->bigball->x + line.i);
			yf = (int)(this->bigball->y + line.j);
			this->bf->draw_line( mx, my, \
			                      xf, yf );
			this->bbdropped = FALSE;
			this->draw_balls();
			if (this->player == IMPAR)
				this->bf->draw_text("ESPERANDO JOGADOR ÍMPAR", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
			else
				this->bf->draw_text("ESPERANDO JOGADOR PAR", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
		
			if (this->clicked == TRUE) {
				vector impulse;
				ball *b;
				#if DEBUGMOD==1
					fprintf(fpdebug, "\n*************\nclick detected\n\n");
				#endif
				impulse.set((pixel)(this->bigball->x - (pixel)mx), (pixel)(this->bigball->y - (pixel)my), 0);
				impulse.multiply((pixel)this->mult_factor);
				#if DEBUGMOD==1
					fprintf(fpdebug, "impulse of %f\n", impulse.module());
				#endif
				this->game_status = RUNNING;
				this->clicked = FALSE;
				this->bb_touch = NOTSETTED;
				this->dropped = FALSE;
			
				this->bigball->coll_checked = FALSE;
				for (b=this->bigball->next; b!=this->bigball; b=b->next)
					b->coll_checked = FALSE;
			
				if (impulse.module() > MAXIMPULSE)
					impulse.set_module(MAXIMPULSE);
				#if DEBUGMOD==1
					fprintf(fpdebug, "Click x=%f y=%f\n", impulse.i, impulse.j);
				#endif
				this->bigball->shock(&impulse, NULL);
			}
		}
		else {
			this->bot->select_play();
			this->game_status = RUNNING;
			this->bb_touch = FALSE;
			this->dropped = FALSE;
			this->bb_touch = NOTSETTED;
			this->bbdropped = FALSE;
		}
	}
	else if (this->game_status == RUNNING) {
		ball *b, *ant;
		boolean active;
		
		this->draw_balls();
		this->bf->draw_text("SIMULANDO...", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
		
		this->bigball->coll_checked = FALSE;
		for (b=this->bigball->next; b!=this->bigball; b=b->next)
			b->coll_checked = FALSE;
		
		active = FALSE;
		
		if (this->bbdropped == FALSE) {
			if (this->bigball->check_if_dropped() == FALSE) {
				int test;
				this->bigball->update_pos();
				this->bigball->reduce_speed();
				this->bigball->check_coll_table();
			
				test = this->bigball->check_collision_noimpact();
				if (test!=-1 && this->bb_touch==NOTSETTED)
					if ((test % 2) == this->player)
						this->bb_touch = TRUE;
					else
						this->bb_touch = FALSE;
			
				if (this->bigball->qm.speed.module() < BALLSTOPLIMIT)
					this->bigball->stop();
				else
					active = TRUE;
			}
			else {
				this->bbdropped = TRUE;
				this->bigball->x = this->bigball->y = -100;
				this->bigball->coll_checked = TRUE;
				this->bigball->stop();
			}
		}
		
		ant = this->bigball;
		for (b=this->bigball->next; b!=this->bigball; b=b->next) {
			if (b->check_if_dropped() == FALSE) {
				ant = b;
				b->update_pos();
				b->reduce_speed();
				b->check_coll_table();
				if (b->coll_checked == FALSE)
					b->check_collision_noimpact();
				if (b->qm.speed.module() < BALLSTOPLIMIT)
					b->stop();
				else
					active = TRUE;
			}
			else {
				if ((b->id%2) == this->player)
					this->dropped = TRUE;
				this->list->exclude(b, ant);
				b = ant->next;
			}
		}
		
		if (active == FALSE) {
            this->check_if_winner();
			if (this->winner == NOWINNER) { // ninguém ganhou
				this->game_status = DISPLAY_MESSAGE;
			
				if (this->bb_touch==TRUE && this->dropped == TRUE && this->bbdropped == FALSE)
					this->switch_player = FALSE;
				else {
					this->switch_player = TRUE;
					this->player ^= 1;
				}
			}
			else { // alguém ganhou
                this->setime = GetTickCount() - this->setime;
                this->setime /= 1000;
                this->setime = 20000000 / this->setime;
                data_exchange = &this->setime;
                this->hs->load();
                if (this->opponent == ZEHMUAMBA && this->winner == IMPAR && this->hs->test(setime)) {
                    //this->pause();
                    KillTimer(hwnd, TEMPID);
                    DialogBox(g_hInst, (LPSTR)IDD_SCOREBOX, hwnd, (DLGPROC)ScoreDlgProc);
                    SetTimer(hwnd, TEMPID, TEMPDELAY, NULL);
                    //this->unpause();
                }
                sprintf(textofm, "%i pontos", this->setime);
				this->game_status = SOMEONE_WON;
            }
		}
	}
	else if (this->game_status == BIGBALL_DROPPED) {
		if (this->opponent == HUMANPLAYER || this->player==IMPAR) {
			this->draw_balls();
			
			if (this->player == PAR)
				this->bf->draw_text("ESCOLHA A POSIÇÃO DO BOLÃO JOGADOR PAR", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
			else
				this->bf->draw_text("ESCOLHA A POSIÇÃO DO BOLÃO JOGADOR ÍMPAR", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
			this->bigball->x = mx;
			this->bigball->y = my;
			if (mx>LIMIT_X0+this->bigball->r && mx<LIMIT_XF-this->bigball->r \
				&& my>LIMIT_Y0+this->bigball->r && my<LIMIT_YF-this->bigball->r \
				&& this->check_bb_inter()) {
				this->bf->draw_picture(this->bigball->pic, this->bigball->mask, \
		                       (int)(this->bigball->x - this->bigball->r), \
							   (int)(this->bigball->y - this->bigball->r), \
							   (int)this->bigball->r*2, (int)this->bigball->r*2);
				if (this->clicked == TRUE) {
					KillTimer(hwnd, TEMPID);
					this->game_status = SHOW_TABLE;
					this->bf->print(hwnd);
					SetTimer(hwnd, TEMPID, 1000, NULL);
				}
			}
			else {
				this->bf->draw_picture(this->nobigball, this->bigball->mask, \
		                       (int)(this->bigball->x - this->bigball->r), \
							   (int)(this->bigball->y - this->bigball->r), \
							   (int)this->bigball->r*2, (int)this->bigball->r*2);
				this->clicked = FALSE;
			}
		}
		else {
			KillTimer(hwnd, TEMPID);
			this->bbdropped = FALSE;
			this->bot->put_bigball();
			this->draw_balls();
			this->bf->draw_text("O ZEH JÁ ESCOLHEU A POSIÇÃO", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
			this->bf->print(hwnd);
			this->game_status = SHOW_TABLE;
			SetTimer(hwnd, TEMPID, 2000, NULL);
		}
	}
	else if (this->game_status == DISPLAY_MESSAGE) {
		#if DEBUGMOD==1
			fprintf(fpdebug, "MSG printed\n");
		#endif
		KillTimer(hwnd, TEMPID);
		this->game_status = MSG_DISPLAYED;
		this->draw_balls();
		if (this->switch_player == FALSE)
			this->bf->draw_text("ACERTOU PORQUE A JOGADA ERA TRIVIAL...", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
		else
			this->bf->draw_text("ERROU...", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
		this->bf->print(hwnd);
		SetTimer(hwnd, TEMPID, 2000, NULL);
	}
	else if (this->game_status == MSG_DISPLAYED) {
		#if DEBUGMOD==1
			fprintf(fpdebug, "Restarting loop\n");
		#endif
		KillTimer(hwnd, TEMPID);
		if (this->bbdropped == TRUE)
			this->game_status = BIGBALL_DROPPED;
		else
			this->game_status = WAITING_PLAYER;
		this->clicked = FALSE;
		SetTimer(hwnd, TEMPID, TEMPDELAY, NULL);
	}
	else if (this->game_status == SHOW_TABLE) {
		#if DEBUGMOD==1
			fprintf(fpdebug, "Restarting loop\n");
		#endif
		KillTimer(hwnd, TEMPID);
		this->game_status = WAITING_PLAYER;
		this->clicked = FALSE;
		SetTimer(hwnd, TEMPID, TEMPDELAY, NULL);
	}
	else if (this->game_status == SOMEONE_WON) {
		this->draw_balls();
		this->hs->print_table(hwnd, this->bf);
		if (this->winner == IMPAR)
			this->bf->draw_text("JOGADOR ÍMPAR GANHOU", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
		else if (this->winner == PAR)
			this->bf->draw_text("JOGADOR PAR GANHOU", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
		else
			this->bf->draw_text("EMPATE", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
	}
	else if (this->game_status == PAUSED) {
		this->draw_balls();
		this->bf->draw_text("JOGO PAUSADO", 0, STARTY+TABLE_YSIZE, SCREEN_XSIZE, SCREEN_YSIZE);
	}
		
	if (this->render_status == RENDER_TIMED_OUT) {
		this->bf->print(hwnd);
		this->render_next_frame(hwnd, mx, my);
	}
	else {
		#if DEBUGMOD==1
			fprintf(fpdebug, "frame rendered\n");
		#endif
		this->render_status = RENDERED;
	}
}

boolean snuke::check_bb_inter()
{
	int *p;
	ball *b;
	
	for (b=this->bigball->next; b!=this->bigball; b=b->next)
		if (DISTANCE(b, this->bigball) < this->bigball->r+b->r)
			return FALSE;

	p = ball_holes;
	while (*p) {
		if ( sqrt( (this->bigball->x - *p)*(this->bigball->x - *p) + (this->bigball->y - *(++p))*(this->bigball->y - *p) )  < this->bigball->r+DROP_BALL_FACTOR )
			return FALSE;
		p++;
	}
	return TRUE;
}

void snuke::click()
{
	if (this->game_status == WAITING_PLAYER || this->game_status == BIGBALL_DROPPED)
		this->clicked = TRUE;
}

void snuke::check_if_winner ()
{
	ball *b;
	int win[2];
	win[0] = win[1] = 0;
	for (b=this->bigball->next; b!=this->bigball; b=b->next)
        win[b->id%2]++;

	#if DEBUGMOD==1
		fprintf(fpdebug, "Ímpar: %i --- Par: %i\n", imp, pa);
	#endif
	
	if (win[1] == 0) {
		if (win[0] == 0)
			this->winner = BOTH;
		else
			this->winner = IMPAR;
	}
	else if (win[0] == 0)
		this->winner = PAR;
}
	
void snuke::draw_balls ()
{
	ball *b;
	
	if (this->bbdropped == FALSE) {
		this->bf->draw_picture(this->bigball->pic, this->bigball->mask, \
		                       (int)(this->bigball->x - this->bigball->r), \
							   (int)(this->bigball->y - this->bigball->r), \
							   (int)this->bigball->r*2, (int)this->bigball->r*2);
	}
	
	for (b=this->bigball->next; b!=this->bigball; b=b->next) {
		this->bf->draw_picture(b->pic, b->mask, \
		                       (int)(b->x - b->r), (int)(b->y - b->r), \
							   (int)b->r*2, (int)b->r*2 );
	}
}

void snuke::set_mult_factor (int m)
{
	this->mult_factor = m;
}

void snuke::pause ()
{
    this->game_status_saved = this->game_status;
    this->game_status = PAUSED;
}

void snuke::unpause ()
{
    this->game_status = this->game_status_saved;
}
