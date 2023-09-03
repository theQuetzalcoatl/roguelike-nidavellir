#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "mob.h"
#include "terminal.h"

extern void display_runic_lines(void);
extern void display_player_stats(const mob_t player, const uint64_t turns);
extern void display_to_player_window(const char * const option);
extern void display_recent_events(void);

#define RUNIC_LINE_POS (TERMINAL_HEIGHT*6)/8 /* 75% */

#endif /* _DISPLAY_H */