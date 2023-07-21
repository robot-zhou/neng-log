#ifndef __NENG_LOG_PROPERTIES_MISC_H__
#define __NENG_LOG_PROPERTIES_MISC_H__

#include <neng/log/log.h>
#include <common/array.h>

#include "properties.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ARRAY_HEAD(stIntArray, int) IntArray;
ARRAY_PROTOTYPE(stIntArray, int, NULL, NULL, NULL, NULL);

int _string2ints(const char *s, IntArray *int_head);
int _string2levels(const char *s, IntArray *int_head);

int _LoadAppenderFilter(const char *prefix, PropertiesHead *root, NengLogFilter *filter);
int _LoadAppenderFlagsProperties(const char *prefix, PropertiesHead *root, NengLogAppender *appender);
int _LoadAppenderFilterProperties(const char *prefix, PropertiesHead *root, NengLogAppender *appender);
int _LoadAppenderProperties(const char *prefix, PropertiesHead *root, NengLogAppender *appender);

#ifdef __cplusplus
}
#endif

#endif