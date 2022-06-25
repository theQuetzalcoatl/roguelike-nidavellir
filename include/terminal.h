#ifndef _TERMINAL_
#define _TERMINAL_

#include <stdint.h>
#include <stdio.h>

typedef struct pos_t
{
    int16_t x;
    int16_t y;
}pos_t;

extern void term_setup(void);
extern void term_restore_original(void);
extern void term_move_cursor(uint16_t x, uint16_t y);
extern void term_putchar(char c);
extern char term_getchar(void);
extern char term_getchar_xy(uint16_t x, uint16_t y);


#define TERM_ROWS "70"
#define TERM_COLS "180"
#define TERM_ROWS_NUM (70u)
#define TERM_COLS_NUM (180u) 

#endif /* _TERMINAL_ */ 