#include "log_syslog_properties.h"

#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

#include <neng/log/log_syslog.h>

#include "log_filter.h"
#include "log_misc.h"
#include "log_properties_misc.h"

static NengLogCode _SyslogOptionNames[] = {
    {"LOG_PID", LOG_PID},
    {"PID", LOG_PID},
    {"LOG_CONS", LOG_CONS},
    {"CONS", LOG_CONS},
    {"LOG_ODELAY", LOG_ODELAY},
    {"ODELAY", LOG_ODELAY},
    {"LOG_NDELAY", LOG_NDELAY},
    {"NDELAY", LOG_NDELAY},
    {"LOG_PERROR", LOG_PERROR},
    {"PERROR", LOG_PERROR},
    {NULL, -1}};

static int _string2syslogoption(const char *s)
{
    int option = 0;

    if (s != NULL && s[0] != '\0')
    {
        char *tmp = strdup(s);

        if (tmp != NULL)
        {
            char *s_ptr = NULL;

            for (char *sub = strtok_r(tmp, ",", &s_ptr); sub != NULL; sub = strtok_r(NULL, ",", &s_ptr))
            {
                for (int n = 0; n < COUNT_OF(_SyslogOptionNames) && _SyslogOptionNames[n].name != NULL; n++)
                {
                    if (strcasecmp(sub, _SyslogOptionNames[n].name) == 0)
                    {
                        option |= _SyslogOptionNames[n].val;
                        break;
                    }
                }
            }

            free(tmp);
        }
    }

    return option;
}

static NengLogCode _FacilityNames[] = {
    {"auth", LOG_AUTH},
    {"authpriv", LOG_AUTHPRIV},
    {"cron", LOG_CRON},
    {"daemon", LOG_DAEMON},
    {"ftp", LOG_FTP},
    {"kern", LOG_KERN},
    {"lpr", LOG_LPR},
    {"mail", LOG_MAIL},
    {"news", LOG_NEWS},
    {"security", LOG_AUTH}, /* DEPRECATED */
    {"syslog", LOG_SYSLOG},
    {"user", LOG_USER},
    {"uucp", LOG_UUCP},
    {"local0", LOG_LOCAL0},
    {"local1", LOG_LOCAL1},
    {"local2", LOG_LOCAL2},
    {"local3", LOG_LOCAL3},
    {"local4", LOG_LOCAL4},
    {"local5", LOG_LOCAL5},
    {"local6", LOG_LOCAL6},
    {"local7", LOG_LOCAL7},
    {NULL, -1}};

static int _string2facility(const char *s)
{
    if (s != NULL && s[0] != '\0')
    {
        for (int n = 0; n < COUNT_OF(_FacilityNames) && _FacilityNames[n].name != NULL; n++)
        {
            if (strcasecmp(s, _FacilityNames[n].name) == 0)
            {
                return _FacilityNames[n].val;
            }
        }
    }

    return 0;
}

NengLogAppender *NengLogLoadSyslogProperties(const char *prefix, NengLogPropertiesHandler handler)
{
    char name[128] = {0};

    snprintf(name, sizeof(name), "%s.ident", prefix);
    char *ident = _neng_log_properties_get(handler, name);

    snprintf(name, sizeof(name), "%s.option", prefix);
    int option = _string2syslogoption(_neng_log_properties_get(handler, name));

    snprintf(name, sizeof(name), "%s.facility", prefix);
    int facility = _string2facility(_neng_log_properties_get(handler, name));

    snprintf(name, sizeof(name), "%s.override_facility", prefix);
    int override_facility = _neng_log_properties_get_defbool(handler, name, 0);

    NengLogSyslogAppender *sys_appender = NengLogCreateSyslogAppender(ident, option, facility, override_facility);
    if (sys_appender == NULL)
    {
        return NULL;
    }

    for (int n = 0;; n++)
    {
        NengLogSyslogFilter sys_filter = {0};

        snprintf(name, sizeof(name), "%s.facility_filter[%d].facility", prefix, n);
        const char *faility_pr = _neng_log_properties_get(handler, name);
        if (faility_pr == NULL)
        {
            if (n > 0)
            {
                break;
            }

            continue;
        }

        sys_filter.facility = _string2facility(faility_pr);
        if (sys_filter.facility == 0)
        {
            continue;
        }

        snprintf(name, sizeof(name), "%s.facility_filter[%d]", prefix, n);
        if (_NengLogLoadFilterProperties(name, handler, &(sys_filter.filter)) != 0)
        {
            continue;
        }

        if (NengLogFilterIsEmpty(&(sys_filter.filter)))
        {
            continue;
        }

        NengLogSyslogAppenderAddFilter(sys_appender, &sys_filter);
    }

    _NengLogLoadAppenderProperties(prefix, handler, &(sys_appender->appender));
    return &(sys_appender->appender);
}