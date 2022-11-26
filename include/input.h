#ifndef _INPUT_
#define _INPUT_

#include <stdint.h>

#define ARROW_UP    (0x1b5b41)
#define ARROW_DOWN  (0x1b5b42)
#define ARROW_RIGHT (0x1b5b43)
#define ARROW_LEFT  (0x1b5b44)

typedef uint32_t input_code_t;

extern uint32_t get_keypress(void);

#endif /* _INPUT_ */