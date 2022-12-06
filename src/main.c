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
#include "debug.h"


static void draw(void)
{
    fflush(stdout);
}

#include <sys/ioctl.h>

int main(void)
{
    bool game_running = true;

    srand(time(NULL)); /* RNG init */
    debug_init();

    struct winsize ws = {0};

    ioctl(0, TIOCGWINSZ, &ws);

    printf("screen -> %dx%d\n", ws.ws_col, ws.ws_row);

    if(ws.ws_col < TERM_COLS_NUM || ws.ws_row < TERM_ROWS_NUM){
        printf("Terminal window is currently too small. Make it at least %dx%d large and try again!\n", TERM_COLS_NUM, TERM_ROWS_NUM);
        exit(1);
    }
    term_setup();
    atexit(term_restore_original);
    atexit(debug_deinit);

    room_t *r = room_create_rooms();
    for(uint8_t n = 0; n < room_get_num_of_rooms(); ++n) room_draw(r[n]);

    creature_t *player = creature_summon(ID_PLAYER);

    player->obj.pos.x = r[0].obj.pos.x + 1;
    player->obj.pos.y = r[0].obj.pos.y + 1;
    creature_move_abs(player, (pos_t){.x=player->obj.pos.x, .y=player->obj.pos.y});

    display_runic_line();
    display_player_stats(*player);

    input_code_t input = 'a';
    char obj_ahead = 0;

    //cutscene_intro();
    draw();

    while(game_running){
        input = get_keypress();
        switch(input)
        {
            case ARROW_UP:
                obj_ahead = term_getchar_xy(player->obj.pos.x, player->obj.pos.y - 1);
                if(player->obj.pos.y > 0 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    creature_move_rel(player, (pos_t){.x=0, .y=-1});
                }
                break;

            case ARROW_LEFT:
                obj_ahead = term_getchar_xy(player->obj.pos.x - 1, player->obj.pos.y);
                if(player->obj.pos.x > 0 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    creature_move_rel(player, (pos_t){.x=-1, .y=0});
                }
                break;
        
            case ARROW_DOWN:
                obj_ahead = term_getchar_xy(player->obj.pos.x, player->obj.pos.y + 1);
                if(player->obj.pos.y < (signed int)TERM_ROWS_NUM-1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    creature_move_rel(player, (pos_t){.x=0, .y=1});
                }
                break;
        
            case ARROW_RIGHT:
                obj_ahead = term_getchar_xy(player->obj.pos.x + 1, player->obj.pos.y);
                if(player->obj.pos.x < (signed int)TERM_COLS_NUM-1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    creature_move_rel(player, (pos_t){.x=1, .y=0});
                }
                break;

            case 'q':
            case 'Q':
                game_running = false;
                break;

            default:
                break;
        }

        /*  temporarily teleporting the player due to the lack of corridors*/
        if(player->stands_on == ROOM_DOOR) {
            int num = CALC_RAND(room_get_num_of_rooms() -1, 0);
            int x = r[num].obj.pos.x + 1;
            int y = r[num].obj.pos.y + 1;
            creature_move_abs(player, (pos_t){.x=x, .y=y});
        }
        display_runic_line();
        display_player_stats(*creature_get_creatures());
        draw();
    }

    /* CLEAN UP*/
    free(r);

    return 0;
}
