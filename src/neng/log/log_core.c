#include <neng/log/log.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <common/queue.h>

#include "log_def.h"
#include "log_misc.h"
#include "log_item.h"
#include "log_appender.h"
#include "sysfunc.h"

////////////////////////////////////////////////////////////////////
// 日志通用头输出函数
int NengLogWriteHeader(NengLogAppenderFlags *flags, const NengLogItem *item, char *buf, int bufsiz)
{
    int len = 0;
    int is_calc = (buf == NULL || bufsiz <= 0) ? 1 : 0;
    const char *separator = (flags->enable_tab_separator) ? "\t" : " ";

#define WRITE_HEADER_CHECK(n)  \
    do                         \
    {                          \
        if ((n) < 0)           \
        {                      \
            return len;        \
        }                      \
        len += (n);            \
        if (bufsiz <= len + 1) \
        {                      \
            return len;        \
        }                      \
    } while (0)

    if (!flags->disable_time)
    {
        if (is_calc)
        {
            len += 24;
        }
        else
        {
            time_t t = item->time / 1000;
            int ms = item->time % 1000;
            struct tm *tm = localtime(&t);
            int n = snprintf(buf + len, bufsiz - len, "%04d-%02d-%02d %02d:%02d:%02d.%03d%s",
                             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                             tm->tm_hour, tm->tm_min, tm->tm_sec, ms, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (flags->enable_hostname)
    {
        if (is_calc)
        {
            len += strlen(item->hostname) + 1;
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "%s%s", item->hostname, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (flags->enable_progname)
    {
        if (is_calc)
        {
            len += strlen(item->progname) + 1;
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "%s%s", item->progname, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (!flags->disable_pid)
    {
        if (is_calc)
        {
            char tmpbuf[32] = {0};
            snprintf(tmpbuf, 32, "[%d]%s", item->pid, separator);
            len += strlen(tmpbuf);
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "[%d]%s", item->pid, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (!flags->disable_level)
    {
        if (is_calc)
        {
            len += 7;
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "%-6s%s", NengLogLevel2Name(item->level), separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (flags->enable_tid)
    {
        if (is_calc)
        {
            char tmpbuf[32] = {0};
            snprintf(tmpbuf, 32, "tid:%d%s", item->tid, separator);
            len += strlen(tmpbuf);
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "tid:%d%s", item->tid, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (flags->enable_mod)
    {
        if (is_calc)
        {
            char tmpbuf[32] = {0};
            snprintf(tmpbuf, 32, "mod:%d%s", item->mod, separator);
            len += strlen(tmpbuf);
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "mod:%d%s", item->mod, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (flags->enable_tag)
    {
        if (is_calc)
        {
            char tmpbuf[32] = {0};
            snprintf(tmpbuf, 32, "tag:%d%s", item->tag, separator);
            len += strlen(tmpbuf);
        }
        else
        {
            int n = snprintf(buf + len, bufsiz - len, "tag:%d%s", item->tag, separator);
            WRITE_HEADER_CHECK(n);
        }
    }

    if (flags->enable_file)
    {
        const char *p = item->file;

        if (!flags->enable_fullpath)
        {
            p = strrchr(item->file, PATH_SEPARATOR_CHAR);
            p = (p == NULL) ? item->file : p + 1;
        }

        if (flags->enable_func)
        {
            if (is_calc)
            {
                char tmpbuf[NENG_LOG_FUNC_SIZE + NENG_LOG_FILEPATH_SIZE + 32] = {0};
                snprintf(tmpbuf, sizeof(tmpbuf), "<%s@%s:%d>%s", p, item->func, item->line, separator);
                len += strlen(tmpbuf);
            }
            else
            {
                int n = snprintf(buf + len, bufsiz - len, "<%s@%s:%d>%s", p, item->func, item->line, separator);
                WRITE_HEADER_CHECK(n);
            }
        }
        else
        {
            if (is_calc)
            {
                char tmpbuf[NENG_LOG_FUNC_SIZE + NENG_LOG_FILEPATH_SIZE + 32] = {0};
                snprintf(tmpbuf, sizeof(tmpbuf), "<%s:%d>%s", p, item->line, separator);
                len += strlen(tmpbuf);
            }
            else
            {
                int n = snprintf(buf + len, bufsiz - len, "<%s:%d>%s", p, item->line, separator);
                WRITE_HEADER_CHECK(n);
            }
        }
    }
    else
    {
        if (flags->enable_func)
        {
            if (is_calc)
            {
                char tmpbuf[NENG_LOG_FILEPATH_SIZE + 32] = {0};
                snprintf(tmpbuf, sizeof(tmpbuf), "<@%s:%d>%s", item->func, item->line, separator);
                len += strlen(tmpbuf);
            }
            else
            {
                int n = snprintf(buf + len, bufsiz - len, "<@%s:%d>%s", item->func, item->line, separator);
                WRITE_HEADER_CHECK(n);
            }
        }
    }

    return len;
}

////////////////////////////////////////////////////////////////////////////////
// 日志输出
static int _enable_async = 0;
static pthread_mutex_t _async_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t _async_cond = PTHREAD_COND_INITIALIZER;
static int _async_running = 0;
static pthread_t _async_tid;
static NengLogItemList _log_async_list = TAILQ_HEAD_INITIALIZER(_log_async_list);

void NengLogEnableAsync(int enable)
{
    _enable_async = enable;
}

static void NengLogAppenderListWrite(NengLogAppenderList *appender_list, NengLogItem *item)
{
    NengLogAppenderListItem *list_item = NULL;

    TAILQ_FOREACH(list_item, appender_list, entry)
    {
        if (list_item->appender == NULL)
        {
            continue;
        }

        pthread_mutex_lock(&(list_item->mtx));

        NengLogAppender *appender = list_item->appender;

        if (NengLogAppenderHitLogItem(appender, item) == 1)
        {
            appender->writer_fn(appender, item);
        }

        pthread_mutex_unlock(&(list_item->mtx));
    }
}

static void *_log_async_routine(void *arg)
{
    while (_async_running)
    {
        NengLogItemHead *item_head = NULL;

        pthread_mutex_lock(&_async_mtx);
        item_head = TAILQ_FIRST(&_log_async_list);

        if (item_head == NULL)
        {
            pthread_cond_wait(&_async_cond, &_async_mtx);
            pthread_mutex_unlock(&_async_mtx);
            continue;
        }

        TAILQ_REMOVE(&_log_async_list, item_head, entry);
        pthread_mutex_unlock(&_async_mtx);

        pthread_rwlock_rdlock(&_neng_log_appender_list_rwlock);
        NengLogAppenderListWrite(&_neng_log_appender_list, NENG_LOG_HEAD2ITEM(item_head));
        pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);

        NengLogFreeItem(NENG_LOG_HEAD2ITEM(item_head));
    }

    _async_running = 0;
    return NULL;
}

static void _log_async_clear(void)
{
    int empty = 0;

    if (_enable_async == 0 || _async_running == 0)
    {
        return;
    }

    for (int n = 0; n < 10 && empty == 0; n++)
    {
        pthread_mutex_lock(&_async_mtx);
        empty = (TAILQ_FIRST(&_log_async_list) == NULL) ? 1 : 0;
        pthread_mutex_unlock(&_async_mtx);
        usleep(50 * 1000);
    }

    _async_running = 0;
}

static void NengLogWriteAsync(NengLogItem *item)
{
    pthread_mutex_lock(&_async_mtx);
    TAILQ_INSERT_TAIL(&_log_async_list, NENG_LOG_ITEM2HEAD(item), entry);
    if (_async_running == 0)
    {
        _async_running = 1;
        if (pthread_create(&_async_tid, NULL, _log_async_routine, NULL) != 0)
        {
            _async_running = 0;
            CRIT_LOG("create async thread fail: %s", strerror(errno));
        }
    }
    pthread_mutex_unlock(&_async_mtx);
    pthread_cond_broadcast(&_async_cond);
}

static void NengLogWrite(NengLogItem *item)
{
    int async = 0;
    NengLogItemHead *item_head = NENG_LOG_ITEM2HEAD(item);

    pthread_rwlock_rdlock(&_neng_log_appender_list_rwlock);
    NengLogAppenderListWrite(&_neng_log_appender_sync_list, item);
    if (!_enable_async || item_head->flags.u_bits.is_sync == 1)
    {
        NengLogAppenderListWrite(&_neng_log_appender_list, item);
    }
    else
    {
        if (TAILQ_FIRST(&_neng_log_appender_list) != NULL)
        {
            async = 1;
        }
    }
    pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);

    if (async == 1)
    {
        NengLogWriteAsync(item);
        return;
    }

    NengLogFreeItem(item);
}

////////////////////////////////////////////////////////////////////
#ifdef __LINUX__
static pthread_mutex_t _install_onexit_mtx = PTHREAD_MUTEX_INITIALIZER;
static int _onexit_installed = 0;

static void _onexit(int n, void *arg)
{
    NengLogClear();
}

static void _install_onexit(void)
{
    if (_onexit_installed)
    {
        return;
    }

    pthread_mutex_lock(&_install_onexit_mtx);
    if (_onexit_installed == 0)
    {
        on_exit(_onexit, NULL);
        _onexit_installed = 1;
    }
    pthread_mutex_unlock(&_install_onexit_mtx);
}
#endif

////////////////////////////////////////////////////////////////////
// NengLog Function
#define NENG_LOG_MODLEVEL_BITMASK 0x00001000
static int _log_mod_level[NENG_LOG_MOD_SIZE] = {0};

void NengLogSetLevel(int level, int mod)
{
    if (mod < 0 || mod >= NENG_LOG_MOD_SIZE)
    {
        return;
    }

    if (level < kNengLogMinLevel || level > kNengLogMaxLevel)
    {
        return;
    }

    _log_mod_level[mod] = level | NENG_LOG_MODLEVEL_BITMASK;
}

int NengLogGetLevel(int mod)
{
    if (mod < 0 || mod >= NENG_LOG_MOD_SIZE)
    {
        return -1;
    }

    int level = _log_mod_level[mod];
    if (level & NENG_LOG_MODLEVEL_BITMASK)
    {
        return level ^ NENG_LOG_MODLEVEL_BITMASK;
    }

    return (mod == 0) ? kNengLogInfo : -1;
}

void NengLogClearLevel(int mod)
{
    if (mod < 0 || mod >= NENG_LOG_MOD_SIZE)
    {
        memset(_log_mod_level, 0, sizeof(_log_mod_level));
        return;
    }

    _log_mod_level[mod] = 0;
}

int NengLogCheckLeve(int mod, int level)
{
    int mod_level = NengLogGetLevel(mod);
    level = level & 0x00ff;
    if (mod_level >= 0)
    {
        if (level > mod_level)
        {
            return 0;
        }
    }
    else if (mod != 0 && level > NengLogGetLevel(0))
    {
        return 0;
    }

    return 1;
}

void _NengLogV(int mod, int tag, const char *file, const char *func, int line, int level, const char *fmt, va_list ap)
{
#ifdef __LINUX__
    _install_onexit();
#endif

    uint8_t log_flags = (uint8_t)(((uint32_t)level & 0xff00) >> 8);
    int size = vsnprintf(NULL, 0, fmt, ap);
    if (size <= 0)
    {
        return;
    }

    NengLogItem *item = NengLogAllocItem(size);

    if (item == NULL)
    {
        CRIT_LOG("alloc log item fail: %s", strerror(errno));
        return;
    }

    item->mod = mod;
    item->tag = tag;
    item->pid = getpid();
    item->tid = neng_log_sys_gettid();
    item->time = neng_log_sys_gettime_millisec();
    item->level = level;
    item->line = line;

    __neng_log_get_hostname(item->hostname, sizeof(item->hostname));
    __neng_log_get_progname(item->progname, sizeof(item->progname));

    if (file != NULL)
    {
        strncpy(item->file, file, sizeof(item->file) - 1);
    }

    if (func != NULL)
    {
        strncpy(item->func, func, sizeof(item->func) - 1);
    }

    vsprintf(item->content, fmt, ap);

    NengLogItemHead *item_head = NENG_LOG_ITEM2HEAD(item);
    item_head->flags.u8_val = log_flags;

    NengLogWrite(item);
}

void NengLogV(int mod, int tag, const char *file, const char *func, int line, int level, const char *fmt, va_list ap)
{
    if (NengLogCheckLeve(mod, level) == 0)
    {
        return;
    }

    return _NengLogV(mod, tag, file, func, line, level, fmt, ap);
}

void _NengLog(int mod, int tag, const char *file, const char *func, int line, int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _NengLogV(mod, tag, file, func, line, level, fmt, ap);
    va_end(ap);
}


void NengLog(int mod, int tag, const char *file, const char *func, int line, int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    NengLogV(mod, tag, file, func, line, level, fmt, ap);
    va_end(ap);
}

void NengLogClose(void)
{
    NengLogAppenderListItem *appender_item = NULL;

    pthread_rwlock_rdlock(&_neng_log_appender_list_rwlock);

    TAILQ_FOREACH(appender_item, &(_neng_log_appender_list), entry)
    {
        pthread_mutex_lock(&(appender_item->mtx));
        if (appender_item->appender->close_fn != NULL)
        {
            appender_item->appender->close_fn(appender_item->appender);
        }
        pthread_mutex_unlock(&(appender_item->mtx));
    }

    TAILQ_FOREACH(appender_item, &(_neng_log_appender_sync_list), entry)
    {
        pthread_mutex_lock(&(appender_item->mtx));
        if (appender_item->appender->close_fn != NULL)
        {
            appender_item->appender->close_fn(appender_item->appender);
        }
        pthread_mutex_unlock(&(appender_item->mtx));
    }

    pthread_rwlock_unlock(&_neng_log_appender_list_rwlock);
}

void NengLogClear(void)
{
    _log_async_clear();
    NengLogClearItem();
    NengLogClearAppender();
}
