#include "terminal.h"
#include <unistd.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wunused-result"

void cutscene_intro(void)
{
    char runic_text[] = "ᛞᛖᚨᛏᚺ ᛗᚨᛃ ᛒᛖ ᛈᛖᚱᛗᚨᚾᛖᚾᛏ, ᛒᚢᛏ ᚹᚨᛚᚺᚨᛚᛚᚨ ᛁᛊ ᛖᛏᛖᚱᚾᚨᛚ";
    char intro_text[] = "Death may be permanent, but Valhalla is eternal";
    uint16_t random_places[48] = {}; // this should be the size of 'intro_text' with \0. Could be sizeof(intro_text)-1 but that gets the compiler icky
    uint16_t y = TERM_ROWS_NUM/2 - 1;
    uint16_t x = TERM_COLS_NUM/2 - sizeof(intro_text)/2;
    uint16_t rn = 0;
    uint16_t tmp;

    /* Durstenfeld's in-place shuffle*/
    for(int i = 0; intro_text[i]; ++i) random_places[i] = i;
    for(int i = sizeof(intro_text)-1; i; --i){
        rn = CALC_RAND(i, 0);

        tmp = random_places[i];
        random_places[i] = random_places[rn];
        random_places[rn] = tmp;
    }

    term_move_cursor(x, y);
    printf("%s", runic_text);
    fflush(stdout);
    sleep(1);

    for(uint16_t i = 0; i < sizeof(intro_text); ++i){
        term_putchar_xy(intro_text[random_places[i]], x+random_places[i], y);
        system("sleep 0.05s");
        fflush(stdout);
    }
    fflush(stdout);
    sleep(2);
    system("clear");
}