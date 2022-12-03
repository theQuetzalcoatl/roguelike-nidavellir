#ifndef _ITEM_H
#define _ITEM_H

#include "object.h"
#include <stdint.h>

#define ITEM_SYMBOL '*'

enum ITEMS
{
    /* ARMORS */
    wooden_shield,
    leather_gauntlet,
    /* WEAPONS */
    /* POTIONS */
    /* MAGIC STUFF */
    /* FOOD & DRINK*/
    mead,
    NUM_OF_ITEMS
};


typedef struct item_t
{
    object_t obj;
    char symbol;
    uint8_t stand_on;
}item_t;


typedef struct armor
{
    item_t item;
    uint8_t type;
    /* stats */
    uint8_t durability;
    uint8_t shielding;
}armor_t;

#endif /* _ITEM_H */