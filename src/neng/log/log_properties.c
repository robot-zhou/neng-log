#include <neng/log/log.h>
#include <neng/log/log_file.h>
#include <neng/log/log_syslog.h>
#include <neng/log/log_properties.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "properties.h"
#include "log_properties_misc.h"
#include "log_syslog_properties.h"
#include "log_file_properties.h"

int NengLogLoadProperties(const char *filepath)
{
    NengLogPropertiesHandler handler = _neng_log_properties_read_file(filepath);

    if (handler == NULL)
    {
        return -1;
    }

    // load global level
    const char *level = _neng_log_properties_get(handler, "log.level");
    NengLogSetLevel(NengLogName2Level(level), 0);

    // load mod level
    for (int n = 1; n < NENG_LOG_MOD_SIZE; n++)
    {
        char name[128] = {0};
        snprintf(name, sizeof(name), "log.level.mod.%d", n);

        const char *level = _neng_log_properties_get(handler, name);
        if (level)
        {
            NengLogSetLevel(NengLogName2Level(level), n);
        }
    }

    char *appenders = _neng_log_properties_get(handler, "log.appenders");
    appenders = (appenders != NULL) ? strdup(appenders) : NULL;

    if (appenders)
    {
        const char *sep = ",";
        char *s_ptr = NULL;

        for (char *item = strtok_r(appenders, sep, &s_ptr); item != NULL; item = strtok_r(NULL, sep, &s_ptr))
        {
            char name[128] = {0};
            NengLogAppender *appender = NULL;

            snprintf(name, sizeof(name), "log.appender.%s.type", item);
            char *type = _neng_log_properties_get(handler, name);
            if (type == NULL || type[0] == '\0')
            {
                continue;
            }

            if (strcasecmp(type, "file") == 0)
            {
                snprintf(name, sizeof(name), "log.appender.%s", item);
                appender = NengLogLoadFileProperties(name, handler);
            }
            else if (strcasecmp(type, "syslog") == 0)
            {
                snprintf(name, sizeof(name), "log.appender.%s", item);
                appender = NengLogLoadSyslogProperties(name, handler);
            }
            else
            {
                continue;
            }

            if (appender != NULL)
            {
                NengLogAddAppender(appender);
            }
        } // for()

        free(appenders);
    } // if (appenders)

    _neng_log_properties_clear(handler);
    return 0;
}