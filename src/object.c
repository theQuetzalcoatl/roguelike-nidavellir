#include "../include/object.h"
#include "../include/terminal.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void obj_make_visible(object_t * const obj)
{
    obj->is_visible = true;
}


void obj_make_invisible(object_t * const obj)
{
    obj->is_visible = false;
}