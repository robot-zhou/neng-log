#include "properties.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/array.h>
#include <common/tree.h>

#include "log_misc.h"

////////////////////////////////////////////////////////////////////
// astr
typedef ARRAY_HEAD(neng_log_astr_, char) neng_log_astr_t;
ARRAY_GENERATE_STATIC(neng_log_astr_, char, NULL, NULL, NULL, NULL);

////////////////////////////////////////////////////////////////////
// properity
typedef struct stNengLogProperty
{
    RB_ENTRY(stNengLogProperty)
    rb_entry;
    char *name;
    char *property;
} NengLogProperty;

static int __neng_log_property_cmp(const struct stNengLogProperty *p1, const struct stNengLogProperty *p2)
{
    return strcasecmp(p1->name, p2->name);
}

typedef RB_HEAD(stNengLogPropertiesHead, stNengLogProperty) NengLogPropertiesHead;
RB_GENERATE_STATIC(stNengLogPropertiesHead, stNengLogProperty, rb_entry, __neng_log_property_cmp);

typedef struct stPropertiesParser
{
    int status;
    int prev_space;
    int have_equal;
    int is_escape;
    neng_log_astr_t key;
    neng_log_astr_t val;
} PropertiesParser;

#define PROPERTIES_PARSER_INITIALIZER(x)                                                                                              \
    {                                                                                                                                 \
        .status = 0, .prev_space = 0, .have_equal = 0, .is_escape = 0, .key = ARRAY_INITIALIZER(.key), .val = ARRAY_INITIALIZER(.key) \
    }

#define PROPERTIES_PARSER_CLEAR(x)                \
    do                                            \
    {                                             \
        ARRAY_CLEAR(neng_log_astr_, &((x)->key)); \
        ARRAY_CLEAR(neng_log_astr_, &((x)->val)); \
        (x)->status = 0;                          \
        (x)->prev_space = 0;                      \
        (x)->have_equal = 0;                      \
        (x)->is_escape = 0;                       \
    } while (0)

static int _properties_append_string(neng_log_astr_t *s, char ch, int *escape)
{
    if (*escape == 0)
    {
        if (ch == '\\')
        {
            *escape = 1;
            return 0;
        }

        return ARRAY_APPEND(neng_log_astr_, s, ch);
    }

    switch (ch)
    {
    case 'r':
        ARRAY_APPEND(neng_log_astr_, s, '\r');
        break;
    case 'n':
        ARRAY_APPEND(neng_log_astr_, s, '\n');
        break;
    case 't':
        ARRAY_APPEND(neng_log_astr_, s, '\t');
        break;
    case '\\':
        ARRAY_APPEND(neng_log_astr_, s, '\\');
        break;
    default:
        ARRAY_APPEND(neng_log_astr_, s, '\\');
        ARRAY_APPEND(neng_log_astr_, s, ch);
        break;
    }

    *escape = 0;
    return 0;
}

static int _properties_parser_write(PropertiesParser *p, const char *buf, int len, NengLogPropertiesHead *root)
{
    // <sapce:0> <key_name:1> <space:2> <=:3> <space:4> <value:5> <#xxx:7>, 8: error
    // .........<line wrap: 6>
    for (int m = 0; m < len; m++)
    {
        char ch = buf[m];

        if (ch == '\r' && m < len && ((ch == '\n') || (ch == '\0')))
        {
            continue;
        }

        if (ch == ' ' || ch == '\t')
        {
            switch (p->status)
            {
            case 0:
                break;
            case 1:
                p->is_escape = 0;
                p->prev_space++;
                p->status = 2;
                break;
            case 2:
                p->prev_space++;
                break;
            case 3:
                p->status = 4;
                break;
            case 4:
                break;
            case 5:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                break;
            case 6:
                break;
            case 7:
                break;
            default:
                break;
            }
        }
        else if (ch == '=' || ch == ':')
        {
            switch (p->status)
            {
            case 0:
                p->have_equal = 1;
                p->status = 3;
                break;
            case 1:
                p->have_equal = 1;
                p->is_escape = 0;
                p->status = 3;
                break;
            case 2:
                p->have_equal = 1;
                p->prev_space = 0;
                p->status = 3;
                break;
            case 3:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 4:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 5:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                break;
            case 6:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 7:
                break;
            default:
                break;
            }
        }
        else if (ch == '\n' || ch == '\r' || ch == '\0')
        {
            if (p->is_escape && p->status >= 3 && ch != '\0')
            {
                p->is_escape = 0;
                p->status = 6;
            }
            else
            {
                if (p->have_equal && p->key.len > 0)
                {
                    NengLogProperty *prop = (NengLogProperty *)calloc(1, sizeof(NengLogProperty));
                    if (prop != NULL)
                    {
                        prop->name = strdup(p->key.p);
                        if (p->val.p != NULL)
                        {
                            prop->property = strdup(p->val.p);
                        }

                        RB_INSERT(stNengLogPropertiesHead, root, prop);
                    }
                }

                ARRAY_CLEAR(neng_log_astr_, &(p->key));
                ARRAY_CLEAR(neng_log_astr_, &(p->val));
                p->status = 0;
                p->prev_space = 0;
                p->have_equal = 0;
            }
        }
        else if (ch == '#')
        {
            p->status = 7;
        }
        else if (ch == '\\')
        {
            switch (p->status)
            {
            case 0:
                p->is_escape = 1;
                p->status = 1;
                break;
            case 1:
                _properties_append_string(&(p->key), ch, &(p->is_escape));
                break;
            case 2:
                ARRAY_APPEND_N(neng_log_astr_, &(p->key), ' ', p->prev_space);
                if (p->prev_space > 0)
                {
                    p->is_escape = 0;
                }
                _properties_append_string(&(p->key), ch, &(p->is_escape));
                p->prev_space = 0;
                p->status = 1;
                break;
            case 3:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 4:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 5:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                break;
            case 6:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 7:
                break;
            default:
                break;
            }
        }
        else
        {
            switch (p->status)
            {
            case 0:
                _properties_append_string(&(p->key), ch, &(p->is_escape));
                p->status = 1;
                break;
            case 1:
                _properties_append_string(&(p->key), ch, &(p->is_escape));
                break;
            case 2:
                ARRAY_APPEND_N(neng_log_astr_, &(p->key), ' ', p->prev_space);
                if (p->prev_space > 0)
                {
                    p->is_escape = 0;
                }
                _properties_append_string(&(p->key), ch, &(p->is_escape));
                p->prev_space = 0;
                p->status = 1;
                break;
            case 3:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 4:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 5:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                break;
            case 6:
                _properties_append_string(&(p->val), ch, &(p->is_escape));
                p->status = 5;
                break;
            case 7:
                break;
            default:
                break;
            }
        }
    } // for

    return 0;
}

