#include "log_properties_misc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <common/array.h>

#include "log_filter.h"

typedef ARRAY_HEAD(stIntArray, int) IntArray;
ARRAY_GENERATE_STATIC(stIntArray, int, NULL, NULL, NULL, NULL);

static int _string2ints(const char *s, IntArray *int_head)
{
    char *tmp = strdup(s);
    if (tmp == NULL)
    {
        return -1;
    }

    char *s_ptr = NULL;
    for (char *sub = strtok_r(tmp, ",", &s_ptr); sub != NULL; sub = strtok_r(NULL, ",", &s_ptr))
    {
        char *endptr = NULL;
        int v = strtol(sub, &endptr, 10);
        if (errno != 0)
        {
            continue;
        }

        ARRAY_APPEND(stIntArray, int_head, v);
    }

    free(tmp);
    return 0;
}

static int _string2levels(const char *s, IntArray *int_head)
{
    char *tmp = strdup(s);
    if (tmp == NULL)
    {
        return -1;
    }

    char *s_ptr = NULL;
    for (char *sub = strtok_r(tmp, ",", &s_ptr); sub != NULL; sub = strtok_r(NULL, ",", &s_ptr))
    {
        int level = NengLogName2Level(sub);
        if (level != -1)
        {
            ARRAY_APPEND(stIntArray, int_head, level);
        }
    }

    free(tmp);
    return 0;
}

int _NengLogLoadFilterProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogFilter *filter)
{
    char name[128] = {0};

    snprintf(name, sizeof(name), "%s.mods", prefix);
    char *pmods = _neng_log_properties_get(handler, name);

    snprintf(name, sizeof(name), "%s.tags", prefix);
    char *ptags = _neng_log_properties_get(handler, name);

    snprintf(name, sizeof(name), "%s.levels", prefix);
    char *plevels = _neng_log_properties_get(handler, name);

    if (pmods == NULL && ptags == NULL && plevels == NULL)
    {
        return -1;
    }

    if (pmods != NULL)
    {
        IntArray mods = ARRAY_INITIALIZER(mods);

        _string2ints(pmods, &mods);
        for (int n = 0; n < mods.len; n++)
        {
            NengLogFilterSetModBit(filter, mods.p[n]);
        }

        ARRAY_CLEAR(stIntArray, &mods);
    }

    if (ptags != NULL)
    {
        IntArray tags = ARRAY_INITIALIZER(mods);

        _string2ints(ptags, &tags);
        for (int n = 0; n < tags.len; n++)
        {
            NengLogFilterSetTagBit(filter, tags.p[n]);
        }

        ARRAY_CLEAR(stIntArray, &tags);
    }

    if (plevels != NULL)
    {
        IntArray levels = ARRAY_INITIALIZER(levels);

        _string2levels(plevels, &levels);
        for (int n = 0; n < levels.len; n++)
        {
            NengLogFilterSetLevelBit(filter, levels.p[n]);
        }

        ARRAY_CLEAR(stIntArray, &levels);
    }

    return 0;
}

int _NengLogLoadAppenderFilterProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogAppender *appender)
{
    char name[128] = {0};

    for (int n = 0;; n++)
    {
        NengLogFilter filter = {0};
        snprintf(name, sizeof(name), "%s.filter[%d]", prefix, n);

        if (_NengLogLoadFilterProperties(name, handler, &filter) != 0)
        {
            if (n > 0)
            {
                break;
            }

            continue;
        }

        if (NengLogFilterIsEmpty(&filter) == 0)
        {
            NengLogAppenderAddFilter(appender, &filter);
        }
    }

    return 0;
}

int _NengLogLoadAppenderFlagsProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogAppender *appender)
{
    char name[128] = {0};

    // flags
    snprintf(name, sizeof(name), "%s.flags.disable_time", prefix);
    appender->flags.disable_time = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_hostname", prefix);
    appender->flags.enable_hostname = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_progname", prefix);
    appender->flags.enable_progname = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.disable_pid", prefix);
    appender->flags.disable_pid = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.disable_level", prefix);
    appender->flags.disable_level = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_tid", prefix);
    appender->flags.enable_tid = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_mod", prefix);
    appender->flags.enable_mod = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_tag", prefix);
    appender->flags.enable_tag = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_file", prefix);
    appender->flags.enable_file = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_func", prefix);
    appender->flags.enable_func = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_fullpath", prefix);
    appender->flags.enable_fullpath = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.enable_tab_separator", prefix);
    appender->flags.enable_tab_separator = _neng_log_properties_get_defbool(handler, name, 0);

    snprintf(name, sizeof(name), "%s.flags.disable_async", prefix);
    int disable_async = 0;
    if (1 == _neng_log_properties_get_int(handler, name, &disable_async, 0) && disable_async)
    {
        appender->flags.disable_async = 1;
    }

    return 0;
}

int _NengLogLoadAppenderProperties(const char *prefix, NengLogPropertiesHandler handler, NengLogAppender *appender)
{
    _NengLogLoadAppenderFlagsProperties(prefix, handler, appender);
    _NengLogLoadAppenderFilterProperties(prefix, handler, appender);
    return 0;
}
