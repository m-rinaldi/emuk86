#pragma once

#include <stdarg.h>

#ifdef HOSTED
    #include_next <stdio.h>
#endif

int vsprintf(char *buf, const char *format, va_list args);
int sprintf(char *str, const char *format, ...);
int printf(const char *format, ...);
