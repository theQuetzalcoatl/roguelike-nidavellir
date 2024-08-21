#include <stdlib.h>
#include <math.h>
#include "room.h"
#include "display.h"
#include "cutscenes.h"
#include "corridor.h"
#include "terminal.h"
#include "debug.h"

#define MIN_ROOM_HEIGHT (5u)
#define MAX_ROOM_HEIGHT (7u)
#define MIN_ROOM_WIDTH  (5u)
#define MAX_ROOM_WIDTH  (18u)

#define MAX_ROOMS_PER_LEVEL (9u)
#define MIN_ROOMS_PER_LEVEL (3u)
#define BORDER (1u)
#define CELL_BELOW(cell) (cell + 3u)
#define CELL_ABOVE(cell) (cell - 3) /* quick and dirty. Because there are 3 cells in each row, going back 3 and we will end up at a cell above us */
#define CELL_ON_THE_LEFT(cell) (cell - 1)
#define CELL_ON_THE_RIGHT(cell) (cell + 1u)
#define DOOR_EXISTS(door) (door.pos.x != 0 && door.pos.y != 0)

#define HORIZONTAL (0u)
#define VERTICAL   (1u)

static uint8_t num_of_rooms = 0;
static room_t *rooms = NULL;

static uint8_t num_of_corridors = 0;
static corridor_t *corridors = NULL;


typedef struct
{
  point_t pos;
  room_t *room;
}cell_t;


uint8_t room_get_num_of_rooms(void) { return num_of_rooms; }
room_t *room_get_rooms(void) { return rooms; }
uint8_t room_get_num_of_corridors(void) { return num_of_corridors; }
corridor_t *room_get_corridors(void) { return corridors; }


corridor_t *room_find_corridor_with_player(const point_t player)
{ /* NOTE: refactor!!! */
  static corridor_t *last_found = NULL;

  if(last_found){
    point_t upper_left = (point_t){.x = (last_found->line[0].start.x > last_found->line[2].end.x) ? last_found->line[2].end.x : last_found->line[0].start.x,
                                .y = (last_found->line[0].start.y > last_found->line[2].end.y) ? last_found->line[2].end.y : last_found->line[0].start.y};
    point_t lower_right = (point_t){.x = (last_found->line[0].start.x < last_found->line[2].end.x) ? last_found->line[2].end.x : last_found->line[0].start.x,
                                .y = (last_found->line[0].start.y < last_found->line[2].end.y) ? last_found->line[2].end.y : last_found->line[0].start.y};
    if(player.x >= upper_left.x && player.x <= lower_right.x &&
        player.y >= upper_left.y && player.y <= lower_right.y) return last_found;
  }

  corridor_t *c = room_get_corridors();

  /* NOTE: what if two corridors cross each other? */
  /* is player in the square created by the two endpoints? */
  for(int8_t n = room_get_num_of_corridors() - 1; n >= 0; --n){
    point_t upper_left = (point_t){.x = (c[n].line[0].start.x > c[n].line[2].end.x) ? c[n].line[2].end.x : c[n].line[0].start.x,
                                .y = (c[n].line[0].start.y > c[n].line[2].end.y) ? c[n].line[2].end.y : c[n].line[0].start.y};
    point_t lower_right = (point_t){.x = (c[n].line[0].start.x < c[n].line[2].end.x) ? c[n].line[2].end.x : c[n].line[0].start.x,
                                .y = (c[n].line[0].start.y < c[n].line[2].end.y) ? c[n].line[2].end.y : c[n].line[0].start.y};

    if(player.x >= upper_left.x && player.x <= lower_right.x &&
        player.y >= upper_left.y && player.y <= lower_right.y){
          last_found = &c[n];
          return &c[n];
    }
  }
  return NULL;
}


void room_draw_corridor_piece(const corridor_t *c, const point_t player)
{
  for(int8_t checked_line = 2; checked_line >= 0; --checked_line){
    point_t point = c->line[checked_line].start;
    if(c->line[checked_line].is_vertical){
      for(; point.y != c->line[checked_line].end.y + c->line[checked_line].direction; point.y += c->line[checked_line].direction){
        if(3*3 >= ((point.x - player.x)*(point.x - player.x) + (point.y - player.y)*(point.y - player.y))) {
          term_putchar_xy(CORRIDOR, point.x, point.y); /* place corridor if distance is smaller than or equal to 3 */
        }
      }
    }
    else{
      for(; point.x != c->line[checked_line].end.x + c->line[checked_line].direction; point.x += c->line[checked_line].direction){
        if(3*3 >= ((point.x - player.x)*(point.x - player.x) + (point.y - player.y)*(point.y - player.y))) {
          term_putchar_xy(CORRIDOR, point.x, point.y); /* place corridor if distance is smaller than or equal to 3 */
        }
      }
    }
  }
}

