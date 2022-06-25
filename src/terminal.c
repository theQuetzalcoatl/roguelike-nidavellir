#include "../include/terminal.h"
#include "termios.h"
#include <stdint.h>
#include <stdlib.h>


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

    for (uint16_t row = 0; row < TERM_ROWS_NUM; ++row){
        for (uint16_t col = 0; col < TERM_COLS_NUM; ++col){
            win.content[row][col] = ' ';
        }
    }

    tcgetattr(1, &s);

    original_term_settings = s;

    s.c_iflag |= IGNBRK | IGNCR | IGNPAR; /* ignore break(do not register it), ignore CR, ignore parity errors */
    s.c_oflag |= ONLCR;
    s.c_cflag |= CLOCAL;
    s.c_lflag &= ~(ECHO | ICANON);
    //s.c_cc[VTIME] = 0; s.c_cc[VMIN] = 1; IMPLICIT 
    printf("\x1B[?25l"); /* make cursor invisible */

    system("resize -s " TERM_ROWS " "  TERM_COLS " > /dev/null");
    system("temp_PS1=${PS1}");
    system("PS1=\"\""); /* deleting prompt */
    system("clear");

    tcsetattr(1, TCSANOW, &s);
}


void term_restore_original(void)
{
    tcsetattr(1, TCSANOW, &original_term_settings);
    term_move_cursor(1,1);
    system("PS1=$temp_PS1");
    system("clear");
    printf("\x1B[?25h"); /* make cursor visible */
}

void term_move_cursor(uint16_t x, uint16_t y)
{
    printf("\x1B[%u;%uH", y, x);
    win.cursor.x = x;
    win.cursor.y = y;
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

char term_getchar_xy(uint16_t x, uint16_t y)
{
    term_move_cursor(x, y);
    return term_getchar();
}