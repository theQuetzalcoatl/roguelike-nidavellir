#ifndef _ROOM_
#define _ROOM_

#include <stdint.h>
#include "terminal.h"
#include "object.h"
#include <stdlib.h>

#define MAX_DOOR_NUM (4u)
#define MIN_DOOR_NUM (1u)

typedef enum
{
    up_side,
    down_side,
    left_side,
    right_side
}side_e;

typedef struct
{
    object_t obj;
    side_e side;
    bool is_locked;
}door_t;

typedef struct room_t
{
    object_t obj;
    door_t doors[MAX_DOOR_NUM];
    int16_t width;  /* [chars] */
    int16_t height; /* [chars] */
    uint8_t door_num;
}room_t;

extern void room_draw(const room_t r);
extern uint8_t room_get_num_of_rooms(void);
extern room_t *room_create_rooms(void);
extern room_t *room_get_rooms(void);

#define VERTICAL_WALL   '|'
#define HORIZONTAL_WALL '='
#define ROOM_FLOOR      '.'
#define ROOM_DOOR       '/'
#define EMPTY_SPACE     ' '

#endif /* _ROOM_ */