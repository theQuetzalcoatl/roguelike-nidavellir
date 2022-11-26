#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "/home/dani/sajat/Villamosmernok/Sajat projektek/Roguelike/include/terminal.h"
#include "cutscenes.h"
#include "creature.h"
#include "object.h"
#include "room.h"
#include "corridor.h"
#include "input.h"


static void draw(void)
{
    fflush(stdout);
}



int main(void)
{
    srand(time(NULL)); /* RNG init */

    term_setup();

     //cutscene_intro();

    room_t *r = room_create_rooms();
    for(int i = 0; i < room_get_num_of_rooms(); ++i) room_draw(r[i]);

    creature_t player = {.obj.pos.x=r[0].obj.pos.x + 1, .obj.pos.y=r[0].obj.pos.y + 1, .stands_on='.', .symbol='@'};
    creature_move_abs(&player, (pos_t){.x=player.obj.pos.x, .y=player.obj.pos.y});

    input_code_t input = 'a';
    char obj_ahead = 0;
    draw();

    while(input != 'q' && input != 'Q'){
        input = get_keypress();
        switch(input)
        {
            case ARROW_UP:
                obj_ahead = term_getchar_xy(player.obj.pos.x, player.obj.pos.y - 1);
                if(player.obj.pos.y > 0 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=0, .y=-1});
                break;

            case ARROW_LEFT:
                obj_ahead = term_getchar_xy(player.obj.pos.x - 1, player.obj.pos.y);
                if(player.obj.pos.x > 0 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=-1, .y=0});
                break;
        
            case ARROW_DOWN:
                obj_ahead = term_getchar_xy(player.obj.pos.x, player.obj.pos.y + 1);
                if(player.obj.pos.y < (signed int)TERM_ROWS_NUM-1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=0, .y=1});
                break;
        
            case ARROW_RIGHT:
                obj_ahead = term_getchar_xy(player.obj.pos.x + 1, player.obj.pos.y);
                if(player.obj.pos.x < (signed int)TERM_COLS_NUM-1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=1, .y=0});
                break;

            default:
                break;
        }
        draw();
    }

    /* CLEAN UP*/
    free(r);
    term_restore_original();

    return 0;
}
