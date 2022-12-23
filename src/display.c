#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"


void display_runic_line(void)
{
    char *runic_string = "ᛁᚠᚢᚨᚱᛖᚨᛒᛚᛖᛏᛟᚱᛖᚨᛞᛏᚺᛁᛊᛏᚺᛖᚾᛁᚺᛟᛈᛖᚢᛚᛁᚲᛖᛏᚺᛖᚷᚨᛗᛖ"; // ifuareabletoreadthisthenihopeulikethegame

    term_move_cursor(0, RUNIC_LINE_POS);

    printf("%s", runic_string);
    printf("%s", runic_string);
    printf("%s", runic_string);
    printf("%s", runic_string);
}
    

void display_player_stats(const mob_t player)
{
    term_move_cursor(6, RUNIC_LINE_POS + 2);
    printf("health: %d/%d    ", player.health, PLAYER_MAX_HEALTH);
    term_move_cursor(6, RUNIC_LINE_POS + 4);
    printf("level: %d    ", player.level);
}