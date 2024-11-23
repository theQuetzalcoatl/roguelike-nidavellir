#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "mob.h"
#include "debug.h"
#include "room.h"
#include "corridor.h"
#include "item.h"
#include "display.h"

static void summon_player(mob_t *player);
static void summon_goblin(mob_t *goblin);
static void summon_draugr(mob_t *draugr);
static void add_to_list(mob_t *mob);
static void move_towards_last_seen_pos(mob_t *mob);

static mob_t *head = NULL;
static mob_t *player = NULL;

extern void event_log_add(const char *event);
extern void draw(void);
extern bool g_game_is_running;
extern bool g_input_source;

#define UPPER_EDGE_OF_MAP (-1)
#define LEFT_EDGE_OF_MAP  (-1)


void mob_free_mobs(void)
{
  mob_t *mob = NULL;
  while(head){
    mob = head->next;
    free(head);
    head = mob;
  }
}


static void remove_mob(mob_t *mob)
{
  mob_t *search_mob = head;
  
  if(mob == search_mob) head = head->next;
  else{
    while(search_mob->next != mob) search_mob = search_mob->next;
    search_mob->next = mob->next;
  }
  term_putchar_xy(mob->stands_on, mob->pos.x, mob->pos.y);
  free(mob);
}


void mob_move_to(mob_t *mob, int16_t x, int16_t y)
{
  if(x > LEFT_EDGE_OF_MAP && y > UPPER_EDGE_OF_MAP){
    term_putchar_xy(mob->stands_on, mob->pos.x, mob->pos.y);
    mob->stands_on = term_getchar_xy(x, y);
    mob->pos.x = x;
    mob->pos.y = y;
  }
}

void mob_move_by(mob_t *mob, int16_t dx, int16_t dy)
{
  if((dx + mob->pos.x) > LEFT_EDGE_OF_MAP && (dy + mob->pos.y) > UPPER_EDGE_OF_MAP){
    term_putchar_xy(mob->stands_on, mob->pos.x, mob->pos.y);
    mob->stands_on = term_getchar_xy(mob->pos.x + dx, mob->pos.y + dy);
    mob->pos.x += dx;
    mob->pos.y += dy;
  }
}

void mob_show(mob_t mob)
{
  term_putchar_xy(mob.symbol, mob.pos.x, mob.pos.y);
}

void mob_hide(mob_t mob)
{
  term_putchar_xy(mob.stands_on, mob.pos.x, mob.pos.y);
}

mob_t *mob_get_mobs(void) {  return head; }

mob_t *mob_get_player(void) { return player; }


static void attack(mob_t *attacked_mob)
{ /* TODO: refactor hardcoded stuff */
  int16_t damage = CALC_RAND(20,0);
  item_t *armor = attacked_mob->gear.armor;

  if(HAS_ARMOR(attacked_mob)){
    if(SPEC_ATTR(armor, armor_t)->durability) damage -= (damage*SPEC_ATTR(armor, armor_t)->type)/10;
  }
  attacked_mob->health -= damage;
  if(HAS_ARMOR(attacked_mob) && SPEC_ATTR(armor, armor_t)->durability){
    --(SPEC_ATTR(armor, armor_t)->durability);
    
    if(SPEC_ATTR(armor, armor_t)->durability == 0){
      item_destroy(armor);
      attacked_mob->gear.armor = NULL;
    }
  }

  if(attacked_mob->health <= 0){
    attacked_mob->health = 0;
    if(attacked_mob == player) g_game_is_running = false;
    else remove_mob(attacked_mob);
  }
}


static void place_into_inventory(mob_t *m, item_t *i)
{
  char event_string[200];
	for(uint8_t slot = 0; slot < INVENTORY_SIZE; ++slot){
		if(m->inventory[slot] == INV_EMPTY){
			m->inventory[slot] = i;
			item_hide(*i);
			i->stands_on = EMPTY_SPACE; /* (0;0) is definitely empty space */ 
			i->pos.x = i->pos.y = 0; /* NOTE: change it to a define UNKONW place or something */
      snprintf(event_string, 50, "You picked up a %s %s", SPEC_ATTR(i, potion_t)->color, i->description);
			event_log_add(event_string);
			return;
		}
	}
}


