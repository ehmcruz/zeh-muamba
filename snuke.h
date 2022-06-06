#ifndef __snukeheader__
#define __snukeheader__

#include <windows.h>
#include "config.h"
#include "backbuffer.h"
#include "ball_list.h"
#include "ball.h"
#include "snuke_ia.h"
#include "highscore.h"

#define RENDERED           0
#define RENDER_TIMED_OUT   1
#define RENDERING          0

#define PAR    0
#define IMPAR  1

#define WAITING_PLAYER     0
#define RUNNING            1
#define DISPLAY_MESSAGE    2
#define MSG_DISPLAYED      3
#define SOMEONE_WON        4
#define BIGBALL_DROPPED    5
#define SHOW_TABLE         6
#define PAUSED             7

#define ZEHMUAMBA      0
#define HUMANPLAYER    1

#define BOTH       2
#define NOWINNER   3

class snuke_ia;

class snuke
{
	public:
	HBITMAP tablebmp;
	HBITMAP nobigball;
	backbuffer *bf;
	
	ball_list *list;
	ball *bigball;
	
	int game_status;
	int game_status_saved;
	int render_status;
	
	int mult_factor;
	DWORD setime;
	highscore *hs;
	
	snuke_ia *bot;
	int opponent;
	int winner;
	
	int w, h;
	boolean clicked;
	
	int player;
	boolean switch_player, dropped, bb_touch;
	
	boolean bbdropped;
	
	snuke (HWND hwnd, int xsize, int ysize, char *filename);
	void set_mult_factor (int m);
	~snuke();
	
	void render_next_frame (HWND hwnd, int mx, int my);
	void print_rendered (HWND hwnd, int mx, int my);
	void click ();	
	boolean check_bb_inter();
	void draw_balls();
	void check_if_winner ();
	void pause();
	void unpause();
};

#endif
