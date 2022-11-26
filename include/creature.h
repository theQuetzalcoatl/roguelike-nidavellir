#ifndef _CREATURE_
#define _CREATURE_

#include <stdint.h>
#include "terminal.h"
#include "object.h"

typedef struct creature_t
{
    object_t obj;
    char symbol;
    uint8_t stands_on;    
}creature_t;


extern void creature_move_rel(creature_t *creature, pos_t rel_pos);
extern void creature_move_abs(creature_t *creature, pos_t pos);

#endif /* _CREATURE_ */