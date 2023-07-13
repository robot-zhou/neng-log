#ifndef __NENG_LOG_SYSLOG_PROPERTIES_H__
#define __NENG_LOG_SYSLOG_PROPERTIES_H__

#include <sys/cdefs.h>
#include <neng/log/log.h>

#include "properties.h"

__BEGIN_DECLS

NengLogAppender *NengLogLoadSyslogProperties(const char *prefix, PropertiesHead *root);

__END_DECLS

#endif