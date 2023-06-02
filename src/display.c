#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"


void display_runic_lines(void)
{
    char *runic_string = "ᛁᚠᚢᚨᚱᛖᚨᛒᛚᛖᛏᛟᚱᛖᚨᛞᛏᚺᛁᛊᛏᚺᛖᚾᛁᚺᛟᛈᛖᚢᛚᛁᚲᛖᛏᚺᛖᚷᚨᛗᛖ"; // ifuareabletoreadthisthenihopeulikethegame

    term_move_cursor(0, RUNIC_LINE_POS);

    /* actual characters of the runic line is 41 chars  */
    for(int n = TERM_COLS_NUM/41; n >= 0; --n) printf("%s", runic_string);
    for(uint8_t n = 1; n < TERM_ROWS_NUM - RUNIC_LINE_POS; ++n){
        term_move_cursor(40 - n%2, RUNIC_LINE_POS + n);
        printf("ᚷ");
    }
    for(uint8_t n = 0; n < RUNIC_LINE_POS; ++n){
        term_move_cursor(TERM_COLS_NUM-1 - n%2, n);
        printf("ᚷ");
    }
}
    

void display_player_stats(const mob_t player, const uint64_t turns)
{
    term_move_cursor(6, RUNIC_LINE_POS + 2);
    printf("health: %d/%d    ", player.health, PLAYER_MAX_HEALTH);
    term_move_cursor(6, RUNIC_LINE_POS + 3);
    printf("level: %d    ", player.level);
    term_move_cursor(6, RUNIC_LINE_POS + 4);
    printf("Turns: %ld", turns);
}
