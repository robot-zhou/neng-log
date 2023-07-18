#include "log_file_properties.h"

#include <string.h>
#include <stdio.h>
#include <neng/log/log_file.h>

#include "log_properties_misc.h"

NengLogAppender *NengLogLoadFileProperties(const char *prefix, PropertiesHead *root)
{
    char name[128] = {0};

    snprintf(name, sizeof(name), "%s.filepath", prefix);
    char *filepath = _properties_get(root, name);
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
    file_appender->max_size = _properties_get_defint(root, name, 0, kPropertiesUnitTypeByte);
    snprintf(name, sizeof(name), "%s.max_days", prefix);
    file_appender->max_days = _properties_get_defint(root, name, 0, 0);
    snprintf(name, sizeof(name), "%s.max_loop", prefix);
    file_appender->max_loop = _properties_get_defint(root, name, 0, 0);
    snprintf(name, sizeof(name), "%s.daily", prefix);
    file_appender->daily = _properties_get_defbool(root, name, 0);

    _LoadAppenderProperties(prefix, root, &(file_appender->appender));

    return &(file_appender->appender);
}