void mob_open_player_inventory(const uint8_t action)
{
  for(uint8_t row = 0; row < RUNIC_LINE_POS; ++row){
  	term_move_cursor(0, row);
    for(uint8_t col = 0; col < TERMINAL_WIDTH-2; ++col) printf(" ");
  }

  draw();
  term_move_cursor(0,0);

	mob_t *p = mob_get_player();

	for(uint8_t slot = 0; slot < INVENTORY_SIZE; ++slot){
		if(p->inventory[slot] != INV_EMPTY) printf(" %d %s %s\n", slot, SPEC_ATTR(p->inventory[slot], potion_t)->color, p->inventory[slot]->description); /* NOTE: this is temporary */
		else printf(" x empty\n");
	}

  term_move_cursor(0, RUNIC_LINE_POS - 1);
  char *action_str = (action == DROP_ITEM) ? "drop" : "use";
  printf("Press 0-8 to choose which item to %s, any other key to get back...\n", action_str);
	uint8_t requested_slot = get_keypress() - '0';

  if(requested_slot < INVENTORY_SIZE){
		if(p->inventory[requested_slot] != INV_EMPTY){
      if(action == USE_ITEM){
        p->inventory[requested_slot]->use(p->inventory[requested_slot]);
        p->inventory[requested_slot] = INV_EMPTY;
        if(p->stands_on == ITEM_SYMBOL){
          for(item_t *i = item_get_list(); i; i = i->next){
            if(i->pos.x == p->pos.x && i->pos.y == p->pos.y){
              p->stands_on = i->stands_on;
              place_into_inventory(p, i);
              break;
            }
          }
        }
      }
      else if(action == DROP_ITEM && p->stands_on == ROOM_FLOOR){
        item_drop(p->inventory[requested_slot], p);
        p->inventory[requested_slot] = INV_EMPTY;
      }
		}
		else event_log_add("You found nothing in your backpack");
	}

  for(uint8_t col = 0; col < TERMINAL_WIDTH-2; ++col){
    for(uint8_t row = 0; row < RUNIC_LINE_POS; ++row) term_putchar_xy(term_getchar_xy(col, row), col, row);
  }

  draw();
}

void mob_handle_movement(mob_t *mob, input_code_t step_to)
{
  char obj_ahead = 0;
  int16_t dx = 0;
  int16_t dy = 0;

  switch(step_to)
  {
    case STEP_UP:
      obj_ahead = term_getchar_xy(mob->pos.x, mob->pos.y - 1); 
      dx = 0;
      dy = -1;
      break;
    case STEP_LEFT:
      obj_ahead = term_getchar_xy(mob->pos.x - 1, mob->pos.y);
      dx = -1;
      dy = 0;
      break;
    case STEP_DOWN:
      obj_ahead = term_getchar_xy(mob->pos.x, mob->pos.y + 1);
      dx = 0;
      dy = 1;
      break;
    case STEP_RIGHT:
      obj_ahead = term_getchar_xy(mob->pos.x + 1, mob->pos.y);
      dx = 1;
      dy = 0;
      break;
    case NO_ARROW:
    case '.':
      return;
    default:
      nidebug("invalid option for stepping! %s:%d", __FILE__, __LINE__); /* TODO: using an item from inventory triggers this, handle it  */
  }

  switch(obj_ahead)
  {
    case ROOM_FLOOR:
    case ROOM_DOOR:
      mob_move_by(mob, dx, dy);
      break;
    case CORRIDOR:
      if(mob == player){
        corridor_t *c = room_find_corridor_with_player(mob->pos);
        if(c) room_draw_corridor_piece(c, mob->pos);
      }
      mob_move_by(mob, dx, dy);
      break;
    case VERTICAL_WALL:
    case HORIZONTAL_WALL:
      break;
    case EMPTY_SPACE:
      if(mob->stands_on == CORRIDOR){
        room_t *r = room_find((point_t){.y=mob->pos.y + dy, .x=mob->pos.x + dx});
        if(r){
          if(ALREADY_DRAWN != room_draw(*r)){
            point_t upper_left_corner = (point_t){.x=r->pos.x, .y=r->pos.y};
            point_t lower_right_corner = (point_t){.x=r->pos.x + r->width - 1, .y=r->pos.y + r->height - 1};
            for(mob_t *m = mob_get_mobs(); m; m = m->next){
              if(m->pos.x > upper_left_corner.x && m->pos.x < lower_right_corner.x &&
                m->pos.y > upper_left_corner.y && m->pos.y < lower_right_corner.y) m->stands_on = ROOM_FLOOR;
            }
            for(item_t *i = item_get_list(); i; i = i->next){
              if(i->pos.x > upper_left_corner.x && i->pos.x < lower_right_corner.x &&
                i->pos.y > upper_left_corner.y && i->pos.y < lower_right_corner.y) i->stands_on = ROOM_FLOOR;
            } 
          }
          mob_move_by(mob, dx, dy);
        }
        //else nidebug("Could not find room to enter/draw!");
      }
      break;

    case ID_DRAUGR:
    case ID_GOBLIN:
      if(mob == player){
        for(mob_t *hostile_mob = mob_get_mobs(); hostile_mob; hostile_mob = hostile_mob->next){
          if(hostile_mob->pos.x == (dx + player->pos.x) && hostile_mob->pos.y == (dy + player->pos.y)){
            attack(hostile_mob);
            event_log_add("You chopped a piece out of *the mob*");
            break;
          }
        }
      }
      break;

    case ITEM_SYMBOL:
      if(mob == player){
        for(item_t* it = item_get_list(); it; it = it->next){
          if(it->pos.x == (dx + player->pos.x) && it->pos.y == ((dy + player->pos.y))){
						place_into_inventory(player, it);
            mob_move_by(player, dx, dy);
            break;
          }
        }
      }
      break;

    default:
      nidebug("Unknown object ahead:[%c] in %s:%d", obj_ahead, __FILE__, __LINE__);
  }

  if(mob == player && player->stands_on == ROOM_DOOR){
    point_t new_pos = (point_t){.x = player->pos.x + dx, .y = player->pos.y + dy}; /* *2 is necessary because when we *would* arrive at a door we also want to check further for corridor as well */
    corridor_t *c = room_find_corridor_with_player(new_pos);
    if(c) room_draw_corridor_piece(c, new_pos);
  }
}


mob_t *mob_summon(const mob_id_t id)
{
  mob_t *summoned_creature = malloc(sizeof(mob_t));
  if(summoned_creature == NULL){
    nidebug("Could not summon creature in %s:%d", __FILE__, __LINE__);
    exit(1);
  }

  switch(id)
  {
    case ID_PLAYER:
      player = summoned_creature;
      summon_player(summoned_creature);
      break;
    case ID_GOBLIN:
      summon_goblin(summoned_creature);
      break;
    case ID_DRAUGR:
      summon_draugr(summoned_creature);
      break;
  
    default:
      nidebug("Unknown creature id: %c", id);
      free(summoned_creature);
      return NULL;
  }

  if(summoned_creature != NULL && head) add_to_list(summoned_creature);
  else{
    head = summoned_creature;
    head->next = NULL;
  }

	memset(&summoned_creature->inventory, 0, INVENTORY_WIDTH*INVENTORY_HEIGHT*sizeof(summoned_creature->inventory[0]));

  return summoned_creature;
}


static void add_to_list(mob_t *mob)
{
  mob_t *mobs = mob_get_mobs();
  for(; mobs->next; mobs = mobs->next);
  mobs->next = mob;
  mob->next = NULL;
}


void mob_update(mob_t *mob, input_code_t step_to)
{
  if(mob == player){
    mob_handle_movement(mob, step_to);
    mob_show(*mob);
  }
  else{
    int32_t dx = mob->pos.x - player->pos.x;
    int32_t dy = mob->pos.y - player->pos.y;

    if( (1*1 + 1*1) < (dx*dx + dy*dy) ){  /* Is mob near the player? */
      if(is_object_in_eyesight(mob->pos, player->pos)){
        if(abs(dx) > abs(dy)) (dx > 0) ? mob_handle_movement(mob, STEP_LEFT) : mob_handle_movement(mob, STEP_RIGHT);
        else (dy > 0) ? mob_handle_movement(mob, STEP_UP) : mob_handle_movement(mob, STEP_DOWN);
        mob_show(*mob);
        mob->last_seen = player->pos;
      }
      else{
        move_towards_last_seen_pos(mob);
        mob_hide(*mob);
      }
    }
    else{
      mob_show(*mob);
      attack(player);
      g_input_source = USER_INPUT;
      event_log_add("*the mob* cut you badly!");
    }
  }
}


static void move_towards_last_seen_pos(mob_t *mob)
{
  if(mob->last_seen.x != 0 && mob->last_seen.y != 0){ /* the player was spotted at least once */
    int16_t dx = mob->pos.x - mob->last_seen.x;
    int16_t dy = mob->pos.y - mob->last_seen.y;

    if(dx || dy){
      if(abs(dx) > abs(dy)) (dx > 0) ? mob_handle_movement(mob, STEP_LEFT) : mob_handle_movement(mob, STEP_RIGHT);
      else (dy > 0) ? mob_handle_movement(mob, STEP_UP) : mob_handle_movement(mob, STEP_DOWN);
    }
  }
}


static point_t get_random_pos(void)
{
  const room_t *room;
  uint16_t selected_room, tries;
  point_t mobp;
  if(room_get_rooms() == NULL || room_get_room_count() == 0) return (point_t){.x=0, .y=0};
  else{
    for(tries = 10; tries; --tries){
      selected_room = CALC_RAND(room_get_room_count()-1, 1); // room[0] is where the player starts, it shall be safe originally, thats why it is [numofroom:1]
      room = room_get_rooms() + selected_room;

      mobp.x = room->pos.x;
      mobp.x += CALC_RAND(room->width - 2, 1);

      mobp.y = room->pos.y;
      mobp.y += CALC_RAND(room->height - 2, 1);

      for(mob_t *currmob = mob_get_mobs(); currmob; currmob = currmob->next){
        if(currmob->pos.x != mobp.x || currmob->pos.y != mobp.y) goto found_place; /* own pos check unnecessary, not in the mob list yet */
      }
    }

found_place:
    if(tries) return mobp;
    else{
      nidebug("Could not find a place to summon mob."); /* NOTE: handle it more elegantly */
      exit(1);
    }    
  }
}

    /* -------- MOB SUMMONING -------- */

static void summon_player(mob_t *player)
{
  static bool summoned = false;
  room_t *rooms = room_get_rooms();
  if(summoned == false){
    *player = (mob_t){.pos.x=rooms[STARTING].pos.x+1, .pos.y=rooms[STARTING].pos.y+1, .stands_on=ROOM_FLOOR, .symbol=ID_PLAYER, .health=100, .level=1, .next=NULL,
                      .gear.lhand = NULL, .gear.rhand = NULL, .gear.armor = item_spawn(I_armor) };
    player->gear.armor->pos.x = player->gear.armor->pos.y = 0;
    summoned = true;
  }
  else{
    nidebug("Player could not be summoned after it was already\n");
    free(player);
    player = NULL;
  }
}

static void summon_goblin(mob_t *goblin)
{
  *goblin = (mob_t){.pos=get_random_pos(), .stands_on=EMPTY_SPACE, .symbol=ID_GOBLIN, .health=15, .level=1, .next=NULL, .last_seen=(point_t){.x=0, .y=0},
                    .gear.lhand = NULL, .gear.rhand = NULL, .gear.armor = item_spawn(I_armor) };
  goblin->gear.armor->pos.x = goblin->gear.armor->pos.y = 0;
}

static void summon_draugr(mob_t *draugr)
{
  *draugr = (mob_t){.pos=get_random_pos(), .stands_on=EMPTY_SPACE, .symbol=ID_DRAUGR, .health=20, .level=10, .next=NULL, .last_seen=(point_t){.x=0, .y=0},
                    .gear.lhand = NULL, .gear.rhand = NULL, .gear.armor = item_spawn(I_armor) };
  draugr->gear.armor->pos.x = draugr->gear.armor->pos.y = 0;
}
