#ifndef __NENG_LOG_ITEM_H__
#define __NENG_LOG_ITEM_H__

#include <neng/log/log.h>
#include <common/queue.h>

#include "log_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union unNengLogItemFlags
{
    struct log_core
    {
        uint8_t is_sync : 1;
        uint8_t reserve_bits : 7;
    } u_bits;
    uint8_t u8_val;
} NengLogItemFlags;

typedef struct stNengLogItemHead
{
    TAILQ_ENTRY(stNengLogItemHead) entry;
    int size;
    NengLogItemFlags flags;
} NengLogItemHead;

typedef TAILQ_HEAD(stNengLogItemList, stNengLogItemHead) NengLogItemList;

#define NENG_LOG_ITEM_FIXSIZE (sizeof(NengLogItemHead) + sizeof(NengLogItem))
#define NENG_LOG_ITEM_CONTENT_DEFSIZE (NENG_LOG_ITEM_BUFSIZE - NENG_LOG_ITEM_FIXSIZE)
#define NENG_LOG_ITEM2HEAD(x) (NengLogItemHead *)((char *)(x) - sizeof(NengLogItemHead))
#define NENG_LOG_HEAD2ITEM(x) (NengLogItem *)((char *)(x) + sizeof(NengLogItemHead))
#define NENG_LOG_ITEM_SIZE(x) ((x)->size - sizeof(NengLogItemHead))
#define NENG_LOG_ITEM_CONTENT_SIZE(x) ((x)->size - NENG_LOG_ITEM_FIXSIZE)

NengLogItem *NengLogAllocItem(int size);
void NengLogFreeItem(NengLogItem *item);
void NengLogClearItem(void);

#ifdef __cplusplus
}
#endif

#endif
