#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "terminal.h"
#include "cutscenes.h"
#include "mob.h"
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

static void check_terminal_size(void)
{
    struct winsize ws = {0};
    ioctl(0, TIOCGWINSZ, &ws);

    if(ws.ws_col < TERM_COLS_NUM || ws.ws_row < TERM_ROWS_NUM){
        printf("Terminal window is currently %dx%d. Make it at least %dx%d large and try again!\n", ws.ws_col, ws.ws_row, TERM_COLS_NUM, TERM_ROWS_NUM);
        exit(1);
    }
}


int main(void)
{
    bool game_running = true;

    check_terminal_size();
    term_setup();
    
    srand(time(NULL)); /* RNG init */
    debug_init();

    atexit(term_restore_original);
    atexit(debug_deinit);
    atexit(mob_free_mobs);

    //cutscene_intro();

    room_create_rooms();
    room_t *r = room_get_rooms();
    for(uint8_t n = 0; n < room_get_num_of_rooms(); ++n) room_draw(r[n]);

    mob_t *player = mob_summon(ID_PLAYER);
    mob_summon(ID_DRAUGR);
    for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_move_to(mob, mob->obj.pos);

    display_runic_line();
    display_player_stats(*player);

    input_code_t input = 'a';
    char obj_ahead = 0;

    draw();

    while(game_running){
        input = get_keypress();
        switch(input)
        {
            case ARROW_UP:
                obj_ahead = term_getchar_xy(player->obj.pos.x, player->obj.pos.y - 1);
                if(player->obj.pos.y > 0 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    mob_move_by(player, (pos_t){.x=0, .y=-1});
                }
                break;

            case ARROW_LEFT:
                obj_ahead = term_getchar_xy(player->obj.pos.x - 1, player->obj.pos.y);
                if(player->obj.pos.x > 0 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    mob_move_by(player, (pos_t){.x=-1, .y=0});
                }
                break;
        
            case ARROW_DOWN:
                obj_ahead = term_getchar_xy(player->obj.pos.x, player->obj.pos.y + 1);
                if(player->obj.pos.y < (signed int)TERM_ROWS_NUM-1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    mob_move_by(player, (pos_t){.x=0, .y=1});
                }
                break;
        
            case ARROW_RIGHT:
                obj_ahead = term_getchar_xy(player->obj.pos.x + 1, player->obj.pos.y);
                if(player->obj.pos.x < (signed int)TERM_COLS_NUM-1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)){
                    mob_move_by(player, (pos_t){.x=1, .y=0});
                }
                break;

            case 'q':
            case 'Q':
                game_running = false;
                break;

            default:
                break;
        }

        for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_update_mob(mob, player);

        /*  temporarily teleporting the player due to the lack of corridors*/
        if(player->stands_on == ROOM_DOOR) {
            int num = CALC_RAND(room_get_num_of_rooms() -1, 0);
            int x = r[num].obj.pos.x + 1;
            int y = r[num].obj.pos.y + 1;
            mob_move_to(player, (pos_t){.x=x, .y=y});
        }
        display_runic_line();
        display_player_stats(*mob_get_mobs());
        draw();
    }

    /* CLEAN UP*/
    free(r);

    return 0;
}
