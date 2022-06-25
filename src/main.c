#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "../include/terminal.h"
#include "../include/cutscenes.h"
#include "../include/creatures.h"
#include "../include/object.h"
#include "../include/room.h"

int main(void)
{
    srand(time(NULL)); /* RNG init */

    term_setup();

    cutscene_intro();

    room_t r = obj_calculate_room();
    obj_draw_room(r);
    creature_t player = {.pos.x=r.pos.x + 1, .pos.y=r.pos.y + 1, .stands_on='.', .symbol='@'};
    creature_move_abs(&player, (pos_t){.x=player.pos.x, .y=player.pos.y});

    char input_char = 'a';
    fflush(stdout);
    while(input_char != 'q'){
        input_char = getchar();
        switch(input_char)
        {
            case 'w':{
                char obj_ahead = term_getchar_xy(player.pos.x, player.pos.y - 1);
                if(player.pos.y > 1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=0, .y=-1});
                break;}

            case 'a':{
                char obj_ahead = term_getchar_xy(player.pos.x - 1, player.pos.y);
                if(player.pos.x > 1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=-1, .y=0});
                break;}
        
            case 's':{
                char obj_ahead = term_getchar_xy(player.pos.x, player.pos.y + 1);
                if(player.pos.y < TERM_ROWS_NUM && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=0, .y=1});
                break;}
        
            case 'd':{
                char obj_ahead = term_getchar_xy(player.pos.x + 1, player.pos.y);
                if(player.pos.x < TERM_COLS_NUM && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=1, .y=0});
                break;}

            default:
                break;
        }
        fflush(stdout);
    }

    term_restore_original();

    return 0;
}
