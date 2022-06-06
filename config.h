#ifndef __configheader__
#define __configheader__

#define NOME_ARQ    "score_table.dat"
#define MAX_HS      20

#define TABLE_XSIZE 700
#define TABLE_YSIZE 400

#define LINESIZE     100
#define MAXIMPULSE   1500

#define SCREEN_XSIZE 790
#define SCREEN_YSIZE 600

#define STARTX       35
#define STARTY       100

#define ASCII_N_START    48

#define LIMIT_X0    (STARTX+36)
#define LIMIT_XF    (STARTX+664)
#define LIMIT_Y0    (STARTY+36)
#define LIMIT_YF    (STARTY+364)

#define BALLSTOPLIMIT   8

#define DEBUGMOD        0

#define BIGBALL_R       16
#define BIGBALL_M       5
#define SMALLBALL_R     12
#define SMALLBALL_M     4

#define DROP_BALL_FACTOR   20

/////////////////////////////
// -- do not edit from here

#define TRUE        1
#define FALSE       0
#define NOTSETTED   -1
#define TEMPDELAY   10
#define TEMPDELAYMS 0.01
#define TEMPID      0

#define NEWGAME     NULL

typedef char boolean;
typedef float pixel;

#endif