static void make_corridor(const point_t starting, const point_t ending, const uint8_t initial_orientation)
{
  corridors = realloc(corridors, (num_of_corridors + 1)*(sizeof(corridor_t)));
  if(!corridors){
    nidebug("Could not allocate memory for another corridor! Corridor num:%d", num_of_corridors);
    exit(1);
  }
  else ++num_of_corridors;

  int8_t y_dir = (starting.y > ending.y) ? -1 : 1;
  int8_t x_dir = (starting.x > ending.x) ? -1 : 1;
  corridor_t newest_corridor = {0};

  newest_corridor.line[0].start = starting;
  newest_corridor.line[2].end = ending;
  newest_corridor.line[0].is_vertical = newest_corridor.line[2].is_vertical = initial_orientation;
  newest_corridor.line[1].is_vertical = !newest_corridor.line[0].is_vertical;

  if(initial_orientation == VERTICAL){
    uint8_t dy = abs(starting.y - ending.y);
    uint8_t turn_at = (dy > 1 ) ? CALC_RAND(dy - 1, 1) : 1;

    newest_corridor.line[0].direction = newest_corridor.line[2].direction = y_dir;
    newest_corridor.line[1].direction = x_dir;

    newest_corridor.line[0].end = (point_t){.x = starting.x, .y = starting.y + turn_at*y_dir};
    newest_corridor.line[1].start = newest_corridor.line[0].end;
    newest_corridor.line[1].end = (point_t){.x = ending.x, .y = newest_corridor.line[0].end.y};
    newest_corridor.line[2].start = newest_corridor.line[1].end;
  }
  else if(initial_orientation == HORIZONTAL){
    uint8_t dx = abs(starting.x - ending.x);
    uint8_t turn_at = (dx > 1) ? CALC_RAND(dx - 1 , 1) : 1;

    newest_corridor.line[0].direction = newest_corridor.line[2].direction = x_dir;
    newest_corridor.line[1].direction = y_dir;

    newest_corridor.line[0].end = (point_t){.x = starting.x + turn_at*x_dir, .y = starting.y};
    newest_corridor.line[1].start = newest_corridor.line[0].end;
    newest_corridor.line[1].end = (point_t){.x = newest_corridor.line[0].end.x, .y = ending.y};
    newest_corridor.line[2].start = newest_corridor.line[1].end; /* NOTE: this could be out of the if clauses */
  }
  else{
    nidebug("Invalid initial orientation received during corridor making!");
    exit(1);
  }
  corridors[num_of_corridors - 1] = newest_corridor;
}

static int get_neighbouring_empty_cell(cell_t *cells, uint8_t prev_cell_index)
{
  uint16_t directions[4] = {0,1,2,3};
  stir_elements_randomly(sizeof(directions)/sizeof(directions[0]), directions);

  for(uint8_t tries = sizeof(directions)/sizeof(directions[0]); tries; --tries){
    switch(directions[tries])
    {
      case 0: /* UP */
        if(CELL_ABOVE(prev_cell_index) >= 0 && cells[CELL_ABOVE(prev_cell_index)].room == NULL) return CELL_ABOVE(prev_cell_index); 
      break;
      case 1: /* DOWN */
        if(CELL_BELOW(prev_cell_index) <= 8 && cells[CELL_BELOW(prev_cell_index)].room == NULL) return CELL_BELOW(prev_cell_index); 
      break;
      case 2: /* LEFT */
        if(CELL_ON_THE_LEFT(prev_cell_index) >= 0 && cells[CELL_ON_THE_LEFT(prev_cell_index)].room == NULL) return CELL_ON_THE_LEFT(prev_cell_index); 
      break;
      case 3: /* RIGHT */
        if(CELL_ON_THE_RIGHT(prev_cell_index) <= 8 && cells[CELL_ON_THE_RIGHT(prev_cell_index)].room == NULL) return CELL_ON_THE_RIGHT(prev_cell_index); 
      break;
      default: break;
    }
  }
  /* edge case where the prev cell was at a corner and we cant get out */
  for(int8_t cell_i = 8; cell_i >= 0; --cell_i) if(cells[cell_i].room == NULL) return cell_i;

  return 0; /* pleasing the compiler, most likely wont end up here */ 
}

