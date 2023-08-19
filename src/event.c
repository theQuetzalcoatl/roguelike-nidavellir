#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"

static char **event_logs = NULL;
static uint64_t event_entry_num = 0; // there must be always a null event at the end

#define NULL_TERM (1u)


void event_log_add(const char *event)
{
  uint8_t event_string_length = strlen(event) + NULL_TERM;
  char **original_event_logs = event_logs;

  event_logs  = realloc(event_logs, sizeof(char*) * (event_entry_num + 1));
  if(!event_logs){
    nidebug("Could not reallocate enough memory for a new event log entry! Keeping the old one.");
    event_logs = original_event_logs;
    return;
  }
  else{
    *(event_logs + event_entry_num) = malloc(event_string_length); // -1 not needed becasue the index is not inceremented yet, therefore -1 is a given
    if(event_logs[event_entry_num] == NULL){ // NOTE: potential mem leak because if we fail here, the entry won't be shrunk back and memory is gonna be wasted or event a hole in the log will appear
      nidebug("Could not allocate enough memory for a new event log! Deleting the entry.");
      return;
    }
    else{
      ++event_entry_num;
      strncpy(*(event_logs + event_entry_num -1), event, event_string_length < 50 ? event_string_length : 50);
    }
  }
}

char **event_get_entries(void) {return event_logs; }

uint64_t event_get_entry_num(void) {return event_entry_num;}