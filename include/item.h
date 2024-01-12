#ifndef _ITEM_H
#define _ITEM_H


#include "mob.h"
#include "terminal.h"
#include <stdint.h>

#define ITEM_SYMBOL '*'

enum ITEMS
{
  I_potion,
  NUM_OF_ITEMS
};

typedef struct item_t
{
  point_t pos;
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
extern void item_show(item_t it);
extern void item_free_items(void);

#endif /* _ITEM_H */