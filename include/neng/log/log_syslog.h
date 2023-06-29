#ifndef __NENG_LOG_SYSLOG_H__
#define __NENG_LOG_SYSLOG_H__

#include <syslog.h>
#include "log.h"

__BEGIN_DECLS

// Match 定义了符合条件的系统日志， 输出到指定的facility； 此时日志已经被传递到此syslog appender
// mod, tag, level掩码位使用log_utils.h文件定义的__neng_log_bits_xxxxx 函数操作
typedef struct stNengLogSyslogMatch
{
    NengLogMatch match;
    int facility;
} NengLogSyslogMatch;

typedef struct stNengLogSyslogAppender
{
    NengLogAppender appender;
    char ident[64];
    int option;
    int facility;
} NengLogSyslogAppender;

NengLogSyslogAppender *NengLogCreateSyslogAppender(const char *ident, int option, int facility);
void NengLogSyslogAppenderAddMatch(NengLogSyslogAppender *sys_appender, const NengLogSyslogMatch *match);
void NengLogSyslogAppenderClearMatch(NengLogSyslogAppender *sys_appender);

__END_DECLS

#endif
