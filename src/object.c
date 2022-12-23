#include "object.h"
#include "terminal.h"

void obj_make_visible(object_t * const obj)
{
    obj->is_visible = true;
}


void obj_make_invisible(object_t * const obj)
{
    obj->is_visible = false;
}


pos_t obj_get_pos(object_t const *obj)
{
    return obj->pos;
}