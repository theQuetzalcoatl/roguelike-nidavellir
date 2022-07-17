#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "../include/terminal.h"
#include "../include/cutscenes.h"
#include "../include/creatures.h"
#include "../include/object.h"
#include "../include/room.h"
#include "../include/corridor.h"

int main(void)
{
    srand(time(NULL)); /* RNG init */

    term_setup();

//    cutscene_intro();

    /*room_t r = room_create();
    room_draw(r);

    r = room_create();
    room_draw(r);

    r = room_create();
    room_draw(r);
    creature_t player = {.obj.pos.x=r.obj.pos.x + 1, .obj.pos.y=r.obj.pos.y + 1, .stands_on='.', .symbol='@'};
    creature_move_abs(&player, (pos_t){.x=player.obj.pos.x, .y=player.obj.pos.y});*/

    creature_t player = {.obj.pos.x=10, .obj.pos.y=10, .stands_on='.', .symbol='@'};

    door_t start_d1 = {.obj.pos.x = TERM_COLS_NUM/2-2, .obj.pos.y = TERM_ROWS_NUM/2, .side = up_side};
    door_t end_d1 = {.obj.pos.x = TERM_COLS_NUM/2 - 20, .obj.pos.y = TERM_ROWS_NUM/2 - 20, .side = down_side};
    corridor_create(&start_d1, &end_d1);

    door_t start_d2 = {.obj.pos.x = TERM_COLS_NUM/2, .obj.pos.y = TERM_ROWS_NUM/2, .side = up_side};
    door_t end_d2 = {.obj.pos.x = TERM_COLS_NUM/2 - 15, .obj.pos.y = TERM_ROWS_NUM/2 - 25, .side = right_side};
    corridor_create(&start_d2, &end_d2);

    door_t start_d3 = {.obj.pos.x = TERM_COLS_NUM/2 + 2, .obj.pos.y = TERM_ROWS_NUM/2, .side = up_side};
    door_t end_d3 = {.obj.pos.x = TERM_COLS_NUM/2 + 30, .obj.pos.y = TERM_ROWS_NUM/2 - 25, .side = left_side};
    corridor_create(&start_d3, &end_d3);

    door_t start_d4 = {.obj.pos.x = TERM_COLS_NUM/2 + 4, .obj.pos.y = TERM_ROWS_NUM/2, .side = up_side};
    door_t end_d4 = {.obj.pos.x = TERM_COLS_NUM/2 + 40, .obj.pos.y = TERM_ROWS_NUM/2 - 20, .side = down_side};
    corridor_create(&start_d4, &end_d4);

    /*------------------------------*/

    door_t start_d5 = {.obj.pos.x = TERM_COLS_NUM/2-2, .obj.pos.y = TERM_ROWS_NUM/2 + 9, .side = down_side};
    door_t end_d5 = {.obj.pos.x = TERM_COLS_NUM/2 - 30, .obj.pos.y = TERM_ROWS_NUM/2 + 21, .side = up_side};
    corridor_create(&start_d5, &end_d5);

    door_t start_d6 = {.obj.pos.x = TERM_COLS_NUM/2, .obj.pos.y = TERM_ROWS_NUM/2 + 9, .side = down_side};
    door_t end_d6 = {.obj.pos.x = TERM_COLS_NUM/2 - 15, .obj.pos.y = TERM_ROWS_NUM/2 + 25, .side = right_side};
    corridor_create(&start_d6, &end_d6);

    door_t start_d7 = {.obj.pos.x = TERM_COLS_NUM/2 + 2, .obj.pos.y = TERM_ROWS_NUM/2 + 9, .side = down_side};
    door_t end_d7 = {.obj.pos.x = TERM_COLS_NUM/2 + 30, .obj.pos.y = TERM_ROWS_NUM/2 + 25, .side = left_side};
    corridor_create(&start_d7, &end_d7);

    door_t start_d8 = {.obj.pos.x = TERM_COLS_NUM/2 + 4, .obj.pos.y = TERM_ROWS_NUM/2 + 9, .side = down_side};
    door_t end_d8 = {.obj.pos.x = TERM_COLS_NUM/2 + 40, .obj.pos.y = TERM_ROWS_NUM/2 + 20, .side = up_side};
    corridor_create(&start_d8, &end_d8);
    
    /*------------------------------*/

    door_t start_d9 = {.obj.pos.x = TERM_COLS_NUM/2-6, .obj.pos.y = TERM_ROWS_NUM/2 + 1, .side = left_side};
    door_t end_d9 = {.obj.pos.x = TERM_COLS_NUM/2 - 20, .obj.pos.y = TERM_ROWS_NUM/2 - 3, .side = right_side};
    corridor_create(&start_d9, &end_d9);

    door_t start_d10 = {.obj.pos.x = TERM_COLS_NUM/2 - 6, .obj.pos.y = TERM_ROWS_NUM/2 + 3, .side = left_side};
    door_t end_d10 = {.obj.pos.x = TERM_COLS_NUM/2 - 26, .obj.pos.y = TERM_ROWS_NUM/2, .side = down_side};
    corridor_create(&start_d10, &end_d10);

    door_t start_d11 = {.obj.pos.x = TERM_COLS_NUM/2 - 6, .obj.pos.y = TERM_ROWS_NUM/2 + 5, .side = left_side};
    door_t end_d11 = {.obj.pos.x = TERM_COLS_NUM/2 - 26, .obj.pos.y = TERM_ROWS_NUM/2 + 11, .side = up_side};
    corridor_create(&start_d11, &end_d11);

    door_t start_d12 = {.obj.pos.x = TERM_COLS_NUM/2 - 6, .obj.pos.y = TERM_ROWS_NUM/2 + 7, .side = left_side};
    door_t end_d12 = {.obj.pos.x = TERM_COLS_NUM/2 - 20, .obj.pos.y = TERM_ROWS_NUM/2 + 15, .side = right_side};
    corridor_create(&start_d12, &end_d12);

     /*------------------------------*/

    door_t start_d13 = {.obj.pos.x = TERM_COLS_NUM/2 + 6, .obj.pos.y = TERM_ROWS_NUM/2 + 1, .side = right_side};
    door_t end_d13 = {.obj.pos.x = TERM_COLS_NUM/2 + 20, .obj.pos.y = TERM_ROWS_NUM/2 - 3, .side = left_side};
    corridor_create(&start_d13, &end_d13);

    door_t start_d14 = {.obj.pos.x = TERM_COLS_NUM/2 + 6, .obj.pos.y = TERM_ROWS_NUM/2 + 3, .side = right_side};
    door_t end_d14 = {.obj.pos.x = TERM_COLS_NUM/2 + 26, .obj.pos.y = TERM_ROWS_NUM/2, .side = down_side};
    corridor_create(&start_d14, &end_d14);

    door_t start_d15 = {.obj.pos.x = TERM_COLS_NUM/2 + 6, .obj.pos.y = TERM_ROWS_NUM/2 + 5, .side = right_side};
    door_t end_d15 = {.obj.pos.x = TERM_COLS_NUM/2 + 26, .obj.pos.y = TERM_ROWS_NUM/2 + 11, .side = up_side};
    corridor_create(&start_d15, &end_d15);

    door_t start_d16 = {.obj.pos.x = TERM_COLS_NUM/2 + 6, .obj.pos.y = TERM_ROWS_NUM/2 + 7, .side = right_side};
    door_t end_d16 = {.obj.pos.x = TERM_COLS_NUM/2 + 20, .obj.pos.y = TERM_ROWS_NUM/2 + 15, .side = left_side};
    corridor_create(&start_d16, &end_d16);


    char input_char = 'a';
    fflush(stdout);
    while(input_char != 'q' && input_char != 'Q'){
        input_char = getchar();
        switch(input_char)
        {
            case 'w':
            case 'W':{
                char obj_ahead = term_getchar_xy(player.obj.pos.x, player.obj.pos.y - 1);
                if(player.obj.pos.y > 1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=0, .y=-1});
                break;}

            case 'a':
            case 'A':{
                char obj_ahead = term_getchar_xy(player.obj.pos.x - 1, player.obj.pos.y);
                if(player.obj.pos.x > 1 && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=-1, .y=0});
                break;}
        
            case 's':
            case 'S':{
                char obj_ahead = term_getchar_xy(player.obj.pos.x, player.obj.pos.y + 1);
                if(player.obj.pos.y < TERM_ROWS_NUM && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=0, .y=1});
                break;}
        
            case 'd':
            case 'D':{
                char obj_ahead = term_getchar_xy(player.obj.pos.x + 1, player.obj.pos.y);
                if(player.obj.pos.x < TERM_COLS_NUM && (obj_ahead != VERTICAL_WALL && obj_ahead != HORIZONTAL_WALL)) creature_move_rel(&player, (pos_t){.x=1, .y=0});
                break;}

            default:
                break;
        }
        fflush(stdout);
    }

    term_restore_original();

    return 0;
}
