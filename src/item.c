#include "mob.h"
#include "item.h"
#include "room.h"
#include "debug.h"
#include <string.h>
#include <stdbool.h>

#define EMPTY NULL

static void health_up_by_10(item_t *item);
static void destroy_item(item_t *i);

static item_t *items_head = EMPTY;

static bool is_near_door(uint16_t x, uint16_t y)
{
  if(ROOM_DOOR == term_getchar_xy(x-1, y)) return true;
  else if(ROOM_DOOR == term_getchar_xy(x+1, y)) return true;
  else if(ROOM_DOOR == term_getchar_xy(x, y-1)) return true;
  else if(ROOM_DOOR == term_getchar_xy(x, y+1)) return true;
  else return false;
}

item_t *item_spawn(void)
{
  uint8_t tries = 0;
  item_t *spawned_item = malloc(sizeof(item_t));
  if(spawned_item == NULL){
    nidebug("Could not spawn item! In %s at %i.\n", __FILE__, __LINE__);
    return NULL;
  }

  *spawned_item = (item_t){.next = NULL, .pos.x = 0, .pos.y = 0, .type = I_potion};

  room_t *r = room_get_rooms();
  uint8_t random_room = CALC_RAND(room_get_num_of_rooms()-1, 0);

  spawned_item->stands_on = random_room != 0 ? EMPTY_SPACE: ROOM_FLOOR; /* there can be item in the starting room, which is already drawn */
  while(tries < 4){
		bool found = false;
    uint8_t random_x = CALC_RAND(r[random_room].width-2, 1) + r[random_room].pos.x;
    uint8_t random_y = CALC_RAND(r[random_room].height-2, 1) + r[random_room].pos.y;

    if(item_get_list() == EMPTY){
			spawned_item->pos = (point_t){.x=random_x, .y=random_y};
			break;
		}
		else{
			for(item_t *i = item_get_list(); i; i = i->next){
				if(i->pos.x == random_x && i->pos.y == random_y){
					++tries;
					found = true;
          break;
				}
			}
			if(!found && !is_near_door(random_x, random_y)){
				spawned_item->pos = (point_t){.x=random_x, .y=random_y};
				break;
			}
		}
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
      spawned_item->description = strdup("potion ");
      spawned_item->use = health_up_by_10;
      spawned_item->spec_attr = malloc(sizeof(potion_t));
      ((potion_t*)spawned_item->spec_attr)->color = strdup("blue ");
      break;
    default: nidebug("Invalid item type!"); 
  }

  return spawned_item;
}

void item_free_items(void)
{
  item_t *item = NULL;
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

static void destroy_item(item_t *i)
{
  item_t *search_it = items_head;

  if(i == search_it) items_head = items_head->next;
  else{
    while(search_it->next != i) search_it = search_it->next;
    search_it->next = i->next;
  }
  item_hide(*i);
  free(((potion_t*)i->spec_attr)->color);
  free(i->spec_attr);
  free(i);
}


void item_show(item_t it)
{
  term_putchar_xy(ITEM_SYMBOL, it.pos.x, it.pos.y);
}

void item_hide(item_t it)
{
  term_putchar_xy(it.stands_on, it.pos.x, it.pos.y);
}


/****************************
 ** ITEM ACTIONS
 ***************************/

static void health_up_by_10(item_t *item)
{
  mob_t *player =  mob_get_player();
  player->health += 10u;
  player->health -= (player->health*(player->health/100) % PLAYER_MAX_HEALTH); /* capping it to max unreaosonably complicatedly */
  destroy_item(item);
}
