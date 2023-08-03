#ifndef __NENG_LOG_APPENDER_H__
#define __NENG_LOG_APPENDER_H__

#include <pthread.h>
#include <common/queue.h>

#include "log_def.h"

__BEGIN_DECLS

typedef struct stNengLogFilterItem
{
    TAILQ_ENTRY(stNengLogFilterItem)
    entry;
    NengLogFilter filter;
} NengLogFilterItem;

typedef TAILQ_HEAD(stNengLogFilterItemList, stNengLogFilterItem) NengLogFilterItemList;

typedef struct stNengLogAppenderExtend
{
    NengLogFilterItemList filter_list;
} NengLogAppenderExtend;

typedef struct stNengLogAppenderListItem
{
    TAILQ_ENTRY(stNengLogAppenderListItem)
    entry;
    pthread_mutex_t mtx;
    NengLogAppender *appender;
} NengLogAppenderListItem;

typedef TAILQ_HEAD(stNengLogAppenderList, stNengLogAppenderListItem) NengLogAppenderList;

extern pthread_rwlock_t _neng_log_appender_list_rwlock;
extern NengLogAppenderList _neng_log_appender_list;
extern NengLogAppenderList _neng_log_appender_sync_list;

int NengLogAppenderHitLogItem(NengLogAppender *appender, NengLogItem *item);

__END_DECLS

#endif
