#include "terminal.h"
#include "creature.h"
#include <stdlib.h>

static void summon_player(creature_t *player);
static void add_to_list(creature_t *mob, const creature_id_t id);


static creature_t *head = NULL;
static creature_t *tail = NULL;


void creature_move_abs(creature_t *creature, pos_t pos)
{
    term_move_cursor(creature->obj.pos.x, creature->obj.pos.y);
    term_putchar(creature->stands_on);
    term_move_cursor(pos.x, pos.y);
    creature->stands_on = term_getchar();
    term_putchar(creature->symbol);
    creature->obj.pos.x = pos.x;
    creature->obj.pos.y = pos.y;
}


void creature_move_rel(creature_t *creature, pos_t rel_pos)
{
    term_move_cursor(creature->obj.pos.x, creature->obj.pos.y);
    term_putchar(creature->stands_on);
    term_move_cursor(creature->obj.pos.x + rel_pos.x, creature->obj.pos.y + rel_pos.y);
    creature->stands_on = term_getchar();
    term_putchar(creature->symbol);
    creature->obj.pos.x += rel_pos.x;
    creature->obj.pos.y += rel_pos.y;
}

creature_t *creature_get_creatures(void)
{
    return head;
}


creature_t *creature_summon(const creature_id_t id)
{
    creature_t *summoned_creature = malloc(sizeof(creature_t));
    if(summoned_creature == NULL){
        printf("Could not summon creature in %s:%d", __FILE__, __LINE__);
        exit(1);
    }

    switch(id)
    {
        case ID_PLAYER:
            summon_player(summoned_creature);
            break;
        
        default:
            printf("Unknown creature id: %d", id);
            free(summoned_creature);
            summoned_creature = NULL;
    }

    /* */
    if(summoned_creature != NULL) add_to_list(summoned_creature, id);
    else{
        /* This means either an invalid ID was supplied or could not summon the mob*/
    }

    return summoned_creature;
}


static void add_to_list(creature_t *mob, const creature_id_t id)
{
    if(id != ID_PLAYER && head != NULL){
        tail->next = mob;
        tail = mob;
    }
    else if(id == ID_PLAYER && head == NULL){
        head = mob;
        tail = head;
    }
    else{
        printf("Something went wrong during list expansion. Maybe the orther?\n");
        exit(0);
    }
}



static void summon_player(creature_t *player)
{
    static bool summoned = false;
    if(summoned == false){
        *player = (creature_t){.obj.pos.x=0, .obj.pos.y=0, .stands_on='.', .symbol='@', .health = 100, .level=1, .next = NULL};
        summoned = true;
    }
    else{
        printf("Player could not be summoned after it was already so\n");
        free(player);
        player = NULL;
    }
}