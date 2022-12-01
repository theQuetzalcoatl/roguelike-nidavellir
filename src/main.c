#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "terminal.h"
#include "cutscenes.h"
#include "creature.h"
#include "object.h"
#include "room.h"
#include "corridor.h"
#include "input.h"
#include "display.h"


static void draw(void)
{
    fflush(stdout);
}


int main(void)
{
    bool game_running = true;

    srand(time(NULL)); /* RNG init */

    term_setup();

    cutscene_intro();

    room_t *r = room_create_rooms();
    for(int i = 0; i < room_get_num_of_rooms(); ++i) room_draw(r[i]);

    creature_t player = {.obj.pos.x=r[0].obj.pos.x + 1, .obj.pos.y=r[0].obj.pos.y + 1, .stands_on='.', .symbol='@'};
    creature_move_abs(&player, (pos_t){.x=player.obj.pos.x, .y=player.obj.pos.y});

    display_runic_line();

    //corridor_create(&r[0].doors[0], &r[1].doors[0]);

    input_code_t input = 'a';
    char obj_ahead = 0;

    draw();

    while(game_running){
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

            case 'q':
            case 'Q':
                game_running = false;
                break;

            default:
                break;
        }

        /*  temporarily teleporting the player due to the lack of corridors*/
        if(player.stands_on == ROOM_DOOR) {
            int num = CALC_RAND(room_get_num_of_rooms() -1, 0);
            int x = r[num].obj.pos.x + 1;
            int y = r[num].obj.pos.y + 1;
            creature_move_abs(&player, (pos_t){.x=x, .y=y});
        }
        draw();
    }

    /* CLEAN UP*/
    free(r);
    term_restore_original();

    return 0;
}
