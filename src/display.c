#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "input.h"
#include "stdlib.h"

extern char **event_get_entries(void);
extern uint64_t event_get_entry_num(void);
extern void draw(void);


void display_runic_lines(void)
{
    char *runic_string = "ᛁᚠᚢᚨᚱᛖᚨᛒᛚᛖᛏᛟᚱᛖᚨᛞᛏᚺᛁᛊᛏᚺᛖᚾᛁᚺᛟᛈᛖᚢᛚᛁᚲᛖᛏᚺᛖᚷᚨᛗᛖ"; // ifuareabletoreadthisthenihopeulikethegame

    term_move_cursor(0, RUNIC_LINE_POS);

    /* actual characters of the runic line is 41 chars  */
    for(int n = TERM_COLS_NUM/41; n >= 0; --n) printf("%s", runic_string);
    for(uint8_t n = 1; n < TERM_ROWS_NUM - RUNIC_LINE_POS; ++n){
        term_move_cursor(40 - n%2, RUNIC_LINE_POS + n);
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
        int64_t starting_entry = event_get_entry_num() - 2; // diregarding the last entry and taking 0 index into account
        int64_t current_entry = starting_entry;
        uint8_t log_lower_border = RUNIC_LINE_POS - 2;

        while(1){
            int64_t entry = current_entry;
            term_move_cursor(0,0);
            /* for now, assuming an entry to be shorter than the player window */
            for(uint64_t printed_events = 0; (printed_events < log_lower_border) && entry >= 0; ++printed_events, --entry ){
                printf("*   %s\n", entries[entry]); /* draw() is not needed because of \n */
            }

            term_move_cursor(0, RUNIC_LINE_POS - 1);
            printf("Press up/down to scroll, or any other key to get back");

            if(starting_entry < RUNIC_LINE_POS){
                get_keypress();
                break;
            }

            input_code_t key = get_keypress();
            if(key == ARROW_DOWN) current_entry = current_entry - 1*(current_entry > 0);
            else if(key == ARROW_UP) current_entry = current_entry + 1*(current_entry < starting_entry);
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
