#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
extern void debug_init(void);
extern void debug_deinit(void);
extern void nidebug(const char *s, ...);

#endif /* _DEBUG_H */