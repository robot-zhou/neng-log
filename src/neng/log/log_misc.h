#ifndef __NENG_LOG_MISC_H__
#define __NENG_LOG_MISC_H__

#include <stdint.h>
#include <stdio.h>

#include "log_def.h"

__BEGIN_DECLS

typedef struct stNengLogCode
{
    const char *name;
    int val;
} NengLogCode;

// system function
void __neng_log_get_hostname(char *buf, size_t n);
void __neng_log_get_progname(char *buf, size_t n);

// bits function
int __neng_log_bits_set(uint8_t *bits, int n, int val);
int __neng_log_bits_get(uint8_t *bits, int n, int val);
int __neng_log_bits_empty(uint8_t *bits, int n);
void __neng_log_bits_clear(uint8_t *bits, int n);

__END_DECLS

#endif