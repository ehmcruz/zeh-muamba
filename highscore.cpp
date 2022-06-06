#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "highscore.h"
#include "config.h"
#include "backbuffer.h"

/*classe high... funcao high... */
highscore::highscore ()
{
    this->fp = fopen(NOME_ARQ, "r+b");
    if (!this->fp) {
        struct itemhs hs;
        int i;
        this->fp = fopen(NOME_ARQ, "w+b");
        strcpy(hs.name, "VAZIO");
        hs.score = 0;
        for (i=0; i<MAX_HS; i++)
            fwrite(&hs, sizeof(struct itemhs), 1, fp);
        //fclose(this->fp);
        //this->fp = fopen(NOME_ARQ, "r+b");
    }
}

highscore::~highscore ()
{
    fclose(this->fp);
}

void highscore::load ()
{
    rewind(this->fp);
    fread(this->hslist, sizeof(struct itemhs), MAX_HS, this->fp);
}

/* testa se o score foi suficiente para entrar
   na lista dos highscore */
boolean highscore::test (int score)
{
    struct itemhs *hs;
    int i=0;
    for (hs=this->hslist; i<MAX_HS; hs++) {
        if (score > hs->score)
            return TRUE;
        i++;
    }
    return FALSE;
}

void highscore::insert (struct itemhs *hstoadd)
{
    struct itemhs *hs, temp[MAX_HS];
    int pos;
    //find position
    pos = 0;
    for (hs=this->hslist; pos<MAX_HS; hs++) {
        if (hstoadd->score > hs->score)
            break;
        pos++;
    }
//    temp = (struct itemhs *)malloc( sizeof(struct itemhs)*MAX_HS );
    if (pos>0)
        memcpy(temp, this->hslist, sizeof(struct itemhs)*pos);
// void *  memcpy ( void * dest, const void * src, size_t num );
    memcpy(temp+pos, hstoadd, sizeof(struct itemhs));
    if (pos<MAX_HS-1)
        memcpy(temp+pos+1, this->hslist+pos, sizeof(struct itemhs)*(MAX_HS-pos-1));
    memcpy(this->hslist, temp, sizeof(struct itemhs)*MAX_HS);
    //fclose(this->fp);
    //this->fp = fopen(NOME_ARQ, "wb");
    rewind(this->fp);
// posiciona o ponteiro do arquivo no inicio do mesmo
    fwrite(this->hslist, sizeof(struct itemhs), MAX_HS, this->fp);
}

void highscore::print_table(HWND hwnd, backbuffer *bf)
{
    struct itemhs *hs;
    int x0, x1, x2, y, h, l0, l1, l2, i;
    char numero[20];
    h = 15;
    x1 = 100;
    x2 = 400;
    l1 = 280;
    l2 = 100;
    x0 = 80;
    l0 = 18;
    y = 100;
    i = 1;
    for (hs = this->hslist; i<=MAX_HS; hs++) {
        sprintf(numero, "%i", i);
        bf->draw_text(numero, x0, y, x0+l0, y+h);
        bf->draw_text(hs->name, x1, y, x1+l1, y+h);
        sprintf(numero, "%i", hs->score);
        bf->draw_text(numero, x2, y, x2+l2, y+h);
        y += h+1;
        i++;
    }
}
