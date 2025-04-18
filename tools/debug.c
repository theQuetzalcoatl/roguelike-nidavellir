#include "debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wformat-security"

static FILE *fp = NULL;


void debug_init(void)
{
    fp = fopen("debug.txt", "w");
    if(fp == NULL){
        printf("Could not open \"debug.txt\"!\n");
        exit(1);
    }
}


void debug_deinit(void)
{
    fclose(fp);
}


void nidebug(const char *s, ...)
{
    va_list args;
    time_t rawtime;
    struct tm *timeinfo;

    time( &rawtime );
    timeinfo = localtime( &rawtime );
    fprintf(fp, "\n[%d:%d:%d] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    va_start(args, s);
    vfprintf(fp, s, args);
    fflush(fp); // live debug file update
}


void debug_display_object_stats(const room_t *r, const item_t *i, const mob_t *m)
{
	if(r){
		for(int i = room_get_room_count() - 1; i >= 0; --i){
	    term_move_cursor(r[i].pos.x, r[i].pos.y - 1);
	    printf("R%i:%iR", r[i].pos.x, r[i].pos.y);
		}
  }

  for(; i; i = i->next){
    if(i->pos.x != 0 && i->pos.y != 0){
      term_move_cursor(i->pos.x, i->pos.y - 1);
      printf("i%i:%ii", i->pos.x, i->pos.y);
    }
  }

  for(; m; m = m->next){
    term_move_cursor(m->pos.x, m->pos.y - 1);
    printf("M%i:%iM", m->pos.x, m->pos.y);
  }

  point_t sc_pos = room_get_staircase_pos();
  term_move_cursor(sc_pos.x, sc_pos.y - 1);
  printf("S%i:%iS", sc_pos.x, sc_pos.y);
}


void debug_print_item(item_t const * const i, char *helper)
{
  nidebug("%s:", helper);
  nidebug("\n\tuse:%p\n\tdesc:%s\n\tnext:%p\n\tpos:%d;%d\n\ttype:%d\n\tstandson:%c", i->use, i->description, i->next, i->pos.x, i->pos.y,
          i->type, i->stands_on);
  switch(i->type){
    case I_armor:
      nidebug("\n\t\ttype:%d\n\t\tdurability:%d", SPEC_ATTR(i, armor_t)->type, SPEC_ATTR(i, armor_t)->durability);
      break;
    default: nidebug("INVALID ITEM TYPE");
  }
}
