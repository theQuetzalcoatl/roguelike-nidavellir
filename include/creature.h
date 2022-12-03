#ifndef _CREATURE_
#define _CREATURE_

#include <stdint.h>
#include "terminal.h"
#include "object.h"
#include "item.h"


enum CREATURES
{
    ID_PLAYER,
    ID_THOR,
    ID_ODIN,
    ID_GOBLIN,
    ID_TROLL,
    ID_WITCH,
    ID_MIMIR,
    NUM_OF_CREATURES
};

typedef uint16_t creature_id_t;

typedef struct creature_t
{
    object_t obj;
    char symbol;
    uint8_t stands_on;
    uint8_t health;
    uint8_t level;
    item_t left_hand_item;
    item_t right_hand_item;
    struct creature_t *next;
}creature_t;

#define PLAYER_MAX_HEALTH (100u)

extern void creature_move_rel(creature_t *creature, pos_t rel_pos);
extern void creature_move_abs(creature_t *creature, pos_t pos);
extern creature_t *creature_summon(const creature_id_t id);
extern creature_t *creature_get_creatures(void);

#endif /* _CREATURE_ */