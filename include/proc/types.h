#pragma once

#include <stdint.h>

#ifndef HOSTED
    typedef uint16_t pid_t;
#else
    #include <sys/types.h>
#endif
