#include "terminal.h"
#include "cutscenes.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#pragma GCC diagnostic ignored "-Wunused-result"

void cutscene_intro(void)
{
  char *runic_havamal_16[4] = 
      {
          "ᛏᚺᛖ ᚲᛟᚹᚨᚱᛞᛚᛁ ᛗᚨᚾ ᛏᚺᛁᚾᚲᛊ ᚺᛖ ᚹᛁᛚᛚ ᛚᛁᚹᛖ ᚠᛟᚱ ᛖᚹᛖᚱ,",
          "ᛁᚠ ᚺᛖ ᚲᛖᛖᛈᛊ ᚨᚹᚨᛁ ᚠᚱᛟᛗ ᚠᛁᚷᚺᛏᛁᚾᚷ;",
          "ᛒᚢᛏ ᛟᛚᛞ ᚨᚷᛖ ᚹᛟᚾ’ᛏ ᚷᚱᚨᚾᛏ ᚺᛁᛗ ᚨ ᛏᚱᚢᛊᛖ",
          "ᛖᚹᛖᚾ ᛁᚠ ᛊᛈᛖᚨᚱᛊ ᛊᛈᚨᚱᛖ ᚺᛁᛗ."
      };
  char *latin_havamal_16[4] = 
      {
          "The cowardly man thinks he will live for ever,",
          "if he keeps away from fighting;",
          "but old age won't grant him a truce",
          "even if spears spare him."
      };
  uint16_t text_indexes[100] = {0}; // this should be the size of 'latin_text' with \0. Could be sizeof(latin_text)-1 but that gets the compiler icky
  point_t text = {.y = TERMINAL_HEIGHT/2 - 1, .x = TERMINAL_WIDTH/2 - sizeof(latin_havamal_16[0])/2};
  char dummy;

  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK); /* set input to non-blocking to be able to skip cutscene */
  fflush(stdin);

  for(uint8_t line = 0; sizeof(latin_havamal_16)/sizeof(latin_havamal_16[0]) > line; ++line){
    term_move_cursor(text.x, text.y + line);
    printf("%s", runic_havamal_16[line]);
  }
  fflush(stdout); sleep(1);

  for(uint8_t line = 0; sizeof(latin_havamal_16)/sizeof(latin_havamal_16[0]) > line; ++line){
    text.y = TERMINAL_HEIGHT/2 - 1 + line;
    for(int i = 0; latin_havamal_16[line][i]; ++i) text_indexes[i] = i;
    /* Durstenfeld's in-place shuffle*/
    stir_elements_randomly(strlen(latin_havamal_16[line]), text_indexes);

    for(uint16_t c = 0; c < strlen(latin_havamal_16[line]); ++c){
      term_putchar_xy(latin_havamal_16[line][text_indexes[c]], text.x+text_indexes[c], text.y);
      system("sleep 0.05s");
      fflush(stdout);
      if(read(STDIN_FILENO, &dummy, sizeof(dummy)) != -1) goto end_intro; /* skip cutscene if there is anything in the input buffer */
    }
  }

  fflush(stdout); sleep(2);

end_intro:
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);

  for(uint8_t row = 0; row < TERMINAL_HEIGHT; ++row){
    for(uint8_t col = 0; col < TERMINAL_WIDTH; ++col) term_putchar_xy(' ', col, row);
  }
}

void cutscene_dead(void)
{
  char runic_text[] = "ᚾᛟ ᚹᚨᛚᚺᚨᛚᛚᚨ ᛏᚺᛁᛊ ᛏᛁᛗᛖ";
  char latin_text[] = "No Valhalla this time";
  uint16_t text_indexes[22]; // this should be the size of 'runic_text' with \0. Could be sizeof(intro_text)-1 but that gets the compiler icky
  point_t text = {.y = TERMINAL_HEIGHT/2 - 1, .x = TERMINAL_WIDTH/2 - sizeof(latin_text[0])/2};

  system("clear");

  for(int i = 0; latin_text[i]; ++i) text_indexes[i] = i;
  stir_elements_randomly(strlen(latin_text), text_indexes);

  term_move_cursor(text.x, text.y);
  printf("%s", runic_text);
  fflush(stdout); sleep(1);

  for(uint16_t i = 0; i < strlen(latin_text); ++i){
    term_putchar_xy(latin_text[text_indexes[i]], text.x+text_indexes[i], text.y);
    system("sleep 0.05s");
    fflush(stdout);
  }
  fflush(stdout); sleep(2);
  system("clear");
}

void stir_elements_randomly(uint64_t array_len, uint16_t *array)
{
  /* Durstenfeld's in-place shuffle*/
  uint16_t tmp, rn = 0;
  for(int i = array_len-1; i; --i){
    rn = CALC_RAND(i, 0);
    tmp = array[i];
    array[i] = array[rn];
    array[rn] = tmp;
  }
}