#include "mob.h"
#include "item.h"
#include "room.h"
#include "debug.h"
#include <string.h>
#include <stdbool.h>

static void health_up_by_10(item_t *item);
static void destroy_item(item_t *i);

static item_t *items_head = NULL;


item_t *item_spawn(void)
{
  uint8_t tries = 0;
  item_t *spawned_item = malloc(sizeof(item_t));
  if(spawned_item == NULL){
    printf("Could not spawn item! In %s at %i.\n", __FILE__, __LINE__);
    exit(1);
  }

  spawned_item->stands_on = ITEM_SYMBOL;
  spawned_item->type = I_potion; // NOTE: change this to random
  spawned_item->pos = (point_t){.x = 0, .y = 0};

  room_t *r = room_get_rooms();
  uint8_t random_room = CALC_RAND(room_get_num_of_rooms()-1, 0);

  spawned_item->stands_on = random_room != 0 ? EMPTY_SPACE: ROOM_FLOOR; /* there can be item in the starting room, which is already drawn */
  while(tries < 4 ){
    uint8_t random_x = CALC_RAND(r[random_room].width-2, 1) + r[random_room].pos.x;
    uint8_t random_y = CALC_RAND(r[random_room].height-2, 1) + r[random_room].pos.y;

    if(items_head == NULL) items_head = spawned_item;
    for(item_t *i = item_get(); i; i = i->next){
      if(i->pos.x != random_x || i->pos.y != random_y){
        spawned_item->pos = (point_t){.x=random_x, .y=random_y};
        goto found_place;
      }
      else ++tries;
    };
  }

found_place:

  if(tries > 3){
    free(spawned_item);
    nidebug("Could not find a place for item!\n");
    return NULL;
  }
  else{
    item_t *it = items_head;
    for(; it->next; it = it->next);
    it->next = spawned_item;
    spawned_item->next = NULL;
  }

  /* specify item */
  potion_t *spec_item = malloc(sizeof(potion_t));
  if(spec_item == NULL){
    printf("Could not spawn special item! In %s at %i.\n", __FILE__, __LINE__);
    exit(1);
  }

  spec_item->color = calloc(10, sizeof(char));
  strcpy(spec_item->color, "Blue");
  spec_item->use = health_up_by_10;

  spawned_item->spec_attr = spec_item;

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


item_t *item_get(void)
{
  return items_head;
}

static void destroy_item(item_t *i)
{
  item_t *prev_it = items_head;
  item_t *search_it = items_head;
  while(search_it){ /* last item's next is always NULL */
    if(search_it == i){
      if(search_it == items_head) items_head = items_head->next; /* to be able to get all the remaining items after the first is removed */
      prev_it->next = i->next;
      free(((potion_t*)i->spec_attr)->color);
      free(i->spec_attr);
      free(i); 
      break;
    }
    else prev_it = search_it;
    search_it = search_it->next;
  }
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
  player->health -= (player->health*(player->health/100) % PLAYER_MAX_HEALTH); /* capping it to max unreaosonably complicatedly*/
  destroy_item(item);
}
