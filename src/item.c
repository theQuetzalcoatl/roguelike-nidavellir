#include "mob.h"
#include "item.h"
#include "room.h"
#include "debug.h"
#include <string.h>
#include <stdbool.h>

extern bool custom_mode;

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
  spawned_item->stands_on = ROOM_FLOOR;
  spawned_item->type = blue_potion; // NOTE: change this to random
  spawned_item->obj.pos = (pos_t){.x = 0, .y = 0};

  if(custom_mode == false){
    room_t *r = room_get_rooms();
    uint8_t random_room = CALC_RAND(room_get_num_of_rooms()-1, 0);

    while(spawned_item->obj.pos.x == 0 && spawned_item->obj.pos.y == 0){
      uint8_t random_x = CALC_RAND(r[random_room].width-1, 0) + r[random_room].obj.pos.x;
      uint8_t random_y = CALC_RAND(r[random_room].height-1, 0) + r[random_room].obj.pos.y;

      if(term_getchar_xy(random_x, random_y) == ROOM_FLOOR){
        spawned_item->obj.pos = (pos_t){.x=random_x, .y=random_y};
        term_putchar_xy(ITEM_SYMBOL, random_x, random_y);
        break;
      }
      ++tries;
    }
  }

  if(tries == 3){
    free(spawned_item);
    nidebug("Could not find a place for item!\n");
    spawned_item = NULL;
  }
  else{
    if(items_head == NULL) items_head = spawned_item;
    else{
      item_t *it = items_head;
      for(; it->next; it = it->next);
      it->next = spawned_item;
    }
      spawned_item->next = NULL;
  }

  /* specify item */
  if(spawned_item != NULL){
    potion_t *spec_item = malloc(sizeof(potion_t));
    if(spec_item == NULL){
      printf("Could not spawn special item! In %s at %i.\n", __FILE__, __LINE__);
      exit(1);
    }

    spec_item->color = calloc(10, sizeof(char));
    strcpy(spec_item->color, "Blue");
    spec_item->use = health_up_by_10;

    spawned_item->spec_attr = spec_item;
  }

  return spawned_item;
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
      prev_it->next = i->next;
      free(i->spec_attr);
      free(i); // NOTE: string attribute is still unfreed, memory leak
      break;
    }
    else prev_it = search_it;
    search_it = search_it->next;
  }
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
