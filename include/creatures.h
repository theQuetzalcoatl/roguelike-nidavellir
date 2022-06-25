#ifndef _CREATUES_
#define _CREATUES_

#include <stdint.h>
#include "terminal.h"

typedef struct creature_t
{
    pos_t pos;
    char symbol;
    uint8_t stands_on;    
}creature_t;


extern void creature_move_rel(creature_t *creature, pos_t rel_pos);
extern void creature_move_abs(creature_t *creature, pos_t pos);

#endif /* _CREATUES_ */