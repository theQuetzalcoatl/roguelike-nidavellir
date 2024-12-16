#ifndef _ITEM_H
#define _ITEM_H


#include "mob.h"
#include "terminal.h"
#include <stdint.h>

#define ITEM_SYMBOL '*'
#define DROP_ITEM (4u)
#define USE_ITEM  (5u)
#define DROPPED true
#define SPEC_ATTR(p_item, type) ((type *)p_item->spec_attr)
#define HAS_ARMOR(p_mob) (!!((p_mob)->gear.armor))

enum ITEMS
{
  I_potion,
  I_armor,
  ITEMS_COUNT
};

struct mob_t; /* forward declaring mob to be able to use it */

typedef struct item_t
{
  void (*use)(struct item_t **item);
  char *description;
  struct item_t *next;
  void *spec_attr;
  point_t pos;
  uint16_t type;
  uint8_t stands_on;
}item_t;

/* -------- POTIONS -------- */

typedef struct
{
  char *color;
}potion_t;

enum POTION_COLOR
{
  RED,
  BLUE,
  GREEN,
  PURPLE,
  TRANSPARENT,
  COLOR_COUNT
};


/* -------- ARMOR -------- */

typedef struct
{
  uint8_t type;
  int8_t durability;
}armor_t;

enum ARMOR_TYPE
{
  CLOTH,
  GAMBISON,
  MAIL,
  PLATE,
  ARMOR_TYPE_COUNT
};

extern item_t *item_spawn(int type);
extern item_t *item_get_list(void);
extern void item_hide(item_t it);
extern void item_show(item_t it);
extern void item_free_items(void);
extern void item_drop(item_t *i, struct mob_t *m);
extern void item_destroy(item_t *i);

#endif /* _ITEM_H */
