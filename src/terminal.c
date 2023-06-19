#include "terminal.h"
#include "room.h"
#include "corridor.h"
#include "item.h"
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#pragma GCC diagnostic ignored "-Wunused-result"

typedef struct
{
    pos_t cursor;
    char content[TERM_ROWS_NUM][TERM_COLS_NUM];
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
    //printf("\e[8;%d;%dt", TERM_ROWS_NUM, TERM_COLS_NUM); /* resize window - 'resize' does the same thing */
    
    system("temp_PS1=${PS1}; PS1=\"\"; clear;");

    for (uint16_t row = 0; row < TERM_ROWS_NUM; ++row){
        for (uint16_t col = 0; col < TERM_COLS_NUM; ++col) term_putchar_xy(EMPTY_SPACE, col, row);
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
    if(x < TERM_COLS_NUM && y < TERM_ROWS_NUM){
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


bool is_player_in_eyesight(pos_t objp, pos_t playerp)
{
    char c = 0;
    int16_t dx = objp.x - playerp.x;
    int16_t dy = objp.y - playerp.y;

    if( (1*1 + 1*1) >= (dx*dx + dy*dy)) return true;

    if(dx != 0){
        int16_t m = (objp.y - playerp.y) / (objp.x - playerp.x);
        int16_t b = objp.y - (dy*objp.x)/dx;
        
        if(m <= -1 && m >= 1){
            int16_t lower_y = (objp.y > playerp.y) ? objp.y : playerp.y;
            int16_t upper_y = (objp.y > playerp.y) ? playerp.y : objp.y;
            for(++upper_y; upper_y < lower_y; ++upper_y){ // not to start on the mob itself, takes care of 'next to each other' case
                c = term_getchar_xy( ((upper_y - b)*dy)/dy, upper_y);
                if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL) return false;
            }
        }
        else{
            int16_t right_x = (objp.x > playerp.x) ? objp.x : playerp.x;
            int16_t left_x  = (objp.x > playerp.x) ? playerp.x : objp.x;
            for(++left_x; left_x < right_x; ++left_x){ // not to start on the mob itself, takes care of 'next to each other' case
                c = term_getchar_xy(left_x, (dy*left_x)/dx + b);
                if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR_FLOOR && c != ITEM_SYMBOL) return false;
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