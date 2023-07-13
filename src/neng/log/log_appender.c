#include <neng/log/log.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "log_appender.h"

////////////////////////////////////////////////////////////////////
// LogAppender Function
pthread_rwlock_t _appener_list_rwlock = PTHREAD_RWLOCK_INITIALIZER;
AppenderList _appender_list = LIST_HEAD_INITIALIZER(stAppenderList);
AppenderList _appender_sync_list = LIST_HEAD_INITIALIZER(stAppenderList);

int NengLogAddAppender(NengLogAppender *appender)
{
    pthread_rwlock_wrlock(&_appener_list_rwlock);

    AppenderListItem *list_item = NULL;

    LIST_FOREACH(list_item, &_appender_sync_list, entry)
    {
        if (list_item->appender == appender)
        {
            errno = EEXIST;
            pthread_rwlock_unlock(&_appener_list_rwlock);
            return -1;
        }
    }

    LIST_FOREACH(list_item, &_appender_list, entry)
    {
        if (list_item->appender == appender)
        {
            errno = EEXIST;
            pthread_rwlock_unlock(&_appener_list_rwlock);
            return -1;
        }
    }

    list_item = calloc(1, sizeof(AppenderListItem));
    if (list_item == NULL)
    {
        CRIT_LOG("alloc appender list fail: %s", strerror(errno));
        pthread_rwlock_unlock(&_appener_list_rwlock);
        return -1;
    }

    pthread_mutex_init(&(list_item->mtx), NULL);
    list_item->appender = appender;

    if (appender->flags.disable_async)
    {
        LIST_INSERT_HEAD(&_appender_sync_list, list_item, entry);
    }
    else
    {
        LIST_INSERT_HEAD(&_appender_list, list_item, entry);
    }

    pthread_rwlock_unlock(&_appener_list_rwlock);

    return 0;
}

int NengLogRemoveAppender(NengLogAppender *appender)
{
    pthread_rwlock_wrlock(&_appener_list_rwlock);

    AppenderListItem *list_item = NULL;

    LIST_FOREACH(list_item, &_appender_sync_list, entry)
    {
        if (list_item->appender == appender)
        {
            break;
        }
    }

    if (list_item == NULL)
    {
        LIST_FOREACH(list_item, &_appender_list, entry)
        {
            if (list_item->appender == appender)
            {
                break;
            }
        }
    }

    if (list_item == NULL)
    {
        errno = ENOENT;
        pthread_rwlock_unlock(&_appener_list_rwlock);
        return -1;
    }

    LIST_REMOVE(list_item, entry);
    pthread_mutex_destroy(&(list_item->mtx));
    if (list_item->appender->release_fn != NULL)
    {
        list_item->appender->release_fn(list_item->appender);
    }
    free(list_item);

    pthread_rwlock_unlock(&_appener_list_rwlock);
    return 0;
}

void NengLogClearAppender(void)
{
    pthread_rwlock_wrlock(&_appener_list_rwlock);
    while (LIST_FIRST(&_appender_list) != NULL)
    {
        AppenderListItem *list_item = LIST_FIRST(&_appender_list);

        LIST_REMOVE(list_item, entry);
        pthread_mutex_destroy(&(list_item->mtx));

        NengLogAppenderClear(list_item->appender);
        if (list_item->appender->release_fn != NULL)
        {
            list_item->appender->release_fn(list_item->appender);
        }
        free(list_item);
    }
    pthread_rwlock_unlock(&_appener_list_rwlock);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Appender Extend;
static int _AppenerInitExtend(NengLogAppender *appender)
{
    if (appender->extend != NULL)
    {
        return 0;
    }

    AppenderExtend *extend = (AppenderExtend *)calloc(1, sizeof(AppenderExtend));
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
    AppenderExtend *extend = (AppenderExtend *)(appender->extend);

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

    AppenderExtend *extend = (AppenderExtend *)(appender->extend);
    FilterItem *filter_item = (FilterItem *)calloc(1, sizeof(FilterItem));

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
    AppenderExtend *extend = (AppenderExtend *)(appender->extend);

    if (extend != NULL)
    {
        FilterItem *item = NULL;
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
    AppenderExtend *extend = (AppenderExtend *)appender->extend;

    if (extend == NULL || TAILQ_EMPTY(&(extend->filter_list)))
    {
        return 1;
    }

    FilterItem *filter_item = NULL;

    TAILQ_FOREACH(filter_item, &(extend->filter_list), entry)
    {
        NengLogFilter *filter = &(filter_item->filter);

        if (NengLogFilterModBitIsEmpty(filter) == 0)
        {
            if (item->mod < 0 || NengLogFilterGetModBit(filter, item->mod) != 1)
            {
                continue;
            }
        }

        if (NengLogFilterTagBitIsEmpty(filter) == 0)
        {
            if (item->tag < 0 || NengLogFilterGetTagBit(filter, item->tag) != 1)
            {
                continue;
            }
        }

        if (NengLogFilterLevelBitIsEmpty(filter) == 0)
        {
            if (item->level < 0 || NengLogFilterGetLevelBit(filter, item->level) != 1)
            {
                continue;
            }
        }

        return 1;
    }

    return 0;
}