#include "terminal.h"
#include <unistd.h>
#include "utils.h"
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wunused-result"

void cutscene_intro(void)
{
    char runic_text[] = "ᛞᛖᚨᛏᚺ ᛗᚨᛃ ᛒᛖ ᛈᛖᚱᛗᚨᚾᛖᚾᛏ, ᛒᚢᛏ ᚹᚨᛚᚺᚨᛚᛚᚨ ᛁᛊ ᛖᛏᛖᚱᚾᚨᛚ";
    char latin_text[] = "Death may be permanent, but Valhalla is eternal";
    uint16_t text_indexes[48] = {}; // this should be the size of 'latin_text' with \0. Could be sizeof(latin_text)-1 but that gets the compiler icky
    uint16_t text_y = TERM_ROWS_NUM/2 - 1;
    uint16_t text_x = TERM_COLS_NUM/2 - sizeof(latin_text)/2;
    uint16_t rn = 0;
    uint16_t tmp;

    /* Durstenfeld's in-place shuffle*/
    for(int i = 0; latin_text[i]; ++i) text_indexes[i] = i;
    for(int i = sizeof(latin_text)-1; i; --i){
        rn = CALC_RAND(i, 0);

        tmp = text_indexes[i];
        text_indexes[i] = text_indexes[rn];
        text_indexes[rn] = tmp;
    }

    term_move_cursor(text_x, text_y);
    printf("%s", runic_text);
    fflush(stdout); sleep(1);

    for(uint16_t i = 0; i < sizeof(latin_text); ++i){
        term_putchar_xy(latin_text[text_indexes[i]], text_x+text_indexes[i], text_y);
        system("sleep 0.05s");
        fflush(stdout);
    }
    fflush(stdout); sleep(2);
    
    system("clear");
}

void cutscene_dead(void)
{
    char runic_text[] = "ᚾᛟ ᚹᚨᛚᚺᚨᛚᛚᚨ ᛏᚺᛁᛊ ᛏᛁᛗᛖ";
    char latin_text[] = "No Valhalla this time";
    uint16_t text_indexes[22] = {}; // this should be the size of 'intro_text' with \0. Could be sizeof(intro_text)-1 but that gets the compiler icky
    uint16_t text_y = TERM_ROWS_NUM/2 - 1;
    uint16_t text_x = TERM_COLS_NUM/2 - sizeof(latin_text)/2;
    uint16_t rn = 0;
    uint16_t tmp;

    system("clear");

        /* Durstenfeld's in-place shuffle*/
    for(int i = 0; latin_text[i]; ++i) text_indexes[i] = i;
    for(int i = sizeof(latin_text)-1; i; --i){
        rn = CALC_RAND(i, 0);

        tmp = text_indexes[i];
        text_indexes[i] = text_indexes[rn];
        text_indexes[rn] = tmp;
    }

    term_move_cursor(text_x, text_y);
    printf("%s", runic_text);
    fflush(stdout); sleep(1);

    for(uint16_t i = 0; i < sizeof(latin_text); ++i){
        term_putchar_xy(latin_text[text_indexes[i]], text_x+text_indexes[i], text_y);
        system("sleep 0.05s");
        fflush(stdout);
    }
    fflush(stdout); sleep(2);
    
    system("clear");
}