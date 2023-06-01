#include "room.h"
#include "mob.h"
#include "debug.h"
#include "corridor.h"
#include "item.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CUSTOM_FILE_NAME ".custom_map.txt"
#define TERM_ZERO (1u)


void get_objects_from_custom_map(void)
{
    mob_t *mob = NULL;
    item_t *item = NULL;
    int16_t x = 0, y = 0;

    FILE *fp = fopen(CUSTOM_FILE_NAME, "r");
    if(fp == NULL){
        nidebug("Could not open " CUSTOM_FILE_NAME "!\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    uint32_t file_size = ftell(fp) + TERM_ZERO;
    fseek(fp, 0, SEEK_SET);

    char *file_content = malloc(file_size);
    char * tmp = file_content;
    fread(file_content, sizeof(char), file_size - TERM_ZERO, fp);
    file_content[file_size-1] = 0;

    while(*file_content){
        switch(*file_content)
        {
            case ROOM_DOOR:
            case ROOM_FLOOR:
            case VERTICAL_WALL:
            case HORIZONTAL_WALL:
            case CORRIDOR_FLOOR:
            case EMPTY_SPACE:
                break;
            case '\n':
                ++y;
                x = 0;
                break;
            case ITEM_SYMBOL:
                item = item_spawn();
                if(item){
                    item->obj.pos = (pos_t){.x = x, .y = y};
                    nidebug("item at: x:%d y:%d", x, y);
                }
                else nidebug("Could not summon item.");
                break;
            /* from here on, assuming a mob */
            default:
                mob = mob_summon(*file_content);
                if(mob) mob->obj.pos = (pos_t){.x = x, .y = y};
                else nidebug("Could not summon: [%c]", *file_content);
        }
        if(*file_content != '\n'){
            term_putchar_xy(*file_content, x, y);
            ++x;
        }
        ++file_content;
    }

    free(tmp);
    fclose(fp);
}
