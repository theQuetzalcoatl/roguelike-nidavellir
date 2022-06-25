#include <stdint.h>
#include "../include/terminal.h"
#include "../include/creatures.h"



void creature_move_abs(creature_t *creature, pos_t pos)
{
    term_move_cursor(creature->pos.x, creature->pos.y);
    term_putchar(creature->stands_on);
    term_move_cursor(pos.x, pos.y);
    creature->stands_on = term_getchar();
    term_putchar(creature->symbol);
    creature->pos.x = pos.x;
    creature->pos.y = pos.y;
}

void creature_move_rel(creature_t *creature, pos_t rel_pos)
{
    term_move_cursor(creature->pos.x, creature->pos.y);
    term_putchar(creature->stands_on);
    term_move_cursor(creature->pos.x + rel_pos.x, creature->pos.y + rel_pos.y);
    creature->stands_on = term_getchar();
    term_putchar(creature->symbol);
    creature->pos.x += rel_pos.x;
    creature->pos.y += rel_pos.y;
}