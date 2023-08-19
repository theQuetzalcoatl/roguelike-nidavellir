#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "input.h"
#include "stdlib.h"

extern char **event_get_entries(void);
extern uint64_t event_get_entry_num(void);
extern void draw(void);

#define LOWER_DIVIDING_LINE_X_POS ((TERM_COLS_NUM*30)/100)


void display_runic_lines(void)
{
    char *runic_string = "ᛁᚠᚢᚨᚱᛖᚨᛒᛚᛖᛏᛟᚱᛖᚨᛞᛏᚺᛁᛊᛏᚺᛖᚾᛁᚺᛟᛈᛖᚢᛚᛁᚲᛖᛏᚺᛖᚷᚨᛗᛖ"; // ifuareabletoreadthisthenihopeulikethegame

    term_move_cursor(0, RUNIC_LINE_POS);

    /* actual characters of the runic line is 41 chars  */
    for(int n = TERM_COLS_NUM/41; n >= 0; --n) printf("%s", runic_string);
    for(uint8_t n = 1; n < TERM_ROWS_NUM - RUNIC_LINE_POS; ++n){
        term_move_cursor(LOWER_DIVIDING_LINE_X_POS - n%2, RUNIC_LINE_POS + n);
        printf("ᚷ");
    }
    for(uint8_t n = 0; n < RUNIC_LINE_POS; ++n){
        term_move_cursor(TERM_COLS_NUM-1 - n%2, n);
        printf("ᚷ");
    }
}
    

void display_player_stats(const mob_t player, const uint64_t turns)
{
    term_move_cursor(6, RUNIC_LINE_POS + 2);
    printf("health: %d/%d    ", player.health, PLAYER_MAX_HEALTH);
    term_move_cursor(6, RUNIC_LINE_POS + 3);
    printf("level: %d    ", player.level);
    term_move_cursor(6, RUNIC_LINE_POS + 4);
    printf("Turns: %ld", turns);
}


void display_to_player_window(const char * const option)
{
    char saved_content[RUNIC_LINE_POS][TERM_COLS_NUM];

    for(uint8_t col = 0; col < TERM_COLS_NUM-2; ++col){
        for(uint8_t row = 0; row < RUNIC_LINE_POS; ++row){
            saved_content[row][col] = term_getchar_xy(col, row);
            term_putchar_xy(' ', col, row);
        }
    }

    draw();
    term_move_cursor(0,0);

    if(!strcmp(option, "hotkeys")){
        printf("?  Prints this help\n");
        printf("Q  Quits the game\n");
        printf(".  Rest for one turn\n");
        printf("E  Show event log\n");

        term_move_cursor(0, RUNIC_LINE_POS - 1);
        printf("Press any key to get back...\n");
        get_keypress();
    }
    else if(!strcmp(option, "events")){
        char **entries = event_get_entries();
        uint64_t current_entry = event_get_entry_num() - 1; // -1 is for the 0 based indexing
        uint8_t log_lower_border = RUNIC_LINE_POS - 2;

        term_move_cursor(0, RUNIC_LINE_POS - 1);
        printf("Press up/down to scroll, or any other key to get back");

        while(1){
            int64_t entry = current_entry;
            term_move_cursor(0,0);
            /* for now, assuming an entry to be shorter than the player window */
            for(uint64_t y = 0; (y < log_lower_border) && (entry >= 0); ++y, --entry ){
                printf("*   %s\n", entries[entry]); /* draw() is not needed because of \n */
            }

            if(event_get_entry_num() < RUNIC_LINE_POS){
                get_keypress();
                break;
            }

            input_code_t key = get_keypress();
            if(key == ARROW_DOWN) current_entry = current_entry - 1*(current_entry > 0);
            else if(key == ARROW_UP) current_entry = current_entry + 1*(current_entry < event_get_entry_num() - 1);
            else break;

            for(uint8_t col = 0; col < TERM_COLS_NUM-2; ++col){
                for(uint8_t row = 0; row < log_lower_border; ++row) term_putchar_xy(' ', col, row);
            }
        }
    }
    else{
        printf("INVALID OPTION GIVEN\n");
        term_move_cursor(0, RUNIC_LINE_POS - 1);
        printf("Press any key to get back...\n");
        get_keypress();
    }

    for(uint8_t col = 0; col < TERM_COLS_NUM-2; ++col){
        for(uint8_t row = 0; row < RUNIC_LINE_POS; ++row) term_putchar_xy(saved_content[row][col], col, row);
    }

    draw();
}


void display_recent_events(void)
{
    term_move_cursor(LOWER_DIVIDING_LINE_X_POS + 2, RUNIC_LINE_POS + 1);
    for(uint8_t col = LOWER_DIVIDING_LINE_X_POS + 2; col < TERM_COLS_NUM-2; ++col){
        for(uint8_t row = RUNIC_LINE_POS + 1; row < TERM_ROWS_NUM; ++row) term_putchar_xy(' ', col, row);
    }

    term_move_cursor(LOWER_DIVIDING_LINE_X_POS + 2, RUNIC_LINE_POS + 1);

    uint8_t current_line = RUNIC_LINE_POS + 1;
    char **entries = event_get_entries();
    uint64_t entry_num = event_get_entry_num();
    entries += entry_num - 1;
    uint8_t lines_to_print = (entry_num <= (TERM_ROWS_NUM - RUNIC_LINE_POS - 1)) ? entry_num : TERM_ROWS_NUM - RUNIC_LINE_POS - 1;

    if(entries){
        while(lines_to_print){
            printf("%s", *entries);
            ++current_line;
            term_move_cursor(LOWER_DIVIDING_LINE_X_POS + 2, current_line);
            --entries;
            --lines_to_print;
        }
    }
}