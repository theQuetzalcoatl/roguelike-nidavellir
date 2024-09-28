
#include "input.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define KEY_ESCAPE (0x1b)

input_code_t get_keypress(void)
{
  input_code_t ch = 'a';
  ch = getchar();  
  
  if(isprint(ch)) return ch; /* is first char a printable one? */
  else if(ch == KEY_ESCAPE && getchar() == 0x5b){
    ch = getchar();
    return ((0x1b5b<<8) + ch);
  }
  else{
    nidebug("Invalid key combination!\n");
    return 0;
  }
}

