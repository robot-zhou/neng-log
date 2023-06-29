#include "log_item.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////
// NengLogItem Function
static pthread_mutex_t _item_list_mtx = PTHREAD_MUTEX_INITIALIZER;
static NengLogItemList _item_list = TAILQ_HEAD_INITIALIZER(_item_list);
static int _item_list_size = 0;

NengLogItem *NengLogAllocItem(int size)
{
    NengLogItemHead *item_head = NULL;

    if (size < NENG_LOG_ITEM_CONTENT_DEFSIZE)
    {
        pthread_mutex_lock(&_item_list_mtx);

        item_head = TAILQ_FIRST(&_item_list);

        if (item_head != NULL)
        {
            TAILQ_REMOVE(&_item_list, item_head, entry);
            _item_list_size--;
            item_head->flags.u8_val = 0;
            memset(NENG_LOG_HEAD2ITEM(item_head), 0, NENG_LOG_ITEM_SIZE(item_head));
        }

        pthread_mutex_unlock(&_item_list_mtx);

        if (item_head != NULL)
        {
            return NENG_LOG_HEAD2ITEM(item_head);
        }
    }

    size = (size < NENG_LOG_ITEM_CONTENT_DEFSIZE) ? NENG_LOG_ITEM_BUFSIZE : (size + NENG_LOG_ITEM_FIXSIZE);
    item_head = (NengLogItemHead *)calloc(1, size);

    if (item_head == NULL)
    {
        CRIT_LOG("alloc item fail: %s", strerror(errno));
        return NULL;
    }

    item_head->size = size;
    return NENG_LOG_HEAD2ITEM(item_head);
}

void NengLogFreeItem(NengLogItem *item)
{
    NengLogItemHead *item_head = NENG_LOG_ITEM2HEAD(item);

    if (item_head->size != NENG_LOG_ITEM_BUFSIZE)
    {
        free(item_head);
        return;
    }

    pthread_mutex_lock(&_item_list_mtx);
    if (_item_list_size < NENG_LOG_ITEM_POOL_SIZE)
    {
        TAILQ_INSERT_HEAD(&_item_list, item_head, entry);
    }
    else
    {
        free(item_head);
    }
    pthread_mutex_unlock(&_item_list_mtx);
}

void NengLogClearItem(void)
{
    pthread_mutex_lock(&_item_list_mtx);

    while (TAILQ_FIRST(&_item_list) != NULL)
    {
        NengLogItemHead *item_head = TAILQ_FIRST(&_item_list);
        TAILQ_REMOVE(&_item_list, item_head, entry);
        free(item_head);
    }

    pthread_mutex_unlock(&_item_list_mtx);
}