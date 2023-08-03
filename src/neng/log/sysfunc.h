#ifndef __NENG_LOG_SYSFUNC_H__
#define __NENG_LOG_SYSFUNC_H__

#include <stdint.h>

__BEGIN_DECLS

int neng_log_sys_gettid(void);

int64_t neng_log_sys_gettime_millisec(void);
int64_t neng_log_sys_gettime_microsec(void);
long neng_log_sys_gettimezone(void);

__END_DECLS

#endif
