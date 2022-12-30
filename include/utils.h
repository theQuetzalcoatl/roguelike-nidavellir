#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

extern void limit(int16_t max, int16_t *val, int16_t min);

#define CALC_RAND(max, min) (rand()%(max - min + 1) + min) // [max:min]

#endif /* _UTILS_H */