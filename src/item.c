#include "mob.h"
#include "item.h"
#include "room.h"
#include "debug.h"
#include <string.h>
#include <stdbool.h>

#define EMPTY NULL

static void health_up_by_10(item_t **item);
static void health_up_by_5(item_t **item);
static void equip(item_t **item);

static item_t *items_head = EMPTY;

static bool is_next_to_door(const point_t p, const room_t *r)
{
  return ( ((p.x == r->right_door.pos.x-1) && (p.y == r->right_door.pos.y)) || 
           ((p.x == r->left_door.pos.x+1) && (p.y == r->left_door.pos.y))   ||
           ((p.y == r->upper_door.pos.y+1) && (p.x == r->upper_door.pos.x)) || 
           ((p.y == r->lower_door.pos.y-1 && p.x == r->lower_door.pos.x)) );
} /* TODO: refactor by simplifying the bool expression  */

item_t *item_spawn(int type)
{
  uint8_t tries = 0;
  item_t *spawned_item = malloc(sizeof(item_t));
  if(spawned_item == NULL){
    nidebug("Could not spawn item! In %s at %i.\n", __FILE__, __LINE__);
    return NULL;
  }

  *spawned_item = (item_t){.next = NULL, .pos.x = 0, .pos.y = 0, .type = type};

  /* finding a places to put the item at  */
  room_t *r = room_get_rooms();
  uint8_t random_room = CALC_RAND(room_get_room_count()-1, 0);
  spawned_item->stands_on = random_room != 0 ? EMPTY_SPACE: ROOM_FLOOR; /* there can be item in the starting room, which is already drawn */

  while(tries < 4){
    uint8_t x = CALC_RAND(r[random_room].width-2, 1) + r[random_room].pos.x;
    uint8_t y = CALC_RAND(r[random_room].height-2, 1) + r[random_room].pos.y;

    for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next){
      if(mob->pos.x == x && mob->pos.y == y) goto no_luck;
    }

    for(item_t *i = item_get_list(); i; i = i->next){
      if(i->pos.x == x && i->pos.y == y) goto no_luck;
    }

    if(is_next_to_door((point_t){.x=x, .y=y}, &r[random_room])) goto no_luck;

    spawned_item->pos = (point_t){.x=x, .y=y};
    break;
    
no_luck:
    ++tries;
    continue;
  }

  if(tries > 3){
    free(spawned_item);
    nidebug("Could not find a place for item!\n");
    return NULL;
  }
  else if(item_get_list() == EMPTY) items_head = spawned_item;
	else{
    item_t *it = items_head;
    for(; it->next; it = it->next);
    it->next = spawned_item;
  }

  /* specify item */
  switch(spawned_item->type)
  {
    case I_potion:
      spawned_item->description = strdup("potion");
      spawned_item->spec_attr = malloc(sizeof(potion_t));
      char *color = NULL;

      switch(CALC_RAND(COLOR_COUNT-1, 0))
      {
        case RED: color = strdup("red");
          break;
        case BLUE: color = strdup("blue");
          break;
        case GREEN: color = strdup("green");
          break;
        case PURPLE: color = strdup("purple");
          break;
        case TRANSPARENT: color = strdup("transparent");
          break;
        default: color = strdup("COLOR");
      }
      SPEC_ATTR(spawned_item, potion_t)->color = color;

      for(item_t *it = item_get_list(); it; it = it->next){
        if(it->type == I_potion && !strcmp(color, SPEC_ATTR(it, potion_t)->color)){
          spawned_item->use = it->use;
          break;
        }
      }

      if(spawned_item->use == NULL)  spawned_item->use = rand()%100 > 50 ? health_up_by_10 : health_up_by_5;
      break;

    case I_armor:
      spawned_item->spec_attr = malloc(sizeof(armor_t)); /* TODO: free it in item_remove  */
      int type = CALC_RAND(ARMOR_TYPE_COUNT-1, CLOTH);
      SPEC_ATTR(spawned_item, armor_t)->durability = type*10;

      switch(type)
      {
        case CLOTH:
          spawned_item->description = strdup("Cloth");
          SPEC_ATTR(spawned_item, armor_t)->durability = 1; 
          break;
        case GAMBISON:
          spawned_item->description = strdup("Gambison");
          break;
        case MAIL:
          spawned_item->description = strdup("Mail");
          break;
        case PLATE:
          spawned_item->description = strdup("Plate");
          break;
        default: 
          spawned_item->description = strdup("FILL ARMOR DESC"); 
      }
      SPEC_ATTR(spawned_item, armor_t)->type = type;
      spawned_item->use = equip;
      break;

    default: nidebug("Invalid item type!"); 
  }

  return spawned_item;
}


void item_free_items(void)
{
  item_t *item;
  while(items_head){
    item = items_head->next;
    free(items_head);
    items_head = item;
  }
}


item_t *item_get_list(void)
{
  return items_head;
}

void item_remove_from_list(item_t *i)
{
  item_t *search_it = items_head;

  if(i == search_it) items_head = items_head->next;
  else{
    while(search_it->next != i) search_it = search_it->next;
    search_it->next = i->next;
  }
}

void item_destroy(item_t *i)
{
  if(i->type == I_potion) free(SPEC_ATTR(i, potion_t)->color);
  else if(i->type == I_armor) {/* TODO: free armor */}
  free(i->spec_attr);
  free(i->description);
  free(i);
}


void item_show(item_t it)
{
  SET_FG_COLOR(YELLOW_TEXT);
  term_putchar_xy(ITEM_SYMBOL, it.pos.x, it.pos.y);
  SET_FG_COLOR(DEFAULT_TEXT);
}

void item_hide(item_t it)
{
  term_putchar_xy(it.stands_on, it.pos.x, it.pos.y);
}


void item_drop(item_t *i, struct mob_t *m)
{
  i->stands_on = m->stands_on;
  m->stands_on = ITEM_SYMBOL;
  i->pos = m->pos;
  i->next = items_head;
  items_head = i;
}


/****************************
 ** ITEM ACTIONS
 ***************************/

static void health_up_by_10(item_t **item)
{
  mob_t *player =  mob_get_player();
  player->health += 10u;
  player->health -= (player->health*(player->health/100) % PLAYER_MAX_HEALTH); /* capping it to max unreaosonably complicatedly */
  item_destroy(*item);
  *item = NULL;
}


static void health_up_by_5(item_t **item)
{
  mob_t *player =  mob_get_player();
  player->health += 5u;
  player->health -= (player->health*(player->health/100) % PLAYER_MAX_HEALTH); /* capping it to max unreaosonably complicatedly */
  item_destroy(*item);
  *item = NULL;
}


static void equip(item_t **item)
{
  item_t *tmp = mob_get_player()->gear.armor;
  mob_get_player()->gear.armor = *item;
  *item = tmp;
}
