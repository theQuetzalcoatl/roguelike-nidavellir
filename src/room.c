#include "../include/room.h"
#include <stdlib.h>

#define MIN_ROOM_HEIGHT (4u)
#define MIN_ROOM_WIDTH MIN_ROOM_HEIGHT
#define MAX_ROOM_HEIGHT (15u)
#define MAX_ROOM_WIDTH MAX_ROOM_HEIGHT

static void add_doors(room_t * const room);


room_t room_create(void)
{
    room_t r = {0};
    r.height = CALC_RAND(MAX_ROOM_HEIGHT, MIN_ROOM_HEIGHT);
    r.obj.pos.x = CALC_RAND(TERM_COLS_NUM, 0);
    r.obj.pos.y = CALC_RAND(TERM_ROWS_NUM, 0);
    obj_make_invisible(&r);
    r.width = CALC_RAND(MAX_ROOM_WIDTH, MIN_ROOM_WIDTH);

    if(r.obj.pos.x + r.width >= (signed int)TERM_COLS_NUM) r.obj.pos.x = TERM_COLS_NUM - r.width - 10;
    if(r.obj.pos.y + r.height >= (signed int)TERM_ROWS_NUM) r.obj.pos.y = TERM_ROWS_NUM - r.height - 10;

    add_doors(&r);

    return r;
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
        obj_make_invisible(&room->doors[door_num]);
    }
}