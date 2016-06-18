#pragma once

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *);
char *strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

void bzero(void *s, size_t n) __attribute__((weak));
void *memset(void *s, uint8_t c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
