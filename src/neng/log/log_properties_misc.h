#ifndef __NENG_LOG_PROPERTIES_MISC_H__
#define __NENG_LOG_PROPERTIES_MISC_H__

#include <neng/log/log.h>

#include "properties.h"

__BEGIN_DECLS

int _NengLogLoadFilterProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogFilter *filter);
int _NengLogLoadAppenderFlagsProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogAppender *appender);
int _NengLogLoadAppenderFilterProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogAppender *appender);
int _NengLogLoadAppenderProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogAppender *appender);

__END_DECLS

#endif