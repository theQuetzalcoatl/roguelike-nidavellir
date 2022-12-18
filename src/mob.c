#include <stdlib.h>
#include "terminal.h"
#include "mob.h"
#include "debug.h"
#include "room.h"

static void summon_player(mob_t *player);
static void summon_goblin(mob_t *goblin);
static void summon_draugr(mob_t *draugr);
static void add_to_list(mob_t *mob, const mob_id_t id);

static mob_t *head = NULL;
static mob_t *tail = NULL;


void mob_free_mobs(void)
{
    mob_t *mob = head->next;
    while(mob){
        free(head);
        head = mob;
        mob = mob->next;
    }
}


void mob_move_to(mob_t *creature, pos_t pos)
{
    term_move_cursor(creature->obj.pos.x, creature->obj.pos.y);
    term_putchar(creature->stands_on);
    term_move_cursor(pos.x, pos.y);
    creature->stands_on = term_getchar();
    term_putchar(creature->symbol);
    creature->obj.pos.x = pos.x;
    creature->obj.pos.y = pos.y;
}


void mob_move_by(mob_t *creature, pos_t rel_pos)
{
    term_move_cursor(creature->obj.pos.x, creature->obj.pos.y);
    term_putchar(creature->stands_on);
    term_move_cursor(creature->obj.pos.x + rel_pos.x, creature->obj.pos.y + rel_pos.y);
    creature->stands_on = term_getchar();
    term_putchar(creature->symbol);
    creature->obj.pos.x += rel_pos.x;
    creature->obj.pos.y += rel_pos.y;
}


mob_t *mob_get_mobs(void)
{
    return head;
}


mob_t *mob_summon(const mob_id_t id)
{
    mob_t *summoned_creature = malloc(sizeof(mob_t));
    if(summoned_creature == NULL){
        nidebug("Could not summon creature in %s:%d", __FILE__, __LINE__);
        exit(1);
    }

    switch(id)
    {
        case ID_PLAYER:
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
            summoned_creature = NULL;
    }

    if(summoned_creature != NULL) add_to_list(summoned_creature, id);
    else{
        /* This means either an invalid ID was supplied or could not summon the mob*/
    }

    return summoned_creature;
}


static void add_to_list(mob_t *mob, const mob_id_t id)
{
    if(id != ID_PLAYER && head != NULL){
        tail->next = mob;
        tail = mob;
        mob->next = NULL;
    }
    else if(id == ID_PLAYER && head == NULL){
        head = mob;
        tail = head;
    }
    else{
        nidebug("Something went wrong during list expansion. Maybe the order?\n");
        exit(1);
    }
}


static pos_t get_random_pos(void)
{
    const room_t *r;
    uint16_t x, y, selected_room, tries;

    for(tries = 10; tries; --tries){
        selected_room = CALC_RAND(room_get_num_of_rooms()-1, 1); // room[0] is where the player starts, it shall be safe originally, thats why it is [numofroom:1]
        r = room_get_rooms() + selected_room;

        x = CALC_RAND(r->width - 1, 1);
        x += r->obj.pos.x;

        y = CALC_RAND(r->height - 1, 1);
        y += r->obj.pos.y;

        if(term_getchar_xy(x, y) == ROOM_FLOOR) break;
    }

    if(tries){
        return (pos_t){.x=x, .y=y};
    }
    else{
        nidebug("Could not find a place to summon mob."); /* NOTE: handle it more elegantly */
        exit(1);
    }
}


            /* -------- MOB SUMMONING -------- */

static void summon_player(mob_t *player)
{
    static bool summoned = false;
    room_t *rooms = room_get_rooms();
    if(summoned == false){
        *player = (mob_t){.obj.pos.x=rooms[0].obj.pos.x+1, .obj.pos.y=rooms[0].obj.pos.y+1, .stands_on='.', .symbol='@', .health = 100, .level=1, .next = NULL};
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
    *goblin = (mob_t){.obj={get_random_pos()}, .stands_on='.', .symbol='G', .health=15, .level=1, .next = NULL};
}

static void summon_draugr(mob_t *draugr)
{
    *draugr = (mob_t){.obj={get_random_pos()}, .stands_on='.', .symbol='D', .health = 50, .level=10, .next = NULL}; 
}