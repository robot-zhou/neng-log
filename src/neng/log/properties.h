#ifndef __NENG_PROPERTIES_H__
#define __NENG_PROPERTIES_H__

#include <stdint.h>

__BEGIN_DECLS

typedef enum eNengLogPropertiesUnitType
{
    kNengLogPropertiesUnitTypeTime = 1,
    kNengLogPropertiesUnitTypeByte = 2
} kNengLogPropertiesUnitType;

typedef void *NengLogPropertiesHandler;

NengLogPropertiesHandler _neng_log_properties_read_file(const char *filepath);
void _neng_log_properties_clear(NengLogPropertiesHandler handler);

char *_neng_log_properties_get(NengLogPropertiesHandler handler, const char *name);
int _neng_log_properties_get_int(NengLogPropertiesHandler handler, const char *name, int *out_val, int unit_type);
int _neng_log_properties_get_defint(NengLogPropertiesHandler handler, const char *name, int dev_val, int unit_type);
int _neng_log_properties_get_defbool(NengLogPropertiesHandler handler, const char *name, int dev_val);

int _neng_log_properties_unittype_int(int val, char endchar, int unit_type);

__END_DECLS

#endif // __NENG_PROPERTIES_H__