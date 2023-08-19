#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "terminal.h"
#include "cutscenes.h"
#include "mob.h"

#include "room.h"
#include "corridor.h"
#include "input.h"
#include "display.h"
#include "debug.h"
#include "item.h"

extern void get_objects_from_custom_map(void);
extern void event_log_add(const char *event);
bool custom_mode;
bool game_is_running = true;


static void handle_ctrl_c(int num)
{
    exit(1); // run the 'atexit' functions
}


void draw(void)
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
    atexit(item_free_items);

    mob_t *player = NULL;
    room_t *r = NULL;
    uint64_t turns = 0;
    input_code_t input, step_to = NO_ARROW;

    custom_mode = (argnum == 2 && !strcmp(*(argv + 1), "--custom")) ?  true : false;

    if(custom_mode == true) get_objects_from_custom_map();
    else{
        //cutscene_intro();
        r = room_create_rooms();
        for(uint8_t n = 0; n < room_get_num_of_rooms(); ++n) room_draw(r[n]);

        mob_summon(ID_PLAYER); /* player should be summoned first to be updated first, otherwise some mobs be before him, and they see him at a previous point in time */
        mob_summon(ID_DRAUGR);
        mob_summon(ID_DRAUGR);
        mob_summon(ID_GOBLIN);
        mob_summon(ID_GOBLIN);
        for(int i = 10; i; --i) item_spawn();
    }

    player = mob_get_player();
    for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_move_to(mob, mob->pos.x, mob->pos.y);

    display_runic_lines();
    display_player_stats(*player, turns);

    mob_show(*player);
    for(item_t *it = item_get(); it; it = it->next) is_player_in_eyesight(it->pos, player->pos) ? item_show(*it) : item_hide(*it);

    draw();

    while(game_is_running){
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
            case 'Q':
                game_is_running = false;
                continue;
            case '.': /* rest */
                if(turns % 10 == 0 ){ /* NOTE: refactor */
                    player->health += 5u;
                    player->health -= (player->health*(player->health/100) % PLAYER_MAX_HEALTH); /* capping it to max unreaosonably complicatedly*/
                }
                break;
            case '?':
                display_to_player_window("hotkeys");
                continue;
            case 'E':
                display_to_player_window("events");
                continue;

            default:
                continue;
        }

        for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_update(mob, step_to);
        for(item_t *it = item_get(); it; it = it->next) is_player_in_eyesight(it->pos, player->pos) ? item_show(*it) : item_hide(*it);

        /*  temporarily teleporting the player due to the lack of corridors*/
        if(player->stands_on == ROOM_DOOR && custom_mode == false) {
            int num = CALC_RAND(room_get_num_of_rooms() -1, 0);
            int x = r[num].pos.x + 1;
            int y = r[num].pos.y + 1;
            mob_move_to(player, x, y);
            mob_show(*player);
            for(item_t *it = item_get(); it; it = it->next) is_player_in_eyesight(it->pos, player->pos) ? item_show(*it) : item_hide(*it);
            for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) is_player_in_eyesight(mob->pos, player->pos) ? mob_show(*mob) : mob_hide(*mob);
            event_log_add("You teleported to a random room!");
        }

        display_player_stats(*player, turns);
        display_recent_events();
        draw();

        ++turns;
    }

    /* CLEAN UP */
    if(!player->health && custom_mode == false) cutscene_dead();
    free(r);

    return 0;
}
