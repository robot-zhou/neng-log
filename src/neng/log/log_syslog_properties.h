#ifndef __NENG_LOG_SYSLOG_PROPERTIES_H__
#define __NENG_LOG_SYSLOG_PROPERTIES_H__

#include <neng/log/log.h>

#include "properties.h"

__BEGIN_DECLS

NengLogAppender *NengLogLoadSyslogProperties(const char *prefix, NengLogPropertiesHandler handler);

__END_DECLS

#endif