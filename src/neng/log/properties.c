#include "properties.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "log_misc.h"

int __property_cmp(const struct stProperty *p1, const struct stProperty *p2)
{
    return strcasecmp(p1->name, p2->name);
}

RB_GENERATE(stPropertiesHead, stProperty, rb_entry, __property_cmp);

typedef struct stPropertiesParser
{
    int status;
    int prev_space;
    int have_equal;
    int is_escape;
    astr_t key;
    astr_t val;
} PropertiesParser;

#define PROPERTIES_PARSER_INITIALIZER(x)                                                                                              \
    {                                                                                                                                 \
        .status = 0, .prev_space = 0, .have_equal = 0, .is_escape = 0, .key = ARRAY_INITIALIZER(.key), .val = ARRAY_INITIALIZER(.key) \
    }

#define PROPERTIES_PARSER_CLEAR(x)       \
    do                                   \
    {                                    \
        ARRAY_CLEAR(astr_, &((x)->key)); \
        ARRAY_CLEAR(astr_, &((x)->val)); \
        (x)->status = 0;                 \
        (x)->prev_space = 0;             \
        (x)->have_equal = 0;             \
        (x)->is_escape = 0;              \
    } while (0)

static int _properties_append_string(astr_t *s, char ch, int *escape)
{
    if (*escape == 0)
    {
        if (ch == '\\')
        {
            *escape = 1;
            return 0;
        }

        return ARRAY_APPEND(astr_, s, ch);
    }

    switch (ch)
    {
    case 'r':
        ARRAY_APPEND(astr_, s, '\r');
        break;
    case 'n':
        ARRAY_APPEND(astr_, s, '\n');
        break;
    case 't':
        ARRAY_APPEND(astr_, s, '\t');
        break;
    case '\\':
        ARRAY_APPEND(astr_, s, '\\');
        break;
    default:
        ARRAY_APPEND(astr_, s, '\\');
        ARRAY_APPEND(astr_, s, ch);
        break;
    }

    *escape = 0;
    return 0;
}

static int _properties_parser_write(PropertiesParser *p, const char *buf, int len, PropertiesHead *root)
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
                    Property *prop = (Property *)calloc(1, sizeof(Property));
                    if (prop != NULL)
                    {
                        prop->name = strdup(p->key.p);
                        if (p->val.p != NULL)
                        {
                            prop->property = strdup(p->val.p);
                        }

                        RB_INSERT(stPropertiesHead, root, prop);
                    }
                }

                ARRAY_CLEAR(astr_, &(p->key));
                ARRAY_CLEAR(astr_, &(p->val));
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
                ARRAY_APPEND_N(astr_, &(p->key), ' ', p->prev_space);
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
                ARRAY_APPEND_N(astr_, &(p->key), ' ', p->prev_space);
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

int _properties_read_file(const char *filepath, PropertiesHead *root)
{
    int fd = open(filepath, O_RDONLY, 0);

    if (fd == -1)
    {
        return -1;
    }

    int ret = 1;
    char buf[1024] = {0};
    PropertiesParser parser = PROPERTIES_PARSER_INITIALIZER(parser);

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
    return ret;
}

char *_properties_get(PropertiesHead *root, const char *name)
{
    char namebuf[512] = {0};
    Property *find = NULL;
    Property var = {.name = namebuf};

    strncpy(namebuf, name, sizeof(namebuf) - 1);
    find = RB_FIND(stPropertiesHead, root, &var);
    return (find != NULL) ? find->property : NULL;
}

int _properties_unittype_int(int val, char *endptr, int unit_type)
{
    if (endptr != NULL && endptr[0] != '\0')
    {
        if (unit_type == kPropertiesUnitTypeTime)
        {
            switch (*endptr)
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
        else if (unit_type == kPropertiesUnitTypeByte)
        {
            switch (*endptr)
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
    }

    return val;
}

int _properties_get_int(PropertiesHead *root, const char *name, int *out_val, int unit_type)
{
    char *p = _properties_get(root, name);
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
        val = _properties_unittype_int(val, endptr, unit_type);
        *out_val = val;
    }

    return 1;
}

int _properties_get_defint(PropertiesHead *root, const char *name, int dev_val, int unit_type)
{
    char *p = _properties_get(root, name);
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

    return _properties_unittype_int(val, endptr, unit_type);
}

int _properties_get_defbool(PropertiesHead *root, const char *name, int dev_val)
{
    char *p = _properties_get(root, name);
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

void _properties_clear(PropertiesHead *root)
{
    Property *v = NULL;

    while ((v = RB_MIN(stPropertiesHead, root)) != NULL)
    {
        RB_REMOVE(stPropertiesHead, root, v);
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
}
