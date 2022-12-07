#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "terminal.h"
#include "mob.h"

extern void display_runic_line(void);
extern void display_player_stats(const mob_t player);

#define RUNIC_LINE (TERM_ROWS_NUM*6)/8


#endif /* _DISPLAY_H */