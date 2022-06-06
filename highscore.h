#ifndef __highheader__
#define __highheader__

#include <windows.h>
#include <stdio.h>
#include "config.h"
#include "backbuffer.h"

struct itemhs {
    char name[30];
    int score;
};

class highscore
{
    public:
    FILE *fp;
    struct itemhs hslist[MAX_HS];
    
    highscore();
/* nome construtor deve ser o nome da classe,
   construtor serve para criar um objeto      */
    ~highscore();
/* nome do destrutor deve ser o nome da classe
   com o ~anterior, destruir um objeto        */
    void load ();
    boolean test (int score);
    void insert (struct itemhs *hstoadd);
    void print_table(HWND hwnd, backbuffer *bf);
};

#endif
