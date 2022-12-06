#include "debug.h"
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wformat-security"

static FILE *fp = NULL;


void debug_init(void)
{
    fp = fopen("debug.txt", "w");
    if(fp == NULL){
        printf("Could not open \"debug.txt\"!\n");
        exit(1);
    }
}


void debug_deinit(void)
{
    fclose(fp);
}


void nidebug(const char *s, ...)
{
    va_list args;
    time_t rawtime;
    struct tm * timeinfo;

    time( &rawtime );
    timeinfo = localtime( &rawtime );
    fprintf(fp, "\n[%d:%d:%d] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    va_start(args, s);
    fprintf(fp, s, args);
    va_end(args);
}