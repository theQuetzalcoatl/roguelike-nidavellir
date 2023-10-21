#include <stdlib.h>
#include <math.h>
#include "room.h"
#include "display.h"
#include "cutscenes.h"
#include "corridor.h"
#include "terminal.h"
#include "debug.h"

#define MIN_ROOM_HEIGHT (5u)
#define MIN_ROOM_WIDTH  (5u)

#define MAX_NUM_OF_ROOMS_PER_LEVEL (9u)
#define MIN_NUM_OF_ROOMS_PER_LEVEL (3u)
#define BORDER (1u)
#define CELL_BELOW (3u)
#define CELL_ABOVE (-3)
#define CELL_ON_THE_LEFT (-1)
#define CELL_ON_THE_RIGHT (1u)

#define HORIZONTAL (1u)
#define VERTICAL   (2u)

static uint8_t num_of_rooms = 0;
static room_t *rooms = NULL;

static uint8_t num_of_corridors = 0;
static corridor_t *corridors = NULL;


typedef struct
{
    pos_t pos;
    room_t *room;
}cell_t;


uint8_t room_get_num_of_rooms(void) { return num_of_rooms; }

room_t *room_get_rooms(void) { return rooms; }

uint8_t room_get_num_of_corridors(void) { return num_of_corridors; }

corridor_t *room_get_corridors(void) { return corridors; }


corridor_t *room_find_corridor(const pos_t player)
{ /* NOTE: store last successful hit for faster execution */
    corridor_t *c = room_get_corridors();

    /* is player in the square created by the two endpoints? */
    /* NOTE: what if two corridors cross each other? */
    for(int8_t n = room_get_num_of_corridors() - 1; n >= 0; --n){
        pos_t upper_left = (pos_t){.x = (c[n].line[0].p1.x > c[n].line[2].p2.x) ? c[n].line[2].p2.x : c[n].line[0].p1.x,
                                   .y = (c[n].line[0].p1.y > c[n].line[2].p2.y) ? c[n].line[2].p2.y : c[n].line[0].p1.y};
        pos_t lower_right = (pos_t){.x = (c[n].line[0].p1.x < c[n].line[2].p2.x) ? c[n].line[2].p2.x : c[n].line[0].p1.x,
                                   .y = (c[n].line[0].p1.y < c[n].line[2].p2.y) ? c[n].line[2].p2.y : c[n].line[0].p1.y};

        if(player.x >= upper_left.x && player.x <= lower_right.x &&
            player.y >= upper_left.y && player.y <= lower_right.y) return &c[n];
    }
    return NULL;
}

void room_draw_corridor_piece(const corridor_t *c, const pos_t player)
{
    int8_t on_line = 2;

    for(; on_line >= 0; --on_line){
        pos_t p = c->line[on_line].p1;
        if(c->line[on_line].is_vertical){
            for(; p.y != c->line[on_line].p2.y + c->line[on_line].direction; p.y += c->line[on_line].direction){
                if(3*3 >= ((p.x - player.x)*(p.x - player.x) + (p.y - player.y)*(p.y - player.y))) term_putchar_xy(CORRIDOR_FLOOR, p.x, p.y); /* place corridor of distance is smaller than or equal to 3 */
            }
        }
        else{
            for(; p.x != c->line[on_line].p2.x + c->line[on_line].direction; p.x += c->line[on_line].direction){
                if(3*3 >= ((p.x - player.x)*(p.x - player.x) + (p.y - player.y)*(p.y - player.y))) term_putchar_xy(CORRIDOR_FLOOR, p.x, p.y); /* place corridor of distance is smaller than or equal to 3 */
            }
        }
    }
}

