#ifndef __NENG_LOG_APPENDER_H__
#define __NENG_LOG_APPENDER_H__

#include <pthread.h>
#include <common/queue.h>

#include "log_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stFilterItem
{
    TAILQ_ENTRY(stFilterItem)
    entry;
    NengLogFilter filter;
} FilterItem;

typedef TAILQ_HEAD(stFilterItemFilter, stFilterItem) FilterItemList;

typedef struct stAppenderExtend
{
    FilterItemList filter_list;
} AppenderExtend;

typedef struct stAppenderListItem
{
    TAILQ_ENTRY(stAppenderListItem)
    entry;
    pthread_mutex_t mtx;
    NengLogAppender *appender;
} AppenderListItem;

typedef TAILQ_HEAD(stAppenderList, stAppenderListItem) AppenderList;

extern pthread_rwlock_t _appener_list_rwlock;
extern AppenderList _appender_list;
extern AppenderList _appender_sync_list;

int NengLogAppenderHitLogItem(NengLogAppender *appender, NengLogItem *item);

#ifdef __cplusplus
}
#endif

#endif