room_t *room_create_rooms(void)
{
  num_of_rooms = CALC_RAND(MAX_ROOMS_PER_LEVEL, MIN_ROOMS_PER_LEVEL);
  rooms = malloc(num_of_rooms*sizeof(room_t));

  if(rooms == NULL){ /* NOTE: should we decrease the room number until MIN in order to try avoiding a crash? */
    nidebug("Could not allocate enough memory for rooms in %s:%i.\n", __FILE__, __LINE__);
    exit(1);
  }

  // 3x3 grid with borders
  uint8_t cell_width = TERMINAL_WIDTH/3;
  uint8_t cell_height = RUNIC_LINE_POS/3;
  cell_t window_cell[MAX_ROOMS_PER_LEVEL] = {
      [0] = {.pos.x = 0*cell_width + BORDER, .pos.y = 0*cell_height + BORDER, .room=NULL}, [1] = {.pos.x = 1*cell_width + BORDER, .pos.y = 0*cell_height + BORDER, .room=NULL}, [2] = {.pos.x = 2*cell_width + BORDER, .pos.y = 0*cell_height + BORDER, .room=NULL},
      [3] = {.pos.x = 0*cell_width + BORDER, .pos.y = 1*cell_height + BORDER, .room=NULL}, [4] = {.pos.x = 1*cell_width + BORDER, .pos.y = 1*cell_height + BORDER, .room=NULL}, [5] = {.pos.x = 2*cell_width + BORDER, .pos.y = 1*cell_height + BORDER, .room=NULL},
      [6] = {.pos.x = 0*cell_width + BORDER, .pos.y = 2*cell_height + BORDER, .room=NULL}, [7] = {.pos.x = 1*cell_width + BORDER, .pos.y = 2*cell_height + BORDER, .room=NULL}, [8] = {.pos.x = 2*cell_width + BORDER, .pos.y = 2*cell_height + BORDER, .room=NULL}
                                                  };
  for(uint8_t n = 0; n < num_of_rooms; ++n){
    if(n == 0){
      int starting_cell_index = CALC_RAND(MAX_ROOMS_PER_LEVEL-1, 0);
      rooms[n].pos = window_cell[starting_cell_index].pos;
      window_cell[starting_cell_index].room = rooms + n;
      rooms[n].in_cell = starting_cell_index;
    }
    else{
      int prev_cell_index = rooms[n-1].in_cell;
      int cell_index = get_neighbouring_empty_cell(window_cell, prev_cell_index);
      rooms[n].pos = window_cell[cell_index].pos;
      window_cell[cell_index].room = rooms + n;
      rooms[n].in_cell = cell_index;
    }
    rooms[n].width = CALC_RAND(MAX_ROOM_WIDTH, MIN_ROOM_WIDTH);
    rooms[n].height = CALC_RAND(MAX_ROOM_HEIGHT, MIN_ROOM_HEIGHT);

    rooms[n].pos.x += CALC_RAND(cell_width - rooms[n].width - BORDER , 0);
    rooms[n].pos.y += CALC_RAND(cell_height - rooms[n].height - BORDER, 0);
  }

  /* door+corridor generation */
  for(uint8_t n = 0; n < num_of_rooms; ++n){
    int8_t cell_index = (rooms + n)->in_cell;

    int8_t adjacent_cell = CELL_ABOVE(cell_index);

    if((adjacent_cell) >= 0){
      room_t *other_room = window_cell[adjacent_cell].room;
      if(other_room != NULL && other_room->lower_door.pos.x == 0 && other_room->lower_door.pos.y == 0){
        point_t starting = {.y = rooms[n].pos.y-1, .x = CALC_RAND(rooms[n].width - 2, 1) + rooms[n].pos.x};
        point_t ending = {.y = other_room->pos.y + other_room->height, .x = CALC_RAND(other_room->width - 2, 1) + other_room->pos.x};

        make_corridor(starting, ending, VERTICAL);

        rooms[n].upper_door.pos = (point_t){.y = starting.y + 1, .x = starting.x};
        other_room->lower_door.pos = (point_t){.y = ending.y - 1, .x = ending.x};
      }
    }

    adjacent_cell = CELL_BELOW(cell_index);

    if((adjacent_cell) <= 8){
      room_t *other_room = window_cell[adjacent_cell].room;
      if(other_room != NULL && other_room->upper_door.pos.x == 0 && other_room->upper_door.pos.y == 0){
        point_t starting = {.y = rooms[n].pos.y + rooms[n].height, .x = CALC_RAND(rooms[n].width - 2, 1) + rooms[n].pos.x};
        point_t ending = {.y = other_room->pos.y-1, .x = CALC_RAND(other_room->width - 2, 1) + other_room->pos.x};

        make_corridor(starting, ending, VERTICAL);

        rooms[n].lower_door.pos = (point_t){.y = starting.y - 1, .x = starting.x};
        other_room->upper_door.pos = (point_t){.y = ending.y + 1, .x = ending.x};
      }
    }

    adjacent_cell = CELL_ON_THE_LEFT(cell_index);

    if((adjacent_cell) != 2 && (adjacent_cell) != 5 && (adjacent_cell) >= 0){
      room_t *other_room = window_cell[adjacent_cell].room;
      if(other_room != NULL && other_room->right_door.pos.x == 0 && other_room->right_door.pos.y == 0){
        point_t starting = {.y = rooms[n].pos.y + CALC_RAND(rooms[n].height - 2, 1), .x = rooms[n].pos.x - 1};
        point_t ending = {.y = other_room->pos.y + CALC_RAND(other_room->height - 2, 1), .x = other_room->pos.x + other_room->width}; /* +1 is incorporated in width addition to x pos */

        make_corridor(starting, ending, HORIZONTAL);

        rooms[n].left_door.pos = (point_t){.y = starting.y, .x = starting.x + 1};
        other_room->right_door.pos = (point_t){.y = ending.y, .x = ending.x - 1};
      }
    }

    adjacent_cell = CELL_ON_THE_RIGHT(cell_index);

    if((adjacent_cell) != 6 && (adjacent_cell) != 3 && (adjacent_cell) <= 8){
      room_t *other_room = window_cell[adjacent_cell].room;
      if(other_room != NULL && other_room->left_door.pos.x == 0 && other_room->left_door.pos.y == 0){
        point_t starting = {.y = rooms[n].pos.y + CALC_RAND(rooms[n].height - 2, 1), .x = rooms[n].pos.x + rooms[n].width}; /* +1 is incorporated in width addition to x pos */
        point_t ending = {.y = other_room->pos.y + CALC_RAND(other_room->height - 2, 1), .x = other_room->pos.x - 1};

        make_corridor(starting, ending, HORIZONTAL);

        rooms[n].right_door.pos = (point_t){.y = starting.y, .x = starting.x - 1};
        other_room->left_door.pos = (point_t){.y = ending.y, .x = ending.x + 1};
      }
    }
  }

  return rooms;
}


