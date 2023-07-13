#ifndef __NENG_PROPERTIES_H__
#define __NENG_PROPERTIES_H__

#include <sys/cdefs.h>
#include <common/tree.h>

__BEGIN_DECLS

typedef enum ePropertiesUnitType
{
    kPropertiesUnitTypeTime = 1,
    kPropertiesUnitTypeByte = 2
} kPropertiesUnitType;

typedef struct stProperty
{
    RB_ENTRY(stProperty)
    rb_entry;
    char *name;
    char *property;
} Property;

typedef RB_HEAD(stPropertiesHead, stProperty) PropertiesHead;
int __property_cmp(const struct stProperty *p1, const struct stProperty *p2);
RB_PROTOTYPE(stPropertiesHead, stProperty, rb_entry, __property_cmp);

int _properties_read_file(const char *filepath, PropertiesHead *root);
void _properties_clear(PropertiesHead *root);

char *_properties_get(PropertiesHead *root, const char *name);
int _properties_unittype_int(int val, char *endptr, int unit_type);
int _properties_get_int(PropertiesHead *root, const char *name, int *out_val, int unit_type);
int _properties_get_defint(PropertiesHead *root, const char *name, int dev_val, int unit_type);
int _properties_get_defbool(PropertiesHead *root, const char *name, int dev_val);

__END_DECLS

#endif // __NENG_PROPERTIES_H__