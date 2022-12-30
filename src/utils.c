#include "utils.h"

void limit(int16_t max, int16_t *val, int16_t min)
{
    if(*val > max) *val = max;
    else if(*val < min) *val = min;
}