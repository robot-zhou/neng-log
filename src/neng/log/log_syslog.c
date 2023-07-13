#include <neng/log/log.h>
#include <neng/log/log_syslog.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <common/queue.h>

#include "log_def.h"
#include "log_misc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Filter Item
typedef struct stNengLogSyslogFilterItem
{
    TAILQ_ENTRY(stNengLogSyslogFilterItem)
    list_entry;
    NengLogSyslogFilter sys_filter;
} NengLogSyslogFilterItem;

typedef TAILQ_HEAD(stNengLogSyslogFilterList, stNengLogSyslogFilterItem) NengLogSyslogFilterList;

typedef struct stNengLogSyslogAppenderEx
{
    NengLogSyslogFilterList filter_list;
} NengLogSyslogAppenderEx;

typedef struct stNengLogSyslogContext
{
    uint16_t is_opened : 1;
} NengLogSyslogContext;

#define __SYSLOG_APPENDER(x) (NengLogSyslogAppender *)((char *)(x) - (size_t) & (((NengLogSyslogAppender *)0)->appender))
#define __SYSLOG_APPENDER_EX(sa) (NengLogSyslogAppenderEx *)((char *)(sa) + sizeof(NengLogSyslogAppender))
#define __SYSLOG_CONTEXT(sa) (NengLogSyslogContext *)((char *)(sa) + sizeof(NengLogSyslogAppender) + sizeof(NengLogSyslogAppenderEx))

void NengLogSyslogAppenderAddFilter(NengLogSyslogAppender *sys_appender, const NengLogSyslogFilter *sys_filter)
{
    NengLogSyslogAppenderEx *sys_appender_ex = __SYSLOG_APPENDER_EX(sys_appender);
    NengLogSyslogFilterItem *tmp_item = (NengLogSyslogFilterItem *)calloc(1, sizeof(NengLogSyslogFilterItem));

    if (tmp_item == NULL)
    {
        CRIT_LOG("new syslog filter fail: %s", strerror(errno));
        return;
    }

    tmp_item->sys_filter = *sys_filter;
    TAILQ_INSERT_TAIL(&(sys_appender_ex->filter_list), tmp_item, list_entry);
}

void NengLogSyslogAppenderClearFilter(NengLogSyslogAppender *sys_appender)
{
    NengLogSyslogAppenderEx *sys_appender_ex = __SYSLOG_APPENDER_EX(sys_appender);

    while (TAILQ_EMPTY(&(sys_appender_ex->filter_list)))
    {
        NengLogSyslogFilterItem *item = TAILQ_FIRST(&(sys_appender_ex->filter_list));
        TAILQ_REMOVE(&(sys_appender_ex->filter_list), item, list_entry);
        free(item);
    }
}

static void __syslog_write(int pri, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsyslog(pri, fmt, ap);
    va_end(ap);
}

static void NengLogSyslogWrite(struct stNengLogAppender *appender, const NengLogItem *item)
{
    NengLogSyslogAppender *sys_appender = __SYSLOG_APPENDER(appender);
    NengLogSyslogAppenderEx *sys_appender_ex = __SYSLOG_APPENDER_EX(sys_appender);
    NengLogSyslogContext *sys_context = __SYSLOG_CONTEXT(sys_appender);

    if (!sys_context->is_opened)
    {
        if (sys_appender->ident[0] == '\0')
        {
            get_progname(sys_appender->ident, sizeof(sys_appender->ident));
        }

        openlog(sys_appender->ident, sys_appender->option, sys_appender->facility);
        sys_context->is_opened = 1;
    }

    NengLogAppenderFlags flags = appender->flags;
    flags.disable_time = 1;
    flags.disable_level = 1;
    flags.disable_pid = 1;
    flags.enable_hostname = 0;
    flags.enable_progname = 0;

    char buf[NENG_LOG_FILEPATH_SIZE + NENG_LOG_FUNC_SIZE + NENG_LOG_PROGNAME_SIZE + NENG_LOG_HOSTNAME_SIZE + 128] = {0};
    int header_len = NengLogWriteHeader(&(flags), item, buf, sizeof(buf));
    if (header_len > 0)
    {
        buf[header_len++] = ':';
    }
    buf[header_len] = '\0';

    if (TAILQ_FIRST(&(sys_appender_ex->filter_list)) == NULL)
    {
        __syslog_write(item->level, "%s%s", buf, item->content);
        return;
    }

    int pri = item->level;
    NengLogSyslogFilterItem *filter_item = NULL;

    TAILQ_FOREACH(filter_item, &(sys_appender_ex->filter_list), list_entry)
    {
        NengLogSyslogFilter *sys_filter = &(filter_item->sys_filter);
        NengLogFilter *filter = &(sys_filter->filter);

        if (NengLogFilterModBitIsEmpty(filter) == 0)
        {
            if (item->mod <= 0 || NengLogFilterGetModBit(filter, item->mod) != 1)
            {
                continue;
            }
        }

        if (NengLogFilterTagBitIsEmpty(filter) == 0)
        {
            if (item->tag <= 0 || NengLogFilterGetTagBit(filter, item->tag) != 1)
            {
                continue;
            }
        }

        if (NengLogFilterLevelBitIsEmpty(filter) == 0)
        {
            if (NengLogFilterGetLevelBit(filter, item->level) != 1)
            {
                continue;
            }
        }

        if (sys_filter->facility != 0)
        {
            pri |= sys_filter->facility & LOG_FACMASK;
            break;
        }
    }

    if (filter_item == NULL)
    {
        pri |= sys_appender->facility & LOG_FACMASK;
    }

    __syslog_write(pri, "%s%s", buf, item->content);
}

static void NengLogSyslogRelease(struct stNengLogAppender *appender)
{
    NengLogSyslogAppender *sys_appender = __SYSLOG_APPENDER(appender);
    NengLogSyslogContext *sys_context = __SYSLOG_CONTEXT(sys_appender);

    if (sys_context->is_opened)
    {
        closelog();
        sys_context->is_opened = 0;
    }

    NengLogSyslogAppenderClearFilter(sys_appender);
    free(sys_appender);
}

NengLogSyslogAppender *NengLogCreateSyslogAppender(const char *ident, int option, int facility)
{
    NengLogSyslogAppender *sys_appender = (NengLogSyslogAppender *)calloc(1, sizeof(NengLogSyslogAppender) + sizeof(NengLogSyslogAppenderEx) + sizeof(NengLogSyslogContext));

    if (sys_appender == NULL)
    {
        CRIT_LOG("alloc syslog appender fail: %s", strerror(errno));
        return NULL;
    }

    strncpy(sys_appender->appender.name, "syslog", sizeof(sys_appender->appender.name) - 1);
    sys_appender->appender.writer_fn = NengLogSyslogWrite;
    sys_appender->appender.release_fn = NengLogSyslogRelease;
    sys_appender->appender.flags.disable_async = 1;

    if (ident != NULL)
    {
        strncpy(sys_appender->ident, ident, sizeof(sys_appender->ident) - 1);
    }

    if (option == 0)
    {
        sys_appender->option = LOG_ODELAY | LOG_PID;
    }
    else
    {
        sys_appender->option = option;
    }

    sys_appender->facility = facility;

    NengLogSyslogAppenderEx *sys_appender_ex = __SYSLOG_APPENDER_EX(sys_appender);

    TAILQ_INIT(&(sys_appender_ex->filter_list));

    NengLogSyslogContext *sys_context = __SYSLOG_CONTEXT(sys_appender);

    sys_context->is_opened = 0;

    return sys_appender;
}