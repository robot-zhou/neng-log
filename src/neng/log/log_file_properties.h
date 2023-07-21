#ifndef __NENG_LOG_FILE_PROPERTIES_H__
#define __NENG_LOG_FILE_PROPERTIES_H__

#include <neng/log/log.h>

#include "properties.h"

#ifdef __cplusplus
extern "C" {
#endif

NengLogAppender *NengLogLoadFileProperties(const char *prefix, PropertiesHead *root);

#ifdef __cplusplus
}
#endif

#endif //__NENG_LOG_FILE_PROPERTIES_H__