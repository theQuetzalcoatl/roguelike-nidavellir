#include <stdlib.h>
#include "terminal.h"
#include "mob.h"
#include "debug.h"
#include "room.h"
#include "corridor.h"
#include "item.h"

static void summon_player(mob_t *player);
static void summon_goblin(mob_t *goblin);
static void summon_draugr(mob_t *draugr);
static void add_to_list(mob_t *mob);

static mob_t *head = NULL;
static mob_t *player = NULL;

extern bool game_is_running;


void mob_free_mobs(void)
{
    mob_t *mob = NULL;
    while(head){
        mob = head->next;
        free(head);
        head = mob;
    }
}

static void remove_mob(mob_t *mob)
{
    mob_t *prev_mob = head;
    mob_t *search_mob = head;
    term_putchar_xy(mob->stands_on, mob->obj.pos.x, mob->obj.pos.y);
    while(search_mob){
        if(search_mob == mob){
            if(search_mob == head) head = head->next; /* to be able to get all the remaining mobs after the first is removed */
            prev_mob->next = mob->next;
            free(mob);
            break;
        }
        else prev_mob = search_mob;
        search_mob = search_mob->next;
    }
}


void mob_move_to(mob_t *mob, int16_t x, int16_t y)
{
    term_putchar_xy(mob->stands_on, mob->obj.pos.x, mob->obj.pos.y);
    mob->stands_on = term_getchar_xy(x, y);
    mob->obj.pos.x = x;
    mob->obj.pos.y = y;
}

void mob_move_by(mob_t *mob, int16_t dx, int16_t dy)
{
    term_putchar_xy(mob->stands_on, mob->obj.pos.x, mob->obj.pos.y);
    mob->stands_on = term_getchar_xy(mob->obj.pos.x + dx, mob->obj.pos.y + dy);
    mob->obj.pos.x += dx;
    mob->obj.pos.y += dy;
}

void mob_show(mob_t mob)
{
    term_putchar_xy(mob.symbol, mob.obj.pos.x, mob.obj.pos.y);
}

void mob_hide(mob_t mob)
{
    term_putchar_xy(mob.stands_on, mob.obj.pos.x, mob.obj.pos.y);
}

mob_t *mob_get_mobs(void)
{
    return head;
}

mob_t *mob_get_player(void)
{
    return player;
}


static void attack(mob_t *attacked_mob)
{
    int8_t damage = CALC_RAND(20,0);
    attacked_mob->health -= damage;
    if(attacked_mob->health <= 0){
        attacked_mob->health = 0;
        if(attacked_mob == player) game_is_running = false;
        else remove_mob(attacked_mob);
    }
}


static void attack_player(void)
{
    attack(player);
}


void mob_handle_movement(mob_t *mob, input_code_t step_to)
{
    char obj_ahead = 0;
    int16_t new_x = 0;
    int16_t new_y = 0;

    switch(step_to)
    {
        case STEP_UP:
            obj_ahead = term_getchar_xy(mob->obj.pos.x, mob->obj.pos.y - 1); 
            new_x = mob->obj.pos.x;
            new_y = mob->obj.pos.y - 1;
            break;
        case STEP_LEFT:
            obj_ahead = term_getchar_xy(mob->obj.pos.x - 1, mob->obj.pos.y);
            new_x = mob->obj.pos.x - 1;
            new_y = mob->obj.pos.y;
            break;
        case STEP_DOWN:
            obj_ahead = term_getchar_xy(mob->obj.pos.x, mob->obj.pos.y + 1);
            new_x = mob->obj.pos.x;
            new_y = mob->obj.pos.y + 1;
            break;
        case STEP_RIGHT:
            obj_ahead = term_getchar_xy(mob->obj.pos.x + 1, mob->obj.pos.y);
            new_x = mob->obj.pos.x + 1;
            new_y = mob->obj.pos.y;
            break;
        case NO_ARROW:
            return; 
        default:
            nidebug("invalid option for stepping! %s:%d", __FILE__, __LINE__);
    }

    switch(obj_ahead)
    {
        case ROOM_FLOOR:
        case ROOM_DOOR:
        case CORRIDOR_FLOOR:
            mob_move_to(mob, new_x, new_y);
            break;
        case VERTICAL_WALL:
        case HORIZONTAL_WALL:
        case EMPTY_SPACE:
        /* do nothing */
            break;

        case ID_DRAUGR:
        case ID_GOBLIN:
            if(mob == player){
                for(mob_t *hostile_mob = mob_get_mobs(); hostile_mob; hostile_mob = hostile_mob->next){
                    if(hostile_mob->obj.pos.x == new_x && hostile_mob->obj.pos.y == new_y){
                        nidebug("[%c] health: %i", hostile_mob->symbol, hostile_mob->health);
                        attack(hostile_mob);
                        break;
                    }
                }
            }
            break;

        case ITEM_SYMBOL:
            if(mob == player){
                for(item_t* it = item_get(); it; it = it->next){
                    if(it->obj.pos.x == new_x && it->obj.pos.y == new_y){
                        ((potion_t*)it->spec_attr)->use(it);
                        mob_move_to(player, new_x, new_y);
                        player->stands_on = ROOM_FLOOR;
                        break;
                    }
                }
            }
            break;

        default:
            nidebug("Unknown object ahead:[%c] in %s:%d", obj_ahead, __FILE__, __LINE__);
    }
}


