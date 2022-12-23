#ifndef _CREATURE_
#define _CREATURE_

#include <stdint.h>
#include "terminal.h"
#include "object.h"
#include "item.h"


enum MOBS
{
    ID_PLAYER,
    ID_THOR,
    ID_ODIN,
    ID_GOBLIN,
    ID_DRAUGR,
    ID_TROLL,
    ID_WITCH,
    ID_MIMIR,
    NUM_OF_MOBS
};

typedef uint16_t mob_id_t;

typedef struct mob_t
{
    object_t obj;
    char symbol;
    uint8_t stands_on;
    uint8_t health;
    uint8_t level;
    struct mob_t *next;
}mob_t;

#define PLAYER_MAX_HEALTH (100u)

extern void mob_move_by(mob_t *creature, pos_t rel_pos);
extern void mob_move_to(mob_t *creature, pos_t pos);
extern void mob_free_mobs(void);
extern mob_t *mob_summon(const mob_id_t id);
extern mob_t *mob_get_mobs(void);
extern void mob_update(mob_t *mob, mob_t *player);

#endif /* _CREATURE_ */