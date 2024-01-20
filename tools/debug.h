#ifndef _DEBUG_H
#define _DEBUG_H

#include "room.h"
#include "mob.h"
#include "item.h"

extern void debug_init(void);
extern void debug_deinit(void);
extern void nidebug(const char *s, ...);
extern void debug_display_object_stats(const room_t *r, const item_t *i, const mob_t *m);

#endif /* _DEBUG_H */