#ifndef _CREATURE_
#define _CREATURE_

#include <stdint.h>
#include "terminal.h"
#include "object.h"
#include "item.h"
#include "input.h"
#include "utils.h"


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

extern void mob_move_by(mob_t *creature, int16_t x, int16_t y);
extern void mob_move_to(mob_t *creature, int16_t x, int16_t y);
extern void mob_free_mobs(void);
extern mob_t *mob_summon(const mob_id_t id);
extern mob_t *mob_get_mobs(void);
extern void mob_update(mob_t *mob);
extern void mob_handle_movement(mob_t *mob, input_code_t step_to);

#endif /* _CREATURE_ */