#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"

static char **event_logs = NULL;
static uint64_t event_entry_num = 1; // there must be always a null event at the end

#define NULL_TERM (1u)


void event_log_add(const char *event)
{
  uint8_t event_string_length = strlen(event) + NULL_TERM;
  char **tmp = realloc(event_logs, sizeof(char*)*event_entry_num + sizeof(char*));
  if(!tmp){
    nidebug("Could not reallocate enough memory for a new event log entry! Keeping the old one.");
    return;
  }
  else{
    event_logs = tmp;
    ++event_entry_num;

    *(event_logs + event_entry_num - 2) = malloc(event_string_length); 
    if(event_logs[event_entry_num - 2] == NULL){
      free(event_logs + event_entry_num - 2);
      --event_entry_num;
      nidebug("Could not allocate enough memory for a new event log! Deleting the entry.");
      return;
    }
    else{
      strncpy(*(event_logs + event_entry_num -2), event, event_string_length < 50 ? event_string_length : 50);
      *(event_logs + event_entry_num - 1) = NULL; //null terminating the entry list
    }
  }
}

char **event_get_entries(void) {return event_logs; }

uint64_t event_get_entry_num(void) {return event_entry_num;}