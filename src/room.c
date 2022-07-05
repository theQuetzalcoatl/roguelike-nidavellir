#include "../include/room.h"
#include <stdlib.h>

#define MIN_ROOM_HEIGHT (4u)
#define MIN_ROOM_WIDTH MIN_ROOM_HEIGHT
#define MAX_ROOM_HEIGHT (15u)
#define MAX_ROOM_WIDTH MAX_ROOM_HEIGHT

#define CALC_RAND(max, min) (rand()%(max - min + 1) + min) 

/****************************
 ** LOCAL FUNCTION DECS 
 ****************************/

static void generate_doors(room_t *room);


room_t room_create(void)
{
    room_t r = {.height = CALC_RAND(MAX_ROOM_HEIGHT, MIN_ROOM_HEIGHT)};
    r.obj.pos.x = CALC_RAND(TERM_COLS_NUM, 0);
    r.obj.pos.y = CALC_RAND(TERM_ROWS_NUM, 0);
    r.obj.is_visible = false;
    r.width = CALC_RAND(MAX_ROOM_WIDTH, MIN_ROOM_WIDTH);

    if(r.obj.pos.x + r.width >= TERM_COLS_NUM) r.obj.pos.x = TERM_COLS_NUM - r.width - 10;
    if(r.obj.pos.y + r.height >= TERM_ROWS_NUM) r.obj.pos.y = TERM_ROWS_NUM - r.height - 10;

    generate_doors(&r);

    return r;
}


void room_draw(room_t r)
{
    /* drawing upper wall */
    for(int x = r.obj.pos.x; x < r.width + r.obj.pos.x; ++x){
        term_move_cursor(x, r.obj.pos.y);
        term_putchar(HORIZONTAL_WALL);
    }
    /* drawing sidewalls and floor */
    for(int y = r.obj.pos.y + 1; y < r.height + r.obj.pos.y - 1; ++y){
        for(int x = r.obj.pos.x; x < r.width + r.obj.pos.x; ++x){
            term_move_cursor(x, y);
            if(x != r.obj.pos.x && x != r.obj.pos.x + r.width - 1) term_putchar(ROOM_FLOOR); 
            else term_putchar(VERTICAL_WALL);
        }
    }
    /* drawing lower wall */
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


/******************************
 ** LOCAL FUNCTION DEFS
 ******************************/

static void generate_doors(room_t *room)
{
    uint8_t upper_wall = 0;
    uint8_t lower_wall = 0;
    uint8_t left_wall  = 0;
    uint8_t right_wall = 0;
    uint8_t door_num =  CALC_RAND(MAX_DOOR_NUM, MIN_DOOR_NUM);
    room->door_num = door_num;

    for(door_num = 0; door_num < room->door_num;){
        uint8_t doored_wall = rand()%4;
        switch(doored_wall)
        {
CASE_0:
            case 0:
                if(!upper_wall){
                    room->doors[door_num].obj.pos.y = room->obj.pos.y;
                    room->doors[door_num].obj.pos.x = rand()%(room->width-2) + 1 + room->obj.pos.x;
                    room->doors[door_num].is_closed = false;
                    room->doors[door_num].obj.is_visible = false;

                    upper_wall = 1;
                    ++door_num;
                }
                else goto CASE_1;
                break;
CASE_1:
            case 1:
                if(!lower_wall){
                    room->doors[door_num].obj.pos.y = room->obj.pos.y + room->height - 1;
                    room->doors[door_num].obj.pos.x = rand()%(room->width-2) + 1 + room->obj.pos.x;
                    room->doors[door_num].is_closed = false;
                    room->doors[door_num].obj.is_visible = false;

                    lower_wall = 1;
                    ++door_num;
                }
                else goto CASE_2;
                break;
CASE_2:
            case 2:
                if(!left_wall){
                    room->doors[door_num].obj.pos.x = room->obj.pos.x;
                    room->doors[door_num].obj.pos.y = rand()%(room->height-2) + 1 + room->obj.pos.y;
                    room->doors[door_num].is_closed = false;
                    room->doors[door_num].obj.is_visible = false;

                    left_wall = 1;
                    ++door_num;
                }
                else goto CASE_3;
                break;
CASE_3:
            case 3:
                if(!right_wall){
                    room->doors[door_num].obj.pos.x = room->obj.pos.x + room->width - 1;
                    room->doors[door_num].obj.pos.y = rand()%(room->height-2) + 1 + room->obj.pos.y;
                    room->doors[door_num].is_closed = false;
                    room->doors[door_num].obj.is_visible = false;

                    right_wall = 1;
                    ++door_num;
                }
                else goto CASE_0;
                break;
        }
    }
}