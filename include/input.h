#ifndef _INPUT_
#define _INPUT_

#include <stdint.h>

#define NO_ARROW    (0x0u)
#define ARROW_UP    (0x1b5b41)
#define ARROW_DOWN  (0x1b5b42)
#define ARROW_RIGHT (0x1b5b43)
#define ARROW_LEFT  (0x1b5b44)

#define STEP_UP ARROW_UP
#define STEP_DOWN ARROW_DOWN
#define STEP_RIGHT ARROW_RIGHT
#define STEP_LEFT ARROW_LEFT

typedef uint32_t input_code_t;

extern input_code_t get_keypress(void);

#endif /* _INPUT_ */

