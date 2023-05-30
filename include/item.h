#ifndef _ITEM_H
#define _ITEM_H

#include "object.h"
#include "mob.h"
#include <stdint.h>

#define ITEM_SYMBOL '*'

enum ITEMS
{
    I_potion,
    NUM_OF_ITEMS
};

typedef struct item_t
{
  object_t obj;
  uint8_t stands_on;
  uint16_t type;
  void *spec_attr;
  struct item_t *next;
}item_t;

typedef struct
{
  void (*use)(item_t *item);
  char *color;
}potion_t;


extern item_t *item_spawn(void);
extern item_t *item_get(void);
extern void item_hide(item_t it);
extern void item_draw(item_t it);

#endif /* _ITEM_H */