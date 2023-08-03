#ifndef __NENG_LOG_FILE_PROPERTIES_H__
#define __NENG_LOG_FILE_PROPERTIES_H__

#include <neng/log/log.h>

#include "properties.h"

__BEGIN_DECLS

NengLogAppender *NengLogLoadFileProperties(const char *prefix, NengLogPropertiesHandler handler);

__END_DECLS

#endif //__NENG_LOG_FILE_PROPERTIES_H__