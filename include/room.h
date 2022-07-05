#ifndef _ROOM_
#define _ROOM_

#include <stdint.h>
#include "../include/terminal.h"
#include "../include/object.h"

#define MAX_DOOR_NUM (4u)
#define MIN_DOOR_NUM (1u)


typedef struct room_t
{
    object_t obj;
    int16_t width;  /* [chars] */
    int16_t height; /* [chars] */
    struct
    {
        object_t obj;
        bool is_closed;
    }doors[MAX_DOOR_NUM];
    uint8_t door_num;
}room_t;

extern void room_draw(room_t r);
extern room_t room_create(void);

#define VERTICAL_WALL   '|'
#define HORIZONTAL_WALL '='
#define ROOM_FLOOR      '.'
#define ROOM_DOOR       '/'


#endif /* _ROOM_ */