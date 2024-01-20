#ifndef _TERMINAL_
#define _TERMINAL_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct point_t
{
  int16_t x;
  int16_t y;
}point_t;

#define CALC_RAND(max, min) (rand()%(max - min + 1) + min) // [max:min]


extern void term_setup(void);
extern void term_restore_original(void);
extern void term_move_cursor(const uint16_t x, const uint16_t y);
extern void term_putchar(char c);
extern void term_putchar_xy(const char c, const uint16_t x, const uint16_t y);
extern char term_getchar(void);
extern char term_getchar_xy(const uint16_t x, const uint16_t y);
extern  bool is_obejct_in_eyesight(point_t obj1, point_t obj2);

#define TERMINAL_HEIGHT (40u)
#define TERMINAL_WIDTH (160u) 

#endif /* _TERMINAL_ */ 