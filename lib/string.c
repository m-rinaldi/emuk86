#include <string.h>
#include <stdint.h>

size_t strlen(const char *s)
{
    size_t count;

    for (count = 0; s[count]; count++)
        ;

    return count;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;

    for (i = 0; n--; i++) {
        dest[i] = src[i];
        if ('\0' == src[i])
            break;
    }

    return dest;  
}

int strcmp(const char *s1, const char *s2)
{
    for (; *s1; s1++, s2++) {
        if (!*s2)
            return 1;

        if (*s1 < *s2)
            return -1;

        if (*s1 > *s2)
            return 1;
    }

    // *s1 is '\0'
    return !*s2 ? 0 : -1;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    for (; n && --n && *s1; s1++, s2++) {
        if (!*s2)
            return 1;

        if (*s1 < *s2)
            return -1;

        if (*s1 > *s2)
            return 1;
    }

    // all n characters were compared
    if (!n)
        return 0;

    // *s1 is '\0'
    return !*s2 ? 0 : -1;
}

void bzero(void *s, size_t n)
{
    memset(s, 0, n);
}

void *memset(void *s, uint8_t c, size_t n)
{
    while (n--)
        n[(uint8_t *) s] = c;

    return s; 
}

void *memcpy(void *dest, const void *src, size_t n)
{
    while (n--)
        n[(uint8_t *) dest] = n[(uint8_t *) src];

    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    // if the two memory areas are the same, i.e.: dest == src, nothing is done

    if (dest > src) // copy starting from the end of the memory areas
        while (n--)
            n[(uint8_t *) dest] = n[(uint8_t *) src];
    else if (dest < src) { // copy starting at the beginning of the memory areas
        const uint8_t *s;
        uint8_t *d;

        s = src;
        d = dest;

        while (n--)
            *d++ = *s++;
    }

    return dest;
}