room_t *room_find(const point_t p)
{
  room_t *r = room_get_rooms();
  int8_t n = room_get_num_of_rooms() - 1;

  while(n){ /* 0 not included because that's the room where the player starts, therefore already drawn */
    if((r + n)->left_door.pos.x == p.x && (r + n)->left_door.pos.y == p.y) break;
    else if((r + n)->right_door.pos.x == p.x && (r + n)->right_door.pos.y == p.y) break;
    else if((r + n)->lower_door.pos.x == p.x && (r + n)->lower_door.pos.y == p.y) break; 
    else if((r + n)->upper_door.pos.x == p.x && (r + n)->upper_door.pos.y == p.y) break;
    --n;
  }
  return n ? r + n : NULL;
}


uint8_t room_draw(const room_t r)
{
  if(HORIZONTAL_WALL != term_getchar_xy(r.pos.x, r.pos.y)){ /* is it already drawn?  */
    /* upper wall */
    for(int x = r.pos.x; x < r.width + r.pos.x; ++x) term_putchar_xy(HORIZONTAL_WALL, x, r.pos.y);
    /* sidewalls and floor */
    for(int y = r.pos.y + 1; y < r.height + r.pos.y - 1; ++y){
      term_putchar_xy(VERTICAL_WALL, r.pos.x, y);
      term_putchar_xy(VERTICAL_WALL, r.pos.x + r.width - 1, y);
      for(int x = r.pos.x + 1; x < r.width + r.pos.x - 1; ++x) term_putchar_xy(ROOM_FLOOR, x, y);
    }
    /* lower wall */
    for(int x = r.pos.x; x < r.width + r.pos.x; ++x) term_putchar_xy(HORIZONTAL_WALL, x, r.pos.y + r.height - 1);
    /* placing doors */
    if(DOOR_EXISTS(r.upper_door)) term_putchar_xy(ROOM_DOOR, r.upper_door.pos.x, r.upper_door.pos.y);
    if(DOOR_EXISTS(r.lower_door)) term_putchar_xy(ROOM_DOOR, r.lower_door.pos.x, r.lower_door.pos.y);
    if(DOOR_EXISTS(r.left_door)) term_putchar_xy(ROOM_DOOR, r.left_door.pos.x, r.left_door.pos.y);
    if(DOOR_EXISTS(r.right_door)) term_putchar_xy(ROOM_DOOR, r.right_door.pos.x, r.right_door.pos.y);

    return 0;
  }
  else return ALREADY_DRAWN;
}
