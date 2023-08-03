#include <neng/log/log.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "log_filter.h"
#include "log_appender.h"

////////////////////////////////////////////////////////////////////
// LogAppender Function
pthread_rwlock_t _neng_log_appender_list_rwlock = PTHREAD_RWLOCK_INITIALIZER;
NengLogAppenderList _neng_log_appender_list = TAILQ_HEAD_INITIALIZER(_neng_log_appender_list);
NengLogAppenderList _neng_log_appender_sync_list = TAILQ_HEAD_INITIALIZER(_neng_log_appender_sync_list);

int NengLogAddAppender(NengLogAppender *appender)
{
    pthread_rwlock_wrlock(&_neng_log_appender_list_rwlock);

    NengLogAppenderListItem *list_item = NULL;

    TAILQ_FOREACH(list_item, &_neng_log_appender_sync_list, entry)
    {
        if (list_item->appender == appender)
        {
            errno = EEXIST;
            pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
            return -1;
        }
    }

    TAILQ_FOREACH(list_item, &_neng_log_appender_list, entry)
    {
        if (list_item->appender == appender)
        {
            errno = EEXIST;
            pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
            return -1;
        }
    }

    list_item = calloc(1, sizeof(NengLogAppenderListItem));
    if (list_item == NULL)
    {
        CRIT_LOG("alloc appender list fail: %s", strerror(errno));
        pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
        return -1;
    }

    pthread_mutex_init(&(list_item->mtx), NULL);
    list_item->appender = appender;

    if (appender->flags.disable_async)
    {
        TAILQ_INSERT_TAIL(&_neng_log_appender_sync_list, list_item, entry);
    }
    else
    {
        TAILQ_INSERT_TAIL(&_neng_log_appender_list, list_item, entry);
    }

    pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);

    return 0;
}

int NengLogRemoveAppender(NengLogAppender *appender)
{
    pthread_rwlock_wrlock(&_neng_log_appender_list_rwlock);

    NengLogAppenderListItem *list_item = NULL;

    TAILQ_FOREACH(list_item, &_neng_log_appender_sync_list, entry)
    {
        if (list_item->appender == appender)
        {
            TAILQ_REMOVE(&_neng_log_appender_sync_list, list_item, entry);
            break;
        }
    }

    if (list_item == NULL)
    {
        TAILQ_FOREACH(list_item, &_neng_log_appender_list, entry)
        {
            if (list_item->appender == appender)
            {
                TAILQ_REMOVE(&_neng_log_appender_list, list_item, entry);
                break;
            }
        }
    }

    if (list_item == NULL)
    {
        errno = ENOENT;
        pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
        return -1;
    }

    pthread_mutex_destroy(&(list_item->mtx));
    if (list_item->appender->release_fn != NULL)
    {
        list_item->appender->release_fn(list_item->appender);
    }
    free(list_item);

    pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
    return 0;
}

void NengLogClearAppender(void)
{
    pthread_rwlock_wrlock(&_neng_log_appender_list_rwlock);
    while (!TAILQ_EMPTY(&_neng_log_appender_list))
    {
        NengLogAppenderListItem *list_item = TAILQ_FIRST(&_neng_log_appender_list);

        TAILQ_REMOVE(&_neng_log_appender_list, list_item, entry);
        pthread_mutex_destroy(&(list_item->mtx));

        NengLogAppenderClear(list_item->appender);
        if (list_item->appender->release_fn != NULL)
        {
            list_item->appender->release_fn(list_item->appender);
        }
        free(list_item);
    }
    while (!TAILQ_EMPTY(&_neng_log_appender_sync_list))
    {
        NengLogAppenderListItem *list_item = TAILQ_FIRST(&_neng_log_appender_sync_list);

        TAILQ_REMOVE(&_neng_log_appender_sync_list, list_item, entry);
        pthread_mutex_destroy(&(list_item->mtx));

        NengLogAppenderClear(list_item->appender);
        if (list_item->appender->release_fn != NULL)
        {
            list_item->appender->release_fn(list_item->appender);
        }
        free(list_item);
    }
    pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Appender Extend;
static int _AppenerInitExtend(NengLogAppender *appender)
{
    if (appender->extend != NULL)
    {
        return 0;
    }

    NengLogAppenderExtend *extend = (NengLogAppenderExtend *)calloc(1, sizeof(NengLogAppenderExtend));
    if (extend == NULL)
    {
        return -1;
    }

    TAILQ_INIT(&(extend->filter_list));
    appender->extend = extend;
    return 0;
}

static void _AppenderClearExtend(NengLogAppender *appender)
{
    NengLogAppenderExtend *extend = (NengLogAppenderExtend *)(appender->extend);

    if (extend == NULL)
    {
        NengLogAppenderClearFilter(appender);
        free(extend);
        appender->extend = NULL;
    }
}

void NengLogAppenderClear(NengLogAppender *appender)
{
    _AppenderClearExtend(appender);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Appender Filter
static int _FilterIsValid(NengLogFilter *filter)
{
    if (NengLogFilterModBitIsEmpty(filter) && NengLogFilterTagBitIsEmpty(filter) && NengLogFilterLevelBitIsEmpty(filter))
    {
        return 0;
    }

    return 1;
}

int NengLogAppenderAddFilter(NengLogAppender *appender, NengLogFilter *filter)
{
    if (_FilterIsValid(filter) == 0)
    {
        errno = EINVAL;
        return -1;
    }

    if (_AppenerInitExtend(appender) != 0)
    {
        return -1;
    }

    NengLogAppenderExtend *extend = (NengLogAppenderExtend *)(appender->extend);
    NengLogFilterItem *filter_item = (NengLogFilterItem *)calloc(1, sizeof(NengLogFilterItem));

    if (filter_item == NULL)
    {
        return -1;
    }

    filter_item->filter = *filter;
    TAILQ_INSERT_TAIL(&(extend->filter_list), filter_item, entry);
    return 0;
}

void NengLogAppenderClearFilter(NengLogAppender *appender)
{
    NengLogAppenderExtend *extend = (NengLogAppenderExtend *)(appender->extend);

    if (extend != NULL)
    {
        NengLogFilterItem *item = NULL;
        while ((item = TAILQ_FIRST(&(extend->filter_list))) != NULL)
        {
            TAILQ_REMOVE(&(extend->filter_list), item, entry);
            free(item);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Appender Filter Hit
int NengLogAppenderHitLogItem(NengLogAppender *appender, NengLogItem *item)
{
    NengLogAppenderExtend *extend = (NengLogAppenderExtend *)appender->extend;

    if (extend == NULL || TAILQ_EMPTY(&(extend->filter_list)))
    {
        return 1;
    }

    NengLogFilterItem *filter_item = NULL;

    TAILQ_FOREACH(filter_item, &(extend->filter_list), entry)
    {
        NengLogFilter *filter = &(filter_item->filter);

        if (NengLogFilterHit(filter, item->mod, item->tag, item->level) == 1)
        {
            return 1;
        }
    }

    return 0;
}