mob_t *mob_summon(const mob_id_t id)
{
    static bool first_summon = true;

    mob_t *summoned_creature = malloc(sizeof(mob_t));
    if(summoned_creature == NULL){
        nidebug("Could not summon creature in %s:%d", __FILE__, __LINE__);
        exit(1);
    }

    switch(id)
    {
        case ID_PLAYER:
            player = summoned_creature;
            summon_player(summoned_creature);
            break;

        case ID_GOBLIN:
            summon_goblin(summoned_creature);
            break;

        case ID_DRAUGR:
            summon_draugr(summoned_creature);
            break;
        
        default:
            nidebug("Unknown creature id: %d", id);
            free(summoned_creature);
            return NULL;
    }

    if(summoned_creature != NULL && first_summon == false) add_to_list(summoned_creature);
    else{
        first_summon = false;
        head = summoned_creature;
        head->next = NULL;
    }

    return summoned_creature;
}


static void add_to_list(mob_t *mob)
{
    mob_t *mobs = mob_get_mobs();
    for(; mobs->next; mobs = mobs->next);
    mobs->next = mob;
    mob->next = NULL;
}


void mob_update(mob_t *mob, input_code_t step_to)
{
    if(mob == player){
        mob_handle_movement(mob, step_to);
        mob_show(*mob);
    }
    else{
        int16_t dx = mob->obj.pos.x - player->obj.pos.x;
        int16_t dy = mob->obj.pos.y - player->obj.pos.y;

        if( (1*1 + 1*1) < (dx*dx + dy*dy) ){  // sanity check, if mob is within 1 unit radius of player it is definitely in sight.
            if(is_player_in_eyesight(mob->obj.pos, player->obj.pos)){
                if(abs(dx) > abs(dy)) (dx > 0) ? mob_handle_movement(mob, STEP_LEFT) : mob_handle_movement(mob, STEP_RIGHT);
                else (dy > 0) ? mob_handle_movement(mob, STEP_UP) : mob_handle_movement(mob, STEP_DOWN);
                mob_show(*mob);
            }
            else mob_hide(*mob);
        }
        else{
            mob_show(*mob);
            attack_player();
        }
    }
}


static pos_t get_random_pos(void)
{
    const room_t *room;
    uint16_t x, y, selected_room, tries;
    if(room_get_rooms() == NULL || room_get_num_of_rooms() == 0) return (pos_t){.x=0, .y=0};
    else{
        for(tries = 10; tries; --tries){
            selected_room = CALC_RAND(room_get_num_of_rooms()-1, 1); // room[0] is where the player starts, it shall be safe originally, thats why it is [numofroom:1]
            room = room_get_rooms() + selected_room;

            x = room->obj.pos.x;
            x += CALC_RAND(room->width - 1, 1);

            y = room->obj.pos.y;
            y += CALC_RAND(room->height - 1, 1);

            if(term_getchar_xy(x, y) == ROOM_FLOOR) break;
        }

        if(tries) return (pos_t){.x=x, .y=y};
        else{
            nidebug("Could not find a place to summon mob."); /* NOTE: handle it more elegantly */
            exit(1);
        }    
    }
}

            /* -------- MOB SUMMONING -------- */

static void summon_player(mob_t *player)
{
    static bool summoned = false;
    room_t *rooms = room_get_rooms();
    if(summoned == false){
        if(room_get_rooms() != NULL)
            *player = (mob_t){.obj.pos.x=rooms[0].obj.pos.x+1, .obj.pos.y=rooms[0].obj.pos.y+1, .stands_on=ROOM_FLOOR, .symbol=ID_PLAYER, .health = 100, .level=1, .next = NULL};
        else
            *player = (mob_t){.obj.pos.x=1, .obj.pos.y=1, .stands_on=ROOM_FLOOR, .symbol=ID_PLAYER, .health = 100, .level=1, .next = NULL};
        summoned = true;
    }
    else{
        nidebug("Player could not be summoned after it was already so\n");
        free(player);
        player = NULL;
    }
}

static void summon_goblin(mob_t *goblin)
{
    *goblin = (mob_t){.obj={get_random_pos()}, .stands_on=ROOM_FLOOR, .symbol=ID_GOBLIN, .health=15, .level=1, .next = NULL};
}

static void summon_draugr(mob_t *draugr)
{
    *draugr = (mob_t){.obj={get_random_pos()}, .stands_on=ROOM_FLOOR, .symbol=ID_DRAUGR, .health = 20, .level=10, .next = NULL}; 
}
