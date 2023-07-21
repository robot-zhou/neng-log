#ifndef __NENG_LOG_SYSLOG_PROPERTIES_H__
#define __NENG_LOG_SYSLOG_PROPERTIES_H__

#include <neng/log/log.h>

#include "properties.h"

#ifdef __cplusplus
extern "C" {
#endif

NengLogAppender *NengLogLoadSyslogProperties(const char *prefix, PropertiesHead *root);

#ifdef __cplusplus
}
#endif

#endif