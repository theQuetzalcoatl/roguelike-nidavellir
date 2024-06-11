#ifndef _ITEM_H
#define _ITEM_H


#include "mob.h"
#include "terminal.h"
#include <stdint.h>

#define ITEM_SYMBOL '*'
#define DROP_ITEM (4u)
#define USE_ITEM  (5u)
#define DROPPED true

enum ITEMS
{
  I_potion,
  NUM_OF_ITEMS
};

struct mob_t; /* forward declaring mob to be able to use it */

typedef struct item_t
{
  point_t pos;
  uint8_t stands_on;
  uint16_t type;
  char *description;
  void *spec_attr;
  void (*use)(struct item_t *item);
  struct item_t *next;
}item_t;

typedef struct
{
  char *color;
}potion_t;


extern item_t *item_spawn(void);
extern item_t *item_get_list(void);
extern void item_hide(item_t it);
extern void item_show(item_t it);
extern void item_free_items(void);
extern void item_drop(item_t *i, struct mob_t *m);

#endif /* _ITEM_H */
