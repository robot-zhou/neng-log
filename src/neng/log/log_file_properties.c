#include "log_file_properties.h"

#include <string.h>
#include <stdio.h>
#include <neng/log/log_file.h>

#include "log_properties_misc.h"

NengLogAppender *NengLogLoadFileProperties(const char *prefix, NengLogPropertiesHandler handler)
{
    char name[128] = {0};

    snprintf(name, sizeof(name), "%s.filepath", prefix);
    char *filepath = _neng_log_properties_get(handler, name);
    if (filepath == NULL)
    {
        return NULL;
    }

    NengLogFileAppender *file_appender = NengLogCreateFileAppender(filepath);
    if (file_appender == NULL)
    {
        return NULL;
    }

    snprintf(name, sizeof(name), "%s.max_size", prefix);
    file_appender->max_size = _neng_log_properties_get_defint(handler, name, 0, kNengLogPropertiesUnitTypeByte);
    snprintf(name, sizeof(name), "%s.max_days", prefix);
    file_appender->max_days = _neng_log_properties_get_defint(handler, name, 0, 0);
    snprintf(name, sizeof(name), "%s.max_loop", prefix);
    file_appender->max_loop = _neng_log_properties_get_defint(handler, name, 0, 0);
    snprintf(name, sizeof(name), "%s.daily", prefix);
    file_appender->daily = _neng_log_properties_get_defbool(handler, name, 0);

    _NengLogLoadAppenderProperties(prefix, handler, &(file_appender->appender));

    return &(file_appender->appender);
}
