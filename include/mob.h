#ifndef _CREATURE_
#define _CREATURE_

#include <stdint.h>
#include "terminal.h"

#include "item.h"
#include "input.h"

#define ID_PLAYER '@'
#define ID_GOBLIN 'G'
#define ID_DRAUGR 'D'

#define INVENTORY_WIDTH  (3u)
#define INVENTORY_HEIGHT (3u)
#define INVENTORY_SIZE (INVENTORY_WIDTH*INVENTORY_HEIGHT)
#define INV_EMPTY (NULL)


typedef uint16_t mob_id_t;

struct item_t; /* forward declaring item to be able to use it */

typedef struct mob_t
{
  struct item_t *inventory[INVENTORY_SIZE]; /* NOTE: compiler doesn't accept simple item_t, fix it */
  struct mob_t *next;
  struct{
    struct item_t *lhand, *rhand, *armor;
  }gear;
  point_t pos;
  point_t last_seen;
  char symbol;
  uint8_t stands_on;
  int8_t health;
  uint8_t level;
}mob_t;

#define PLAYER_MAX_HEALTH (100u)

extern void mob_move_by(mob_t *creature, int16_t dx, int16_t dy);
extern void mob_free_mobs(void);
extern mob_t *mob_summon(const mob_id_t id);
extern mob_t *mob_get_mobs(void);
extern void mob_update(mob_t *mob, input_code_t step_to);
extern void mob_show(mob_t *mob);
extern void mob_hide(mob_t mob);
extern void mob_handle_movement(mob_t *mob, input_code_t step_to);
extern mob_t *mob_get_player(void);
extern void mob_open_player_inventory(const uint8_t action);

#endif /* _CREATURE_ */
