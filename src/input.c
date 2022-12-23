
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define KEY_ESCAPE (0x1b)

input_code_t get_keypress(void)
{
    input_code_t ch = 'a';
    ch = getchar();  
    
    if(isalpha(ch)) return ch;
    else if(ch == KEY_ESCAPE){
        ch = getchar();
        if(ch == 0x5b){
            ch = getchar();
            return ((0x1b5b<<8) + ch);
        }
        else{
            printf("Invalid key combination!\n");
            exit(1);
        }
    }
    else{
        printf("Invalid key combination!\n");
        exit(1);
    }
}