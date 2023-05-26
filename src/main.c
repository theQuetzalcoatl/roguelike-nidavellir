#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "terminal.h"
#include "cutscenes.h"
#include "mob.h"
#include "object.h"
#include "room.h"
#include "corridor.h"
#include "input.h"
#include "display.h"
#include "debug.h"
#include "item.h"

extern void get_objects_from_custom_map(void);
bool custom_mode;


static void handle_ctrl_c(int num)
{
    exit(1); // run the 'atexit' functions
}


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

bool game_running = true;


int main(int argnum, char **argv)
{
    check_terminal_size();
    signal(SIGINT, handle_ctrl_c);
    term_setup();

    srand(time(NULL)); /* RNG init */
    debug_init();

    atexit(term_restore_original);
    atexit(debug_deinit);
    atexit(mob_free_mobs);

    mob_t *player = NULL;
    room_t *r = NULL;

    custom_mode = (argnum == 2 && !strcmp(*(argv + 1), "--custom")) ?  true : false;

    if(custom_mode == true){
        get_objects_from_custom_map();
        player = mob_get_player();
    }
    else{
       // cutscene_intro();
        r = room_create_rooms();
        for(uint8_t n = 0; n < room_get_num_of_rooms(); ++n) room_draw(r[n]);

        player = mob_summon(ID_PLAYER);
        mob_summon(ID_DRAUGR);
        mob_summon(ID_DRAUGR);
        mob_summon(ID_GOBLIN);
        mob_summon(ID_GOBLIN);
    }

    for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_move_to(mob, mob->obj.pos.x, mob->obj.pos.y);
    mob_draw(*player);

    item_spawn();
    item_spawn();
    item_spawn();
    item_spawn();
    item_spawn();

    display_runic_line();
    display_player_stats(*player);

    input_code_t input, step_to = NO_ARROW;

    draw();

    while(game_running){
        nidebug("p:[%d:%d]", player->obj.pos.x,player->obj.pos.y);
        input = get_keypress();
        step_to = NO_ARROW;

        switch(input)
        {
            case ARROW_UP:
            case ARROW_LEFT:
            case ARROW_DOWN:
            case ARROW_RIGHT:
                step_to = input;
                break;

            case 'q':
            case 'Q':
                game_running = false;
                break;

            default:
                nidebug("Unknown input: %i\n", input);
        }

        for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_update(mob, step_to);
        /*  temporarily teleporting the player due to the lack of corridors*/
        if(player->stands_on == ROOM_DOOR && custom_mode == false) {
            int num = CALC_RAND(room_get_num_of_rooms() -1, 0);
            int x = r[num].obj.pos.x + 1;
            int y = r[num].obj.pos.y + 1;
            mob_move_to(player, x, y);
            mob_draw(*player);
        }

        display_runic_line();
        display_player_stats(*player);
        draw();
    }

    /* CLEAN UP */
    if(!player->health && custom_mode == false) cutscene_dead();
    free(r);

    return 0;
}
