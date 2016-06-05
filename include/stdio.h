#pragma once

#include <stdarg.h>

int vsprintf(char *buf, const char *format, va_list args);
int sprintf(char *str, const char *format, ...);
int printf(const char *format, ...);
