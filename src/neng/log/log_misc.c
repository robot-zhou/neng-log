#include "log_misc.h"

#include <neng/log/log.h>

#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
#include <sys/time.h>

#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////
// astr
ARRAY_GENERATE(astr_, char, NULL, NULL, NULL, NULL);

////////////////////////////////////////////////////////////////////
// NengLogAppender Bit Function
inline int __neng_log_bits_set(uint8_t *bits, int n, int val)
{
    if (val < 0)
    {
        return -1;
    }

    int idx = val / 8;
    uint8_t bit_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

    if (idx >= n)
    {
        return -1;
    }

    int bit_idx = val % 8;
    bits[idx] |= bit_mask[bit_idx];
    return 0;
}

inline int __neng_log_bits_get(uint8_t *bits, int n, int val)
{
    if (val < 0)
    {
        return -1;
    }
    
    int idx = val / 8;
    uint8_t bit_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

    if (idx >= n)
    {
        return -1;
    }

    int bit_idx = val % 8;
    uint8_t bit_val = bits[idx] & bit_mask[bit_idx];

    return (bit_val == 0) ? 0 : 1;
}

inline int __neng_log_bits_empty(uint8_t *bits, int n)
{
    for (int idx = 0; idx < n; idx++)
    {
        if (bits[idx] != 0)
        {
            return 0;
        }
    }

    return 1;
}

inline void __neng_log_bits_clear(uint8_t *bits, int n)
{
    memset(bits, 0, n);
}

////////////////////////////////////////////////////////////////////
// System Function

#if defined(__LINUX__) || defined(__DARWIN__)

#include <sys/syscall.h>

int gettid(void)
{
    return syscall(SYS_gettid);
}

#endif //__LINUX__

int64_t get_systemtime_millisec(void)
{
    struct timeval tv = {0};

    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static pthread_mutex_t _hostname_mtx = PTHREAD_MUTEX_INITIALIZER;
static int _hostname_len = 0;
static char _hostname[128] = {0};
static int _progname_len = 0;
static char _progname[128] = {0};

static inline void __init_globals(void)
{
    pthread_mutex_lock(&_hostname_mtx);
    if (_hostname_len <= 0)
    {
        if (gethostname(_hostname, sizeof(_hostname) - 1) == 0)
        {
            _hostname_len = strlen(_hostname);
        }
    }
    pthread_mutex_unlock(&_hostname_mtx);
}

void get_hostname(char *buf, size_t n)
{
    if (_hostname_len <= 0)
    {
        __init_globals();
    }

    int m = 0;

    if (_hostname_len > 0)
    {
        m = (_hostname_len >= n) ? n - 1 : _hostname_len;
        memcpy(buf, _hostname, m);
    }

    buf[m] = '\0';
}

void NengLogSetProgName(const char *prog)
{
    char *last_sepator = strrchr(prog, PATH_SEPARATOR_CHAR);

    if (last_sepator != NULL)
    {
        prog = last_sepator + 1;
    }

    if (prog != NULL && '\0' != prog[0])
    {
        pthread_mutex_lock(&_hostname_mtx);
        strncpy(_progname, prog, sizeof(_progname) - 1);
        _progname_len = strlen(_progname);
        pthread_mutex_unlock(&_hostname_mtx);
    }
}

void get_progname(char *buf, size_t n)
{
    int m = 0;

    if (_progname_len > 0)
    {
        m = (_progname_len >= n) ? n - 1 : _progname_len;
        memcpy(buf, _progname, m);
    }

    buf[m] = '\0';
}

////////////////////////////////////////////////////////////////////
// log level function

static const char *_LevelNames[] = {
    "Emerg",
    "Alert",
    "Crit",
    "Error",
    "Warn",
    "Notice",
    "Info",
    "Debug"};

const char *NengLogLevel2Name(int level)
{
    if (level >= kNengLogMinLevel && level <= kNengLogMaxLevel)
    {
        return _LevelNames[level];
    }

    return "None";
}

static Code _Name2Level[] = {
    {"Emerg", kNengLogEmerg},
    {"Alert", kNengLogAlert},
    {"Crit", kNengLogCrit},
    {"Error", kNengLogError},
    {"Err", kNengLogError},
    {"Warn", kNengLogWarn},
    {"Warning", kNengLogWarn},
    {"Notice", kNengLogNotice},
    {"Info", kNengLogInfo},
    {"Debug", kNengLogDebug},
    {NULL, -1},
};

int NengLogName2Level(const char *name)
{
    if (name != NULL && name[0] != '\0')
    {
        for (int n = 0; n < COUNT_OF(_Name2Level) && _Name2Level[n].name != NULL; n++)
        {
            if (strcasecmp(name, _Name2Level[n].name) == 0)
            {
                return _Name2Level[n].val;
            }
        }
    }

    return -1;
}