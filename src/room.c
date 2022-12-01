#include "room.h"
#include <stdlib.h>
#include "display.h"

#define MIN_ROOM_HEIGHT (4u)
#define MIN_ROOM_WIDTH MIN_ROOM_HEIGHT
#define MAX_ROOM_HEIGHT (15u)
#define MAX_ROOM_WIDTH MAX_ROOM_HEIGHT

#define MAX_NUM_OF_ROOMS_PER_LEVEL (5u)
#define MIN_NUM_OF_ROOMS_PER_LEVEL (3u)

static void add_doors(room_t * const room);
static room_t calculate_one_room(void);
static void draw_debug_info(room_t room);

static uint8_t num_of_rooms = 0;


uint8_t room_get_num_of_rooms(void)
{
    return num_of_rooms;
}


room_t *room_create_rooms(void)
{
    uint8_t number_of_rooms = CALC_RAND(MAX_NUM_OF_ROOMS_PER_LEVEL, MIN_NUM_OF_ROOMS_PER_LEVEL);
    room_t *room = malloc(number_of_rooms*sizeof(room_t));

    if(room == NULL){ /* NOTE: should we decrease the room number until MIN in order to try avoiding a crash? */
        printf("Could not allocate enough memory for rooms in %s:%i.\n", __FILE__, __LINE__);
        exit(1);
    }

    for(int i = 0; i < number_of_rooms; ++i) *(room + i) = calculate_one_room();

    num_of_rooms += number_of_rooms; 
    return room;
}


static room_t calculate_one_room(void)
{
    room_t r = {0};
    r.height = CALC_RAND(MAX_ROOM_HEIGHT, MIN_ROOM_HEIGHT);
    r.obj.pos.x = CALC_RAND(TERM_COLS_NUM, 0);
    r.obj.pos.y = CALC_RAND(TERM_ROWS_NUM, 0);
    obj_make_invisible((object_t*)&r);
    r.width = CALC_RAND(MAX_ROOM_WIDTH, MIN_ROOM_WIDTH);

    if(r.obj.pos.x + r.width >= (signed int)TERM_COLS_NUM) r.obj.pos.x = TERM_COLS_NUM - r.width - 10;
    if(r.obj.pos.y + r.height >= (signed int)RUNIC_LINE) r.obj.pos.y = RUNIC_LINE - r.height - 10;

    add_doors(&r);

    return r;
}


static void add_doors(room_t * const room)
{
    bool upper_wall_doored, lower_wall_doored, left_wall_doored, right_wall_doored;
    upper_wall_doored = lower_wall_doored = left_wall_doored = right_wall_doored = false;
    room->door_num = CALC_RAND(MAX_DOOR_NUM, MIN_DOOR_NUM);

    for(uint8_t door_num = 0; door_num < room->door_num; ++door_num){
        uint8_t to_be_doored_wall = rand()%4; // a room has 4 sides
        switch(to_be_doored_wall)
        {
            case up_side:
                if(!upper_wall_doored){
                    room->doors[door_num].obj.pos.y = room->obj.pos.y;
                    room->doors[door_num].obj.pos.x = room->obj.pos.x + CALC_RAND(room->width - 2, 1); // 1 - to avoid x position corner, 2 - to avoid x+width corner also taking into account that drawing starts at x;
                    room->doors[door_num].side = up_side;
                    upper_wall_doored = true;
                    break;
                }
                __attribute__ ((fallthrough));

            case down_side:
                if(!lower_wall_doored){
                    room->doors[door_num].obj.pos.x = room->obj.pos.x + CALC_RAND(room->width - 2, 1); // 1 - to avoid x position corner, 2 - to avoid x+width corner also taking into account that drawing starts at x;
                    room->doors[door_num].obj.pos.y = room->obj.pos.y + room->height - 1;
                    room->doors[door_num].side = down_side;
                    lower_wall_doored = true;
                    break;
                }
                __attribute__ ((fallthrough));

            case left_side:
                if(!left_wall_doored){
                    room->doors[door_num].obj.pos.y = room->obj.pos.y + CALC_RAND(room->height - 2, 1); // 1 - to avoid y position corner, 2 - to avoid y+height corner also taking into account that drawing starts at y;
                    room->doors[door_num].obj.pos.x = room->obj.pos.x;
                    room->doors[door_num].side = left_side;
                    left_wall_doored = true;
                    break;
                }
                __attribute__ ((fallthrough));

            case right_side:
                if(!right_wall_doored){
                    room->doors[door_num].obj.pos.y = room->obj.pos.y + CALC_RAND(room->height - 2, 1); // 1 - to avoid y position corner, 2 - to avoid y+height corner also taking into account that drawing starts at y;
                    room->doors[door_num].obj.pos.x = room->obj.pos.x + room->width - 1;
                    room->doors[door_num].side = right_side;
                    right_wall_doored = true;
                    break;
                }
        }
        room->doors[door_num].is_locked = false;
        obj_make_invisible((object_t*)&room->doors[door_num]);
    }
}


void room_draw(const room_t r)
{
    /* upper wall */
    for(int x = r.obj.pos.x; x < r.width + r.obj.pos.x; ++x){
        term_move_cursor(x, r.obj.pos.y);
        term_putchar(HORIZONTAL_WALL);
    }
    /* sidewalls and floor */
    for(int y = r.obj.pos.y + 1; y < r.height + r.obj.pos.y - 1; ++y){
        for(int x = r.obj.pos.x; x < r.width + r.obj.pos.x; ++x){
            term_move_cursor(x, y);
            if(x != r.obj.pos.x && x != r.obj.pos.x + r.width - 1) term_putchar(ROOM_FLOOR); 
            else term_putchar(VERTICAL_WALL);
        }
    }
    /* lower wall */
    for(int x = r.obj.pos.x; x < r.width + r.obj.pos.x; ++x){
        term_move_cursor(x, r.obj.pos.y + r.height - 1);
        term_putchar(HORIZONTAL_WALL);
    }
    /* placing doors */
    for(uint8_t door_num = 0; door_num < r.door_num; ++door_num){
        term_move_cursor(r.doors[door_num].obj.pos.x, r.doors[door_num].obj.pos.y);
        term_putchar(ROOM_DOOR);
    }

    draw_debug_info(r);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

void draw_debug_info(room_t room)
{
#ifdef DEBUG
    uint16_t above_room = (room.obj.pos.y) > 0 ? room.obj.pos.y - 1 : 0;
    term_move_cursor(room.obj.pos.x, above_room);
    printf("x:%d y:%d w:%d h:%d\n", room.obj.pos.x, room.obj.pos.y, room.width, room.height);
#endif
}