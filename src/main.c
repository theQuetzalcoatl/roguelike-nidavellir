#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "terminal.h"
#include "cutscenes.h"
#include "mob.h"
#include "room.h"
#include "corridor.h"
#include "input.h"
#include "display.h"
#include "debug.h"
#include "item.h"

extern void event_log_add(const char *event);
extern uint64_t event_get_entry_num(void);
bool g_game_is_running = true;
bool g_input_source = USER_INPUT; /* TODO: make this mechanism prettier  */


static void handle_ctrl_c(int unused)
{
  unused = unused; // avoid  compiler warning
  exit(1); // run the 'atexit' functions
}


void draw(void)
{
  fflush(stdout);
}

static void check_window_size(void)
{
  struct winsize ws = {0};
  ioctl(0, TIOCGWINSZ, &ws);

  if(ws.ws_col < TERMINAL_WIDTH || ws.ws_row < TERMINAL_HEIGHT){
    printf("Terminal window is currently %dx%d. Make it at least %dx%d large and try again!\n", ws.ws_col, ws.ws_row, TERMINAL_WIDTH, TERMINAL_HEIGHT);
    exit(1);
  }
}

//   /usr/share/fonts/truetype/freefont/FreeMono.ttf

int main(void)
{
  mob_t *player = NULL;
  room_t *r = NULL;
  uint64_t turns = 0, prev_event_num = 0;
  input_code_t input = NO_ARROW;
  bool objects_displayed= false;
  uint64_t rested_turns = 0;

  check_window_size();
  signal(SIGINT, handle_ctrl_c);
  term_setup();

  srand(time(NULL)); /* RNG init */
  debug_init();

  void (*exit_funcs[])(void) = {term_restore_original, debug_deinit, mob_free_mobs, item_free_items};
  for(uint32_t i = 0; i < ELEMENT_COUNT(exit_funcs); ++i) atexit(exit_funcs[i]);

  cutscene_intro();
  r = room_create_rooms();
  room_draw(r[STARTING]);

  int summoned_mobs[] = {ID_PLAYER, ID_DRAUGR, ID_DRAUGR, ID_GOBLIN, ID_GOBLIN, ID_DRAUGR};
  for(uint32_t i = 0; i < ELEMENT_COUNT(summoned_mobs); ++i) mob_summon(summoned_mobs[i]); /* player should be summoned first to be updated first, otherwise some mobs will be before him, and they see him at a previous point in time */
  for(int i = 20; i; --i) item_spawn(CALC_RAND(ITEMS_COUNT-1, I_potion));

  player = mob_get_player();
  display_runic_lines();
  display_player_stats(*player, turns);

  mob_show(*player);
  for(item_t *it = item_get_list(); it; it = it->next) is_object_in_eyesight(it->pos, player->pos) ? item_show(*it) : item_hide(*it);

  draw();
  fflush(stdin);

  /* let the game begin... */

  while(g_game_is_running){
    input = (g_input_source == USER_INPUT) ? get_keypress() : '.';

    if(objects_displayed == true){ /* TODO: delete this  */
      for (uint16_t row = 0; row < RUNIC_LINE_POS; ++row){
        for (uint16_t col = 0; col < TERMINAL_WIDTH - 2; ++col) term_putchar_xy(term_getchar_xy(col, row), col, row);
      }
      objects_displayed = false;
    }

    switch(input)
    {
      case ARROW_UP:
      case ARROW_LEFT:
      case ARROW_DOWN:
      case ARROW_RIGHT:
        break;
      case 'Q':
        g_game_is_running = false;
        continue;
      case '.': 
        ++rested_turns;
        if(rested_turns % 10 == 0 ){ /* NOTE: refactor */
          player->health += 5u;
          player->health -= (player->health*(player->health/100) % PLAYER_MAX_HEALTH); /* capping it to max unreasonably complicatedly */
          if(player->health == PLAYER_MAX_HEALTH) g_input_source = USER_INPUT;
        }
        break;
      case ':':
        g_input_source = !USER_INPUT;
        break;
      case '?':
        display_to_player_window("hotkeys");
        continue;
      case 'E':
        display_to_player_window("events");
        continue;
      case 'D':
        debug_display_object_stats(room_get_rooms(), item_get_list(), mob_get_mobs());
        objects_displayed= true;
        continue;
			case 'i':
			case 'd':
        uint8_t action = (input == 'i') ? USE_ITEM : DROP_ITEM;
        mob_open_player_inventory(action);
        break;
      default:
        continue;
    }

    for(mob_t *mob = mob_get_mobs(); mob; mob = mob->next) mob_update(mob, input);
    for(item_t *it = item_get_list(); it; it = it->next) is_object_in_eyesight(it->pos, player->pos) ? item_show(*it) : item_hide(*it);
		mob_show(*player); /* NOTE: needs to be done, when the player is out of inv. space and approaches an item, the drawing sequence
													is not the best, resulting in the player disappearing. it temporarily fixes it */

    display_player_stats(*player, turns);
    if(prev_event_num < event_get_entry_num()){
      prev_event_num = event_get_entry_num();
      display_recent_events(); 
    } 

    draw();
    ++turns;
  }

  /* CLEAN UP */
  if(!player->health) cutscene_dead();
  free(r);
  free(room_get_corridors());
  /* NOTE: free mobs as well  */

  return 0;
}
