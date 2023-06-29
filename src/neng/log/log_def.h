#ifndef __NENG_LOG_DEFINE_H__
#define __NENG_LOG_DEFINE_H__

#include <stdio.h>

#ifdef _DEBUG

#define CRIT_LOG(format, ...)                                          \
    do                                                                 \
    {                                                                  \
        fprintf(stderr, "neng-log crit: " format "\n", ##__VA_ARGS__); \
    } while (0)

#else

#define CRIT_LOG(format, ...)

#endif

#define NENG_LOG_ITEM_BUFSIZE 512
#define NENG_LOG_ITEM_POOL_SIZE 100

#define PATH_SEPARATOR_CHAR '/'
#define COUNT_OF(x) (sizeof(x) / sizeof((x)[0]))

#endif //__NENG_LOG_DEFINE_H__