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
#include <math.h>


#pragma GCC diagnostic ignored "-Wunused-result"

typedef struct
{
    point_t cursor;
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
    system("temp_PS1=${PS1}; PS1=\"\"; clear;");

    for (uint16_t row = 0; row < TERMINAL_HEIGHT; ++row){
        for (uint16_t col = 0; col < TERMINAL_WIDTH; ++col) term_putchar_xy(EMPTY_SPACE, col, row);
    }

    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &s);
}


void term_restore_original(void)
{
    tcsetattr(STDOUT_FILENO, TCSANOW, &original_term_settings);
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

/*
Explanation:
  floating math:
    deltaY = dY/dX   // deltaY = dY/dX*deltaX (deltaX being 1)
    x1 -> x2 loop:
      plot_pixel(x,y)
      y += dY/dX
  integer math:
    same as floating, but the denominators are the same, so they are discarded.
    x1 -> x2 loop:
      plot_pixel(x,y)
      accumulator = accumulator + deltaY   // acc/deltaX = acc/deltaX + deltaY/deltaX --> accumulate "fractions"
      if accumulator >= deltaX            //  acc/deltaX >= 1 --> when a "fraction" becomes more than 1, "reset" the accumulator
        ++y
        accumulator -= deltaX
*/

bool is_object_in_eyesight(const point_t obj1, const point_t obj2)
{
  char c = 0;
  int64_t accumulator = 0;
  int64_t dx = obj2.x - obj1.x;
  int64_t dy = obj2.y - obj1.y;

  if( (1*1 + 1*1) > (dx*dx + dy*dy) ) return true;

  if(labs(dx) >= labs(dy)){ /* mostly horizontal */
    point_t P1 = (obj2.x <= obj1.x) ? obj2 : obj1;
    point_t P2 = (obj2.x >= obj1.x) ? obj2 : obj1;
    dx = P2.x - P1.x; /* defenitely positive */
    dy = P1.y - P2.y;

    ++P1.x; /* incrementing the starting Y to avoid checking self */
    if(dy >= 0){ /* bottomleft -> topright */
      for(; P1.x < P2.x; ++P1.x){
        accumulator += dy;
        if(accumulator >= dx/2){ // essentially rounding 0.5 progress to 1, because without the division by 2, a dy=1 objects will always see each other
            --P1.y;
            accumulator -= dx; /* this should also be dx/2, but for some reason with small odd numbers, we end up missing our target, I don't know why it is working */
        }
        c = term_getchar_xy(P1.x, P1.y);
        if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR && c != ITEM_SYMBOL) return false;
      }
    }
    else{ /* topleft -> bottomright */
      dy = -1*dy; /* dx is positive, for the math to check out, it should be turned into one as well */
      for(; P1.x < P2.x; ++P1.x){
        accumulator += dy;
        if(accumulator >= dx/2){
            ++P1.y;
            accumulator -= dx;
        }
        c = term_getchar_xy(P1.x, P1.y);
        if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR && c != ITEM_SYMBOL) return false;
      }
    }
  }
  else{ /* mostly vertical */
    point_t P1 = (obj2.y <= obj1.y) ? obj2 : obj1;
    point_t P2 = (obj2.y >= obj1.y) ? obj2 : obj1;
    dx = P1.x - P2.x;
    dy = P2.y - P1.y; /* defenitely positive */

    ++P1.y;
    if(dx > 0){ /* topright -> bottomleft */
      for(; P1.y < P2.y; ++P1.y){
        accumulator += dx;
        if(accumulator >= dy/2){
            --P1.x;
            accumulator -= dy;
        }
        c = term_getchar_xy(P1.x, P1.y);
        if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR && c != ITEM_SYMBOL) return false;
      }
    }
    else if(dx < 0){ /* topleft -> bottomright */
      dx = -1*dx; /* dy is positive, for the math to check out, it should be turned into one as well */
      for(; P1.y < P2.y; ++P1.y){
        accumulator += dx;
        if(accumulator >= dy/2){
            ++P1.x;
            accumulator -= dy;
        }
        c = term_getchar_xy(P1.x, P1.y);
        if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR && c != ITEM_SYMBOL) return false;
      }
    }
    else{ /* vertical case */
      for(; P1.y < P2.y; ++P1.y){
        c = term_getchar_xy(P1.x, P1.y);
        if(c != ROOM_DOOR && c != ROOM_FLOOR && c != CORRIDOR && c != ITEM_SYMBOL) return false;
      }
    }
  }

  return true;
}
