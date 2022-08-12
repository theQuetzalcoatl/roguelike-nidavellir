#include "../include/room.h"
#include "../include/terminal.h"

#define CORRIDOR_FLOOR '#'

typedef enum
{
    upper_right,
    upper_left,
    lower_left,
    lower_right
}quarter_e;

#define HORIZONTAL  (0U)
#define VERTICAL    (1U)

void corridor_create(door_t *d1, door_t *d2)
{
   /* term_move_cursor(d1->obj.pos.x, d1->obj.pos.y);
    term_putchar(ROOM_DOOR);

    term_move_cursor(d2->obj.pos.x, d2->obj.pos.y);
    term_putchar(ROOM_DOOR);*/
    int16_t x = d1->obj.pos.x - d2->obj.pos.x; 
    int16_t y = d1->obj.pos.y - d2->obj.pos.y;
    quarter_e quarter = upper_right;

    /* where is p2 compared to p1 */
    if(x <= 0 && y >= 0) quarter = upper_right;
    else if(x >= 0 && y >= 0) quarter = upper_left;
    else if(x >= 0 && y <= 0) quarter = lower_left;
    else if(x <= 0 && y <= 0) quarter = lower_right;

    pos_t arrival_pos = {0};

    if(d2->side == down_side) arrival_pos = (pos_t){.x = d2->obj.pos.x, .y = d2->obj.pos.y + 1};
    else if(d2->side == up_side) arrival_pos = (pos_t){.x = d2->obj.pos.x, .y = d2->obj.pos.y - 1};
    else if(d2->side == left_side) arrival_pos = (pos_t){.x = d2->obj.pos.x - 1, .y = d2->obj.pos.y};
    else if(d2->side == right_side) arrival_pos = (pos_t){.x = d2->obj.pos.x + 1, .y = d2->obj.pos.y};

    /* drawing the corridors  */
    if(d1->side == up_side){
        pos_t loc_pos = {.x = d1->obj.pos.x, .y = d1->obj.pos.y - 1};
        uint8_t direction = VERTICAL;
        while((loc_pos.x != arrival_pos.x) || (loc_pos.y != arrival_pos.y)){

            int r = CALC_RAND(7, 2);
            if(direction == VERTICAL){
                if((loc_pos.y - r - 1) < d2->obj.pos.y){
                    if(d2->side == left_side || d2->side == right_side) r = loc_pos.y - d2->obj.pos.y;
                    else if(d2->side == down_side) r = loc_pos.y - d2->obj.pos.y - 1;
                }
                for(; r > 0; --r, --loc_pos.y){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = HORIZONTAL;
            }

            else if(direction == HORIZONTAL && quarter == upper_left){
                if((loc_pos.x - r - 1) < d2->obj.pos.x){
                    if(d2->side == right_side)     r = loc_pos.x - d2->obj.pos.x - 1;
                    else if(d2->side == down_side) r = loc_pos.x - d2->obj.pos.x;
                }
                for(; r > 0; --r, --loc_pos.x){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = VERTICAL;
            }
            else if(direction == HORIZONTAL && quarter == upper_right){
                if((loc_pos.x + r + 1) > d2->obj.pos.x){
                    if(d2->side == left_side)      r = d2->obj.pos.x - loc_pos.x - 1;
                    else if(d2->side == down_side) r = d2->obj.pos.x - loc_pos.x;
                }
                for(; r > 0; --r, ++loc_pos.x){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = VERTICAL;
            }
        }
        term_move_cursor(loc_pos.x, loc_pos.y);
        term_putchar(CORRIDOR_FLOOR); 
    }

    else if(d1->side == down_side){
        pos_t loc_pos = {.x = d1->obj.pos.x, .y = d1->obj.pos.y + 1};
        uint8_t direction = VERTICAL;
        while((loc_pos.x != arrival_pos.x) || (loc_pos.y != arrival_pos.y)){

            int r = CALC_RAND(7, 5);
            if(direction == VERTICAL){
                if((loc_pos.y + r + 1) > d2->obj.pos.y){
                    if(d2->side == up_side) r = d2->obj.pos.y - loc_pos.y - 1;
                    else if(d2->side == left_side || d2->side == right_side) r = d2->obj.pos.y - loc_pos.y;
                }
                for(; r > 0; --r, ++loc_pos.y){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = HORIZONTAL;
            }

            else if(direction == HORIZONTAL && quarter == lower_left){
                if((loc_pos.x - r - 1) < d2->obj.pos.x){
                    if(d2->side == right_side)   r = loc_pos.x - d2->obj.pos.x - 1;
                    else if(d2->side == up_side) r = loc_pos.x - d2->obj.pos.x;
                }
                for(; r > 0; --r, --loc_pos.x){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = VERTICAL;
            }
            else if(direction == HORIZONTAL && quarter == lower_right){
                if((loc_pos.x + r + 1) > d2->obj.pos.x){
                   if(d2->side == left_side)    r = d2->obj.pos.x - loc_pos.x - 1;
                   else if(d2->side == up_side) r = d2->obj.pos.x - loc_pos.x;
                }
                for(; r > 0; --r, ++loc_pos.x){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = VERTICAL;
            }
        }
        term_move_cursor(loc_pos.x, loc_pos.y);
        term_putchar(CORRIDOR_FLOOR); 
    }

    else if(d1->side == left_side){
        pos_t loc_pos = {.x = d1->obj.pos.x - 1, .y = d1->obj.pos.y};
        uint8_t direction = HORIZONTAL;
        while((loc_pos.x != arrival_pos.x) || (loc_pos.y != arrival_pos.y)){

            int r = CALC_RAND(7, 5);
            if(direction == HORIZONTAL){
                if((loc_pos.x - r - 1) < d2->obj.pos.x){
                    if(d2->side == right_side) r = loc_pos.x - d2->obj.pos.x - 1;
                    else if(d2->side == down_side || d2->side == up_side) r = loc_pos.x - d2->obj.pos.x;
                }
                for(; r > 0; --r, --loc_pos.x){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = VERTICAL;
            }
            else if(direction == VERTICAL && quarter == upper_left){
                if((loc_pos.y - r - 1) < d2->obj.pos.y){
                   if(d2->side == right_side)     r = loc_pos.y - d2->obj.pos.y;
                   else if(d2->side == down_side) r = loc_pos.y - d2->obj.pos.y - 1;
                }
                for(; r > 0; --r, --loc_pos.y){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = HORIZONTAL;
            }

            else if(direction == VERTICAL && quarter == lower_left){
                if((loc_pos.y + r + 1) > d2->obj.pos.y){
                    if(d2->side == right_side) r = d2->obj.pos.y - loc_pos.y;
                    else if(d2->side == up_side) r = d2->obj.pos.y - loc_pos.y - 1;
                }
                for(; r > 0; --r, ++loc_pos.y){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = HORIZONTAL;
            }
        }
        term_move_cursor(loc_pos.x, loc_pos.y);
        term_putchar(CORRIDOR_FLOOR); 
    }

    else if(d1->side == right_side){
        pos_t loc_pos = {.x = d1->obj.pos.x + 1, .y = d1->obj.pos.y};
        uint8_t direction = HORIZONTAL;
        while((loc_pos.x != arrival_pos.x) || (loc_pos.y != arrival_pos.y)){

            int r = CALC_RAND(7, 5);
            if(direction == HORIZONTAL){
                if((loc_pos.x + r + 1) > d2->obj.pos.x){
                    if(d2->side == left_side) r = d2->obj.pos.x - loc_pos.x - 1;
                    else if(d2->side == down_side || d2->side == up_side) r = d2->obj.pos.x - loc_pos.x;
                }
                for(; r > 0; --r, ++loc_pos.x){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = VERTICAL;
            }
            else if(direction == VERTICAL && quarter == upper_right){
                if((loc_pos.y - r - 1) < d2->obj.pos.y){
                   if(d2->side == left_side)      r = loc_pos.y - d2->obj.pos.y;
                   else if(d2->side == down_side) r = loc_pos.y - d2->obj.pos.y - 1;
                }
                for(; r > 0; --r, --loc_pos.y){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = HORIZONTAL;
            }

            else if(direction == VERTICAL && quarter == lower_right){
                if((loc_pos.y + r + 1) > d2->obj.pos.y){
                    if(d2->side == left_side) r = d2->obj.pos.y - loc_pos.y;
                    else if(d2->side == up_side) r = d2->obj.pos.y - loc_pos.y - 1;
                }
                for(; r > 0; --r, ++loc_pos.y){
                    term_move_cursor(loc_pos.x, loc_pos.y);
                    term_putchar(CORRIDOR_FLOOR);
                }
                direction = HORIZONTAL;
            }
        }
        term_move_cursor(loc_pos.x, loc_pos.y);
        term_putchar(CORRIDOR_FLOOR); 
    }
}