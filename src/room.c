#include "../include/room.h"

#define MIN_ROOM_HEIGHT (4u)
#define MIN_ROOM_WIDTH MIN_ROOM_HEIGHT
#define MAX_ROOM_HEIGHT (15u)
#define MAX_ROOM_WIDTH MAX_ROOM_HEIGHT


room_t obj_calculate_room(void)
{
    room_t r = {.height = rand()%MAX_ROOM_HEIGHT + MIN_ROOM_HEIGHT, .width = rand()%MAX_ROOM_WIDTH + MIN_ROOM_WIDTH};
    r.pos.x = rand()%TERM_ROWS_NUM;
    r.pos.y = rand()%TERM_COLS_NUM;

    if(r.pos.x + r.width >= TERM_COLS_NUM) r.pos.x = TERM_COLS_NUM - r.width - 2;
    if(r.pos.y + r.height >= TERM_ROWS_NUM) r.pos.y = TERM_ROWS_NUM - r.height - 2;

    return r;
}


void obj_draw_room(room_t r)
{
    for(int x = r.pos.x; x < r.width + r.pos.x; ++x){
        term_move_cursor(x, r.pos.y);
        term_putchar(HORIZONTAL_WALL);
    }
    for(int y = r.pos.y + 1; y < r.height + r.pos.y - 1; ++y){
        for(int x = r.pos.x; x < r.width + r.pos.x; ++x){
            term_move_cursor(x, y);
            if(x != r.pos.x && x != r.pos.x + r.width - 1) term_putchar(ROOM_FLOOR); 
            else term_putchar(VERTICAL_WALL);
        }
    }
    for(int x = r.pos.x; x < r.width + r.pos.x; ++x){
        term_move_cursor(x, r.pos.y + r.height - 1);
        term_putchar(HORIZONTAL_WALL);
    }
}