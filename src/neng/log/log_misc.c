#include "log_misc.h"

#include <neng/log/log.h>

#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
#include <sys/time.h>

////////////////////////////////////////////////////////////////////
// NengLogAppender Bit Function
inline int __neng_log_bits_set(uint8_t *bits, int n, int val)
{
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