static void make_corridor(pos_t starting, const pos_t ending, const uint8_t initial_orientation)
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

    newest_corridor.line[0].p1 = starting;
    newest_corridor.line[2].p2 = ending;

    if(initial_orientation == VERTICAL){
        uint8_t dy = abs(starting.y - ending.y);
        uint8_t turn_at = (dy > 1 ) ? CALC_RAND(dy - 1, 1) : 1;

        newest_corridor.line[0].is_vertical = newest_corridor.line[2].is_vertical = true;
        newest_corridor.line[1].is_vertical = false;

        newest_corridor.line[0].direction = newest_corridor.line[2].direction = y_dir;
        newest_corridor.line[1].direction = x_dir;

        newest_corridor.line[0].p2 = (pos_t){.x = starting.x, .y = starting.y + turn_at*y_dir};
        newest_corridor.line[1].p1 = newest_corridor.line[0].p2;
        newest_corridor.line[1].p2 = (pos_t){.x = ending.x, .y = newest_corridor.line[0].p2.y};
        newest_corridor.line[2].p1 = newest_corridor.line[1].p2;
    }
    else if(initial_orientation == HORIZONTAL){
        uint8_t dx = abs(starting.x - ending.x);
        uint8_t turn_at = (dx > 1) ? CALC_RAND(dx - 1 , 1) : 1;

        newest_corridor.line[0].is_vertical = newest_corridor.line[2].is_vertical = false;
        newest_corridor.line[1].is_vertical = true;

        newest_corridor.line[0].direction = newest_corridor.line[2].direction = x_dir;
        newest_corridor.line[1].direction = y_dir;

        newest_corridor.line[0].p2 = (pos_t){.x = starting.x + turn_at*x_dir, .y = starting.y};
        newest_corridor.line[1].p1 = newest_corridor.line[0].p2;
        newest_corridor.line[1].p2 = (pos_t){.x = newest_corridor.line[0].p2.x, .y = ending.y};
        newest_corridor.line[2].p1 = newest_corridor.line[1].p2;
    }
    else{
        nidebug("Invalid initial orientation received during corridor making!");
        exit(1);
    }
    corridors[num_of_corridors - 1] = newest_corridor;
}


room_t *room_create_rooms(void)
{
    uint8_t number_of_rooms = CALC_RAND(MAX_NUM_OF_ROOMS_PER_LEVEL, MIN_NUM_OF_ROOMS_PER_LEVEL);
    rooms = malloc(number_of_rooms*sizeof(room_t));

    if(rooms == NULL){ /* NOTE: should we decrease the room number until MIN in order to try avoiding a crash? */
        nidebug("Could not allocate enough memory for rooms in %s:%i.\n", __FILE__, __LINE__);
        exit(1);
    }

    // 3x3 grid with borders
    uint8_t cell_width = TERMINAL_WIDTH/3;
    uint8_t cell_height = RUNIC_LINE_POS/3;
    cell_t window_cell[MAX_NUM_OF_ROOMS_PER_LEVEL] = {
        [0] = {.pos.x = 0*cell_width + BORDER, .pos.y = 0*cell_height + BORDER, .room=NULL}, [1] = {.pos.x = 1*cell_width + BORDER, .pos.y = 0*cell_height + BORDER, .room=NULL}, [2] = {.pos.x = 2*cell_width + BORDER, .pos.y = 0*cell_height + BORDER, .room=NULL},
        [3] = {.pos.x = 0*cell_width + BORDER, .pos.y = 1*cell_height + BORDER, .room=NULL}, [4] = {.pos.x = 1*cell_width + BORDER, .pos.y = 1*cell_height + BORDER, .room=NULL}, [5] = {.pos.x = 2*cell_width + BORDER, .pos.y = 1*cell_height + BORDER, .room=NULL},
        [6] = {.pos.x = 0*cell_width + BORDER, .pos.y = 2*cell_height + BORDER, .room=NULL}, [7] = {.pos.x = 1*cell_width + BORDER, .pos.y = 2*cell_height + BORDER, .room=NULL}, [8] = {.pos.x = 2*cell_width + BORDER, .pos.y = 2*cell_height + BORDER, .room=NULL}
                                                    };

    uint16_t mixed_cell_numbers[MAX_NUM_OF_ROOMS_PER_LEVEL] = {0,1,2,3,4,5,6,7,8};
    stir_elements_randomly(sizeof(mixed_cell_numbers)/sizeof(mixed_cell_numbers[0]), mixed_cell_numbers);

    /* place rooms */
    for(uint8_t n = 0; n < number_of_rooms; ++n){
        (rooms + n)->pos = window_cell[mixed_cell_numbers[n]].pos;

        window_cell[mixed_cell_numbers[n]].room = rooms + n;
        (rooms + n)->in_cell = mixed_cell_numbers[n];

        uint64_t x = CALC_RAND((cell_width - 2*BORDER) - MIN_ROOM_WIDTH, 0);
        rooms[n].pos.x += x;
        uint64_t y = CALC_RAND((cell_height - 2*BORDER) - MIN_ROOM_HEIGHT, 0);
        rooms[n].pos.y += y;

        uint64_t available_width = (cell_width - 2*BORDER) - x;
        uint64_t available_height = (cell_height - 2*BORDER) - y;

        rooms[n].width = CALC_RAND(available_width, MIN_ROOM_WIDTH);
        rooms[n].height = CALC_RAND(available_height, MIN_ROOM_HEIGHT);
    }

    /* door+corridor generation */
    for(uint8_t n = 0; n < number_of_rooms; ++n){
        int8_t room_cell = (rooms + n)->in_cell;
        int8_t other_cell = room_cell + CELL_ABOVE;

        if((other_cell) >= 0){
            room_t *other_room = window_cell[other_cell].room;
            if(other_room != NULL && other_room->lower_door.pos.x == 0 && other_room->lower_door.pos.y == 0){
                pos_t starting = {.y = rooms[n].pos.y-1, .x = CALC_RAND(rooms[n].width - 2, 1) + rooms[n].pos.x};
                pos_t ending = {.y = other_room->pos.y + other_room->height, .x = CALC_RAND(other_room->width - 2, 1) + other_room->pos.x};

                make_corridor(starting, ending, VERTICAL);

                rooms[n].upper_door.pos = (pos_t){.y = starting.y + 1, .x = starting.x};
                other_room->lower_door.pos = (pos_t){.y = ending.y - 1, .x = ending.x};
            }
        }

        other_cell = room_cell + CELL_BELOW;

        if((other_cell) <= 8){
            room_t *other_room = window_cell[other_cell].room;
            if(other_room != NULL && other_room->upper_door.pos.x == 0 && other_room->upper_door.pos.y == 0){
                pos_t starting = {.y = rooms[n].pos.y + rooms[n].height, .x = CALC_RAND(rooms[n].width - 2, 1) + rooms[n].pos.x};
                pos_t ending = {.y = other_room->pos.y-1, .x = CALC_RAND(other_room->width - 2, 1) + other_room->pos.x};

                make_corridor(starting, ending, VERTICAL);

                rooms[n].lower_door.pos = (pos_t){.y = starting.y - 1, .x = starting.x};
                other_room->upper_door.pos = (pos_t){.y = ending.y + 1, .x = ending.x};
            }
        }

        other_cell = room_cell + CELL_ON_THE_LEFT;

        if((other_cell) != 2 && (other_cell) != 5 && (other_cell) >= 0){
            room_t *other_room = window_cell[other_cell].room;
            if(other_room != NULL && other_room->right_door.pos.x == 0 && other_room->right_door.pos.y == 0){
                pos_t starting = {.y = rooms[n].pos.y + CALC_RAND(rooms[n].height - 2, 1), .x = rooms[n].pos.x - 1};
                pos_t ending = {.y = other_room->pos.y + CALC_RAND(other_room->height - 2, 1), .x = other_room->pos.x + other_room->width}; /* +1 is incorporated in width addition to x pos */

                make_corridor(starting, ending, HORIZONTAL);

                rooms[n].left_door.pos = (pos_t){.y = starting.y, .x = starting.x + 1};
                other_room->right_door.pos = (pos_t){.y = ending.y, .x = ending.x - 1};
            }
        }

        other_cell = room_cell + CELL_ON_THE_RIGHT;

        if((other_cell) != 6 && (other_cell) != 3 && (other_cell) <= 8){
            room_t *other_room = window_cell[other_cell].room;
            if(other_room != NULL && other_room->left_door.pos.x == 0 && other_room->left_door.pos.y == 0){
                pos_t starting = {.y = rooms[n].pos.y + CALC_RAND(rooms[n].height - 2, 1), .x = rooms[n].pos.x + rooms[n].width}; /* +1 is incorporated in width addition to x pos */
                pos_t ending = {.y = other_room->pos.y + CALC_RAND(other_room->height - 2, 1), .x = other_room->pos.x - 1};

                make_corridor(starting, ending, HORIZONTAL);

                rooms[n].right_door.pos = (pos_t){.y = starting.y, .x = starting.x - 1};
                other_room->left_door.pos = (pos_t){.y = ending.y, .x = ending.x + 1};
            }
        }
    }

    num_of_rooms = number_of_rooms;
    return rooms;
}


room_t *room_find(const pos_t p)
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
        if(r.upper_door.pos.x != 0 && r.upper_door.pos.y != 0) term_putchar_xy(ROOM_DOOR, r.upper_door.pos.x, r.upper_door.pos.y);
        if(r.lower_door.pos.x != 0 && r.lower_door.pos.y != 0) term_putchar_xy(ROOM_DOOR, r.lower_door.pos.x, r.lower_door.pos.y);
        if(r.left_door.pos.x != 0 && r.left_door.pos.y != 0) term_putchar_xy(ROOM_DOOR, r.left_door.pos.x, r.left_door.pos.y);
        if(r.right_door.pos.x != 0 && r.right_door.pos.y != 0) term_putchar_xy(ROOM_DOOR, r.right_door.pos.x, r.right_door.pos.y);

        return 0;
    }
    else return ALREADY_DRAWN;
}