#include "room.h"
#include "terminal.h"
#include "corridor.h"

void corridor_create(door_t *d1, door_t *d2)
{
    int16_t x1 = d1->obj.pos.x;
    int16_t x2 = d2->obj.pos.x;
    int16_t y1 = d1->obj.pos.y;
    int16_t y2 = d2->obj.pos.y;
    float y = 0.0f;

    if(x1 == x2){ /* avoiding devision by zero */
        for(int y = (y2 > y1) ? y1 + 1 : y2 + 1; y < y2; ++y){ /* +1 not to start from the door*/
            term_move_cursor(x1,y);
            term_putchar(CORRIDOR_FLOOR);
        }
    }
    else{
        float m = (float)(y1 - y2) / (x1 - x2);
        float b = y1 - m*x1;
        for(int x = (x2 > x1) ? x1 : x2; x < x2; ++x){
            y = m*x + b + 0.5f; 
            term_move_cursor(x,y);
            term_putchar(CORRIDOR_FLOOR);
        }
    }
}