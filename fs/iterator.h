#pragma once

#define DEFINE_ITERATE(array, len, type, func_type)                         \
        static void _iterate(func_type func_do)                             \
        {                                                                   \
            for (int i = 0; i < len; i++)                                   \
                func_do(array + i);                                         \
        }

#define DEFINE_ITERATE_SEL(array, len, type, func_type, check_type)         \
        static type *_iterate_sel(func_type does_match, check_type num)     \
        {                                                                   \
            for (int i = 0; i < len; i++)                                   \
                if (does_match(array + i, num))                             \
                    return array + i;                                       \
                return NULL;                                                \
        }
