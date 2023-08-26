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
static void move_towards_player_last_seen_pos(mob_t *mob);

static mob_t *head = NULL;
static mob_t *player = NULL;

extern bool game_is_running;
extern void event_log_add(const char *event);


#define UPPER_EDGE_OF_MAP (-1)
#define LEFT_EDGE_OF_MAP  (-1)


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
    term_putchar_xy(mob->stands_on, mob->pos.x, mob->pos.y);
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
    if(x > LEFT_EDGE_OF_MAP && y > UPPER_EDGE_OF_MAP){
        term_putchar_xy(mob->stands_on, mob->pos.x, mob->pos.y);
        mob->stands_on = term_getchar_xy(x, y);
        mob->pos.x = x;
        mob->pos.y = y;
    }
}

void mob_move_by(mob_t *mob, int16_t dx, int16_t dy)
{
    if((dx + mob->pos.x) > LEFT_EDGE_OF_MAP && (dy + mob->pos.y) > UPPER_EDGE_OF_MAP){
        term_putchar_xy(mob->stands_on, mob->pos.x, mob->pos.y);
        mob->stands_on = term_getchar_xy(mob->pos.x + dx, mob->pos.y + dy);
        mob->pos.x += dx;
        mob->pos.y += dy;
    }
}

void mob_show(mob_t mob)
{
    term_putchar_xy(mob.symbol, mob.pos.x, mob.pos.y);
}

void mob_hide(mob_t mob)
{
    term_putchar_xy(mob.stands_on, mob.pos.x, mob.pos.y);
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
    int16_t dx = 0;
    int16_t dy = 0;

    switch(step_to)
    {
        case STEP_UP:
            obj_ahead = term_getchar_xy(mob->pos.x, mob->pos.y - 1); 
            dx = 0;
            dy = -1;
            break;
        case STEP_LEFT:
            obj_ahead = term_getchar_xy(mob->pos.x - 1, mob->pos.y);
            dx = -1;
            dy = 0;
            break;
        case STEP_DOWN:
            obj_ahead = term_getchar_xy(mob->pos.x, mob->pos.y + 1);
            dx = 0;
            dy = 1;
            break;
        case STEP_RIGHT:
            obj_ahead = term_getchar_xy(mob->pos.x + 1, mob->pos.y);
            dx = 1;
            dy = 0;
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
            mob_move_by(mob, dx, dy);
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
                    if(hostile_mob->pos.x == (dx + player->pos.x) && hostile_mob->pos.y == (dy + player->pos.y)){
                        nidebug("[%c] health: %i", hostile_mob->symbol, hostile_mob->health);
                        attack(hostile_mob);
                        event_log_add("You chopped a piece out of *the mob*");
                        break;
                    }
                }
            }
            break;

        case ITEM_SYMBOL:
            if(mob == player){
                for(item_t* it = item_get(); it; it = it->next){
                    if(it->pos.x == (dx + player->pos.x) && it->pos.y == ((dy + player->pos.y))){
                        ((potion_t*)it->spec_attr)->use(it);
                        mob_move_by(player, dx, dy);
                        player->stands_on = ROOM_FLOOR;
                        event_log_add("You picked up an unknown blue potion");
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
        int16_t dx = mob->pos.x - player->pos.x;
        int16_t dy = mob->pos.y - player->pos.y;

        if( (1*1 + 1*1) < (dx*dx + dy*dy) ){  /* Is mob near the player? */
            if(is_player_in_eyesight(mob->pos, player->pos)){
                if(abs(dx) > abs(dy)) (dx > 0) ? mob_handle_movement(mob, STEP_LEFT) : mob_handle_movement(mob, STEP_RIGHT);
                else (dy > 0) ? mob_handle_movement(mob, STEP_UP) : mob_handle_movement(mob, STEP_DOWN);
                mob_show(*mob);
                mob->last_seen = player->pos;
            }
            else{
                move_towards_player_last_seen_pos(mob);
                mob_hide(*mob);
            }
        }
        else{
            mob_show(*mob);
            attack_player();
            event_log_add("*the mob* cut you badly!");
        }
    }
}


static void move_towards_player_last_seen_pos(mob_t *mob)
{
    if(mob->last_seen.x != 0 && mob->last_seen.y != 0){ /* the player was spotted at least once */
        int16_t dx = mob->pos.x - mob->last_seen.x;
        int16_t dy = mob->pos.y - mob->last_seen.y;

        if(dx || dy){
            if(abs(dx) > abs(dy)) (dx > 0) ? mob_handle_movement(mob, STEP_LEFT) : mob_handle_movement(mob, STEP_RIGHT);
            else (dy > 0) ? mob_handle_movement(mob, STEP_UP) : mob_handle_movement(mob, STEP_DOWN);
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

            x = room->pos.x;
            x += CALC_RAND(room->width - 1, 1);

            y = room->pos.y;
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
            *player = (mob_t){.pos.x=rooms[0].pos.x+1, .pos.y=rooms[0].pos.y+1, .stands_on=ROOM_FLOOR, .symbol=ID_PLAYER, .health = 100, .level=1, .next = NULL};
        else
            *player = (mob_t){.pos.x=1, .pos.y=1, .stands_on=ROOM_FLOOR, .symbol=ID_PLAYER, .health = 100, .level=1, .next = NULL, .last_seen = (pos_t){.x=0, .y=0} };
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
    *goblin = (mob_t){.pos=get_random_pos(), .stands_on=ROOM_FLOOR, .symbol=ID_GOBLIN, .health=15, .level=1, .next = NULL, .last_seen = (pos_t){.x=0, .y=0}};
}

static void summon_draugr(mob_t *draugr)
{
    *draugr = (mob_t){.pos=get_random_pos(), .stands_on=ROOM_FLOOR, .symbol=ID_DRAUGR, .health = 20, .level=10, .next = NULL, .last_seen = (pos_t){.x=0, .y=0}}; 
}
