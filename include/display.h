#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "terminal.h"
#include "mob.h"

extern void display_runic_lines(void);
extern void display_player_stats(const mob_t player, const uint64_t turns);

#define RUNIC_LINE_POS (TERM_ROWS_NUM*6)/8 /* 75% */

#endif /* _DISPLAY_H */