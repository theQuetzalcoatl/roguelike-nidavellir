#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "../include/terminal.h"
#include "../include/cutscenes.h"
#include "../include/creatures.h"
#include "../include/object.h"
#include "../include/room.h"
#include "../include/corridor.h"
#include "../include/input.h"


static void update(void)
{
    fflush(stdout);
}



int main(void)
{
    srand(time(NULL)); /* RNG init */

    term_setup();

     //cutscene_intro();

    for(int i = 0; i < 3; ++i)room_draw(room_create());

    room_t r = room_create();
    room_draw(r);

    creature_t player = {.obj.pos.x=r.obj.pos.x + 1, .obj.pos.y=r.obj.pos.y + 1, .stands_on='.', .symbol='@'};
    creature_move_abs(&player, (pos_t){.x=player.obj.pos.x, .y=player.obj.pos.y});

    uint32_t input_char = 'a';
    char obj_ahead = 0;
    fflush(stdout);

    while(input_char != 'q' && input_char != 'Q'){
        input_char = get_keypress();
        switch(input_char)
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
        update();
    }

    term_restore_original();

    return 0;
}
