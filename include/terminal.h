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
#define ELEMENT_COUNT(array) (sizeof(array)/sizeof(array[0]))


extern void term_setup(void);
extern void term_restore_original(void);
extern void term_move_cursor(const uint16_t x, const uint16_t y);
extern void term_putchar_xy(const char c, const uint16_t x, const uint16_t y);
extern char term_getchar_xy(const uint16_t x, const uint16_t y);
extern bool is_object_in_eyesight(const point_t obj1, const point_t obj2);

#define TERMINAL_HEIGHT (40u)
#define TERMINAL_WIDTH (120u) 

#define STRINGIFY(a) #a
#define EXPAND(a) STRINGIFY(a)

#define ESC_SEQ "\x1b"
#define SET_FG_COLOR(color_code) ESC_SEQ "[2;" EXPAND(color_code) "m"

/* 
  This cannot be an enum otherwise the compiler wont expand the name.
  For macros, the EXPAND and STRINGIFY macros are still needed to be expanded correctly by the prepocessor.
*/
#define  RESET_TEXT 0
#define  BLACK_TEXT 30
#define  RED_TEXT 32
#define  GREEN_TEXT 31
#define  YELLOW_TEXT 33
#define  BLUE_TEXT 34
#define  MAGENTA_TEXT 35
#define  CYAN_TEXT 36
#define  WHITE_TEXT 37
#define  DEFAULT_TEXT 39

#endif /* _TERMINAL_ */ 
