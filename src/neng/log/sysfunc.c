#include "sysfunc.h"

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#if defined(__LINUX__)

#include <sys/syscall.h>

int neng_log_sys_gettid(void)
{
    return syscall(SYS_gettid);
}

#elif defined(__DARWIN__) // #if defined(__LINUX__)

#include <pthread.h>

int gettid(void)
{
    uint64_t id = 0;
    pthread_threadid_np(NULL, &id);
    return (int)id;
}

#endif // #elif defined(__DARWIN__)

int64_t neng_log_sys_gettime_microsec(void)
{
    struct timeval tv = {0};

    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

int64_t neng_log_sys_gettime_millisec(void)
{
    return neng_log_sys_gettime_microsec() / 1000;
}

long neng_log_sys_gettimezone(void)
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    return (tm != NULL) ? (long)(tm->tm_gmtoff) : 0;
}