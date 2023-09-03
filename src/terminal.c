#include "terminal.h"
#include "room.h"
#include "corridor.h"
#include "item.h"
#include "debug.h"
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#pragma GCC diagnostic ignored "-Wunused-result"

typedef struct
{
    pos_t cursor;
    char content[TERMINAL_HEIGHT][TERMINAL_WIDTH];
}window_t;

static window_t win = {0};
static struct termios original_term_settings;

void term_setup(void)
{
    struct termios s;

    tcgetattr(STDOUT_FILENO, &s);
    original_term_settings = s;

    s.c_iflag |= IGNBRK | IGNCR | IGNPAR; /* ignore break(do not register it), ignore CR, ignore parity errors */
    s.c_oflag |= ONLCR;
    s.c_cflag |= CLOCAL;
    s.c_lflag &= ~(ECHO | ICANON);
    //s.c_cc[VTIME] = 0; s.c_cc[VMIN] = 1; IMPLICIT 
    printf("\x1B[?25l"); /* make cursor invisible */
    //printf("\e[8;%d;%dt", TERMINAL_HEIGHT, TERMINAL_WIDTH); /* resize window - 'resize' does the same thing */
    
    system("temp_PS1=${PS1}; PS1=\"\"; clear;");

    for (uint16_t row = 0; row < TERMINAL_HEIGHT; ++row){
        for (uint16_t col = 0; col < TERMINAL_WIDTH; ++col) term_putchar_xy(EMPTY_SPACE, col, row);
    }

    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &s);
}


void term_restore_original(void)
{
    tcsetattr(1, TCSANOW, &original_term_settings);
    term_move_cursor(0,0);
    system("PS1=$temp_PS1; clear;");
    printf("\x1B[?25h"); /* make cursor visible */
}

void term_move_cursor(const uint16_t x, const uint16_t y)
{
    if(x < TERMINAL_WIDTH && y < TERMINAL_HEIGHT){
        printf("\x1B[%u;%uH", y + 1, x + 1); /* terminal is 1 based, not 0 */
        win.cursor.x = x;
        win.cursor.y = y;
    }
}

void term_putchar(char c)
{
    fputc(c, stdout);
    win.content[win.cursor.y][win.cursor.x] = c;
}

char term_getchar(void)
{
    return win.content[win.cursor.y][win.cursor.x];
}

char term_getchar_xy(const uint16_t x, const uint16_t y)
{
    term_move_cursor(x, y);
    return term_getchar();
}

void term_putchar_xy(const char c, const uint16_t x, const uint16_t y)
{
    term_move_cursor(x, y);
    term_putchar(c);
}


#define ROUNDED_DIVISION(a, b) ((((a)<<1)/(b) + 1) >> 1)

bool is_player_in_eyesight(pos_t objp, pos_t playerp)
{
    char c = 0;
    int16_t dx = playerp.x - objp.x;
    int16_t dy = playerp.y -  objp.y;

    if( (1*1 + 1*1) >= (dx*dx + dy*dy)) return true;

    if(dx != 0){
        int16_t m = ROUNDED_DIVISION(dy, dx);
        int16_t b = playerp.y - ROUNDED_DIVISION(dy*playerp.x, dx);
    
        if(m <= -1 || m >= 1){
            int32_t start_y = playerp.y;
            int32_t finish_y = objp.y;
            int32_t x_dir = (playerp.x <= objp.x) ? 1 : -1;
            int32_t y_dir = (playerp.y >= objp.y) ? -1 : 1;

            for(; start_y != finish_y; start_y += y_dir){
                int16_t start_x = ROUNDED_DIVISION(((start_y - b)*dx), dy);
                int16_t finish_x = ROUNDED_DIVISION(((start_y + y_dir - b)*dx), dy);

                for(; start_x != finish_x; start_x += x_dir){
                    c = term_getchar_xy(start_x, start_y+y_dir);
                    if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL){
                       return (start_x == objp.x && start_y == objp.y) ? true : false;
                    }
                }
                /* cover the start_x = finish_x case as well*/
                c = term_getchar_xy( start_x, start_y);
                if(c == ID_PLAYER) continue;
                if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL ){
                    return (start_x == objp.x && start_y == objp.y) ? true : false;
                }
            }
        }
        else{
            int32_t start_x = playerp.x;
            int32_t finish_x = objp.x;
            int32_t x_dir = (playerp.x <= objp.x) ? 1 : -1;
            int32_t y_dir = (playerp.y >= objp.y) ? -1 : 1;

            for(; start_x != finish_x; start_x += x_dir){
                int16_t start_y = ROUNDED_DIVISION((dy*start_x), dx) + b;
                int16_t finish_y = ROUNDED_DIVISION((dy*(start_x + x_dir)), dx) + b;

                for(; start_y != finish_y; start_y += y_dir){
                    c = term_getchar_xy(start_x+x_dir, start_y);
                    if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL){
                        return (start_x == objp.x && start_y == objp.y) ? true : false;
                    }
                }
                /* cover the start_y = finish_y case as well*/
                c = term_getchar_xy(start_x, start_y);
                if(c == ID_PLAYER) continue;
                if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL){
                    return (start_x == objp.x && start_y == objp.y) ? true : false;
                }
            }
        }
    }
    else{ /* vertical case */
        int16_t lower_y = (objp.y > playerp.y) ? objp.y : playerp.y;
        int16_t upper_y = (objp.y > playerp.y) ? playerp.y : objp.y;
        for(++upper_y; upper_y < lower_y; ++upper_y){// not to start on the mob itself, takes care of 'next to each other' case
            c = term_getchar_xy(playerp.x, upper_y);
            if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL) return false;
        }
    }

    return true;
}