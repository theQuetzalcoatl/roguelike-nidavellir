#ifndef _ROOM_
#define _ROOM_

#include <stdint.h>
#include "terminal.h"

#include <stdlib.h>

#define MAX_DOOR_NUM (4u)
#define MIN_DOOR_NUM (1u)

typedef struct
{
  point_t pos;
  bool is_locked;
}door_t;

typedef struct room_t
{
  point_t pos;
  door_t upper_door;
  door_t lower_door;
  door_t left_door;
  door_t right_door;
  int16_t width;  /* [chars] */
  int16_t height; /* [chars] */
  uint8_t in_cell;
}room_t;

typedef struct corridor_t
{
  struct
  {
    point_t start, end;
    bool is_vertical;
    int8_t direction;
  }line[3]; /* max 2 turns = 3 lines max */
}corridor_t;

extern uint8_t room_draw(const room_t r);
extern uint8_t room_get_num_of_rooms(void);
extern room_t *room_create_rooms(void);
extern room_t *room_get_rooms(void);
extern room_t *room_find(const point_t p);
extern uint8_t room_get_num_of_corridors(void);
extern corridor_t *room_get_corridors(void);
extern corridor_t *room_find_corridor_with_player(const point_t player);
extern void room_draw_corridor_piece(const corridor_t *c, const point_t player);

#define VERTICAL_WALL   '|'
#define HORIZONTAL_WALL '='
#define ROOM_FLOOR      '.'
#define ROOM_DOOR       '/'
#define EMPTY_SPACE     ' '

#define ALREADY_DRAWN (1u)

#endif /* _ROOM_ */