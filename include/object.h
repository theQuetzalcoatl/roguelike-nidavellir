#ifndef _OBJECT_
#define _OBJECT_

#include <stdint.h>
#include "terminal.h"
#include <stdbool.h>


typedef struct object_t
{
    pos_t pos;
    bool is_visible;
}object_t;


extern void obj_make_invisible(object_t * const obj);
extern void obj_make_visible(object_t * const obj);

#endif /* _OBJECT_ */