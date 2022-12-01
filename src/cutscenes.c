#include "terminal.h"
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

void cutscene_intro(void)
{
    char intro_text[] = "Death may be permanent, but Valhalla is eternal....";
    term_move_cursor(TERM_COLS_NUM/2 - sizeof(intro_text)/2, TERM_ROWS_NUM/2 - 1 );
    printf("%s", intro_text);
    fflush(stdout);
    sleep(4);
    system("clear");
}