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
    PropertiesHead root = RB_INITIALIZER(root);

    if (0 != _properties_read_file(filepath, &root))
    {
        return -1;
    }

    // load global level
    const char *level = _properties_get(&root, "log.level");
    NengLogSetLevel(NengLogName2Level(level), 0);

    // load mod level
    for (int n = 1; n < NENG_LOG_MOD_SIZE; n++)
    {
        char name[128] = {0};
        snprintf(name, sizeof(name), "log.level.mod.%d", n);

        const char *level = _properties_get(&root, name);
        if (level)
        {
            NengLogSetLevel(NengLogName2Level(level), n);
        }
    }

    char *appenders = _properties_get(&root, "log.appenders");
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
            char *type = _properties_get(&root, name);
            if (type == NULL || type[0] == '\0')
            {
                continue;
            }

            if (strcasecmp(type, "file") == 0)
            {
                snprintf(name, sizeof(name), "log.appender.%s", item);
                appender = NengLogLoadFileProperties(name, &root);
            }
            else if (strcasecmp(type, "syslog") == 0)
            {
                snprintf(name, sizeof(name), "log.appender.%s", item);
                appender = NengLogLoadSyslogProperties(name, &root);
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

    _properties_clear(&root);
    return 0;
}