#ifndef utils_h
#define utils_h

#include <Arduino.h>

static void p(Print *printer, char const *fmt, ... ){
    char buf[128]; // resulting string limited to 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(buf, 128, fmt, args);
    va_end (args);
    printer->print(buf);
}

#endif