NengLogPropertiesHandler _neng_log_properties_read_file(const char *filepath)
{
    int fd = open(filepath, O_RDONLY, 0);

    if (fd == -1)
    {
        return NULL;
    }

    int ret = 1;
    char buf[1024] = {0};
    PropertiesParser parser = PROPERTIES_PARSER_INITIALIZER(parser);
    NengLogPropertiesHead *root = calloc(sizeof(NengLogPropertiesHead), 1);

    if (root == NULL)
    {
        return NULL;
    }

    RB_INIT(root);
    
    do
    {
        ret = read(fd, buf, sizeof(buf));
        if (ret < 0)
        {
            break;
        }

        int len = ret;
        if (len == 0)
        {
            buf[len++] = '\0';
        }

        _properties_parser_write(&parser, buf, len, root);
    } while (ret > 0);

    PROPERTIES_PARSER_CLEAR(&parser);
    close(fd);

    if (ret < 0)
    {
        int lerrno = errno;
        free(root);
        errno = lerrno;
        return NULL;
    }

    return (NengLogPropertiesHandler)root;
}

static char _null_str[] = "\0";
char *_neng_log_properties_get(NengLogPropertiesHandler handler, const char *name)
{
    NengLogPropertiesHead *root = (NengLogPropertiesHead *)handler;
    char namebuf[512] = {0};
    NengLogProperty *find = NULL;
    NengLogProperty var = {.name = namebuf};

    strncpy(namebuf, name, sizeof(namebuf) - 1);
    find = RB_FIND(stNengLogPropertiesHead, root, &var);
    return (find != NULL) ? (find->property ? find->property : _null_str) : NULL;
}

int _neng_log_properties_unittype_int(int val, char endchar, int unit_type)
{
    if (unit_type == kNengLogPropertiesUnitTypeTime)
    {
        switch (endchar)
        {
        case 'H':
        case 'h':
            val *= 3600;
            break;
        case 'M':
        case 'm':
            val *= 60;
            break;
        default:
            break;
        }
    }
    else if (unit_type == kNengLogPropertiesUnitTypeByte)
    {
        switch (endchar)
        {
        case 'K':
        case 'k':
            val *= 1024;
            break;
        case 'M':
        case 'm':
            val *= (1024 * 1024);
            break;
        case 'G':
        case 'g':
            val *= (1024 * 1024 * 1024);
        default:
            break;
        }
    }

    return val;
}

int _neng_log_properties_get_int(NengLogPropertiesHandler handler, const char *name, int *out_val, int unit_type)
{
    NengLogPropertiesHead *root = (NengLogPropertiesHead *)handler;
    char *p = _neng_log_properties_get(root, name);

    if (p == NULL)
    {
        return 0;
    }

    char *endptr = NULL;
    int val = strtol(p, &endptr, 10);
    if (errno != 0)
    {
        return -1;
    }

    if (*out_val)
    {
        val = _neng_log_properties_unittype_int(val, endptr ? *endptr : '\0', unit_type);
        *out_val = val;
    }

    return 1;
}

int _neng_log_properties_get_defint(NengLogPropertiesHandler handler, const char *name, int dev_val, int unit_type)
{
    NengLogPropertiesHead *root = (NengLogPropertiesHead *)handler;
    char *p = _neng_log_properties_get(root, name);

    if (p == NULL)
    {
        return dev_val;
    }

    char *endptr = NULL;
    int val = strtol(p, &endptr, 10);
    if (errno != 0)
    {
        return dev_val;
    }

    return _neng_log_properties_unittype_int(val, endptr ? *endptr : '\0', unit_type);
}

int _neng_log_properties_get_defbool(NengLogPropertiesHandler handler, const char *name, int dev_val)
{
    NengLogPropertiesHead *root = (NengLogPropertiesHead *)handler;
    char *p = _neng_log_properties_get(root, name);

    if (p == NULL)
    {
        return dev_val;
    }

    if (strcasecmp(p, "on") == 0 || strcasecmp(p, "true") == 0 || strcasecmp(p, "yes") == 0)
    {
        return 1;
    }

    return 0;
}

void _neng_log_properties_clear(NengLogPropertiesHandler handler)
{
    NengLogPropertiesHead *root = (NengLogPropertiesHead *)handler;
    NengLogProperty *v = NULL;

    while ((v = RB_MIN(stNengLogPropertiesHead, root)) != NULL)
    {
        RB_REMOVE(stNengLogPropertiesHead, root, v);
        if (v->name)
        {
            free(v->name);
        }
        if (v->property)
        {
            free(v->property);
        }
        free(v);
    }

    RB_INIT(root);
    free(root);
}
