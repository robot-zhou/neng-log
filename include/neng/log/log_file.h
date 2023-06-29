#ifndef __NENG_LOG_FILE_H__
#define __NENG_LOG_FILE_H__

#include "log.h"

__BEGIN_DECLS

typedef struct stNengLogFileAppender
{
    NengLogAppender appender;
    char filepath[NENG_LOG_FILEPATH_SIZE];
    int max_size;
    int max_loop;
    uint16_t daily : 1;
    uint16_t reserve_flags : 15;
} NengLogFileAppender;

NengLogFileAppender *NengLogCreateFileAppender(const char *filepath);

__END_DECLS

#endif
