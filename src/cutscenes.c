#include "terminal.h"
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wunused-result"

void cutscene_intro(void)
{
    char runic_text[] = "ᛞᛖᚨᛏᚺ ᛗᚨᛃ ᛒᛖ ᛈᛖᚱᛗᚨᚾᛖᚾᛏ, ᛒᚢᛏ ᚹᚨᛚᚺᚨᛚᛚᚨ ᛁᛊ ᛖᛏᛖᚱᚾᚨᛚ";
    char intro_text[] = "Death may be permanent, but Valhalla is eternal";
    uint16_t y = TERM_ROWS_NUM/2 - 1;
    uint16_t x = TERM_COLS_NUM/2 - sizeof(intro_text)/2;

    term_move_cursor(x, y);
    printf("%s", runic_text);
    fflush(stdout);
    sleep(1);

    term_move_cursor(x, y);
    for(int i = 0; i < (int)sizeof(intro_text); i = i + 2){
        term_putchar_xy(intro_text[i], x + i, y);
        system("sleep 0.05s");
        fflush(stdout);
    }
    for(int i = sizeof(intro_text) - 1; i > 0; i = i - 2){
        term_putchar_xy(intro_text[i], x + i, y);
        system("sleep 0.05s");
        fflush(stdout);
    }
    fflush(stdout);
    sleep(2);
    system("clear");
}