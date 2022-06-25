#ifndef _ROOM_
#define _ROOM_

#include <stdint.h>
#include "../include/terminal.h"

typedef struct room_t
{
    pos_t pos;
    int16_t width;
    int16_t height;
}room_t;

extern void obj_draw_room(room_t r);
extern room_t obj_calculate_room(void);

#define VERTICAL_WALL   '|'
#define HORIZONTAL_WALL '='
#define ROOM_FLOOR      '.'


#endif /* _ROOM_ */