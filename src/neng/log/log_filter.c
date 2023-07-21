#include <neng/log/log.h>

#include "log_filter.h"
#include "log_misc.h"

////////////////////////////////////////////////////////////////////
// NengLogFilter Bit Function
// Mod Bits
int NengLogFilterSetModBit(NengLogFilter *filter, int mod)
{
    return __neng_log_bits_set(filter->mod_bits, COUNT_OF(filter->mod_bits), mod);
}

int NengLogFilterGetModBit(NengLogFilter *filter, int mod)
{
    return __neng_log_bits_get(filter->mod_bits, COUNT_OF(filter->mod_bits), mod);
}

int NengLogFilterModBitIsEmpty(NengLogFilter *filter)
{
    return __neng_log_bits_empty(filter->mod_bits, COUNT_OF(filter->mod_bits));
}

void NengLogFilterClearModBit(NengLogFilter *filter)
{
    __neng_log_bits_clear(filter->mod_bits, COUNT_OF(filter->mod_bits));
}

// Tag Bits
int NengLogFilterSetTagBit(NengLogFilter *filter, int tag)
{
    return __neng_log_bits_set(filter->tag_bits, COUNT_OF(filter->tag_bits), tag);
}

int NengLogFilterGetTagBit(NengLogFilter *filter, int tag)
{
    return __neng_log_bits_get(filter->tag_bits, COUNT_OF(filter->tag_bits), tag);
}

int NengLogFilterTagBitIsEmpty(NengLogFilter *filter)
{
    return __neng_log_bits_empty(filter->tag_bits, COUNT_OF(filter->tag_bits));
}

void NengLogFilterClearTagBit(NengLogFilter *filter)
{
    __neng_log_bits_clear(filter->tag_bits, COUNT_OF(filter->tag_bits));
}

// Level Bits
int NengLogFilterSetLevelBit(NengLogFilter *filter, int level)
{
    return __neng_log_bits_set(filter->level_bits, COUNT_OF(filter->level_bits), level - kNengLogMinLevel);
}

int NengLogFilterGetLevelBit(NengLogFilter *filter, int level)
{
    return __neng_log_bits_get(filter->level_bits, COUNT_OF(filter->level_bits), level - kNengLogMinLevel);
}

int NengLogFilterLevelBitIsEmpty(NengLogFilter *filter)
{
    return __neng_log_bits_empty(filter->level_bits, COUNT_OF(filter->level_bits));
}

void NengLogFilterClearLevelBit(NengLogFilter *filter)
{
    __neng_log_bits_clear(filter->level_bits, COUNT_OF(filter->level_bits));
}

void NengLogFilterClear(NengLogFilter *filter)
{
    NengLogFilterClearModBit(filter);
    NengLogFilterClearTagBit(filter);
    NengLogFilterClearLevelBit(filter);
}

int NengLogFilterIsEmpty(NengLogFilter *filter)
{
    if (NengLogFilterModBitIsEmpty(filter) && NengLogFilterTagBitIsEmpty(filter) && NengLogFilterLevelBitIsEmpty(filter))
    {
        return 1;
    }

    return 0;
}

int NengLogFilterHit(NengLogFilter *filter, int mod, int tag, int level)
{
    if (NengLogFilterModBitIsEmpty(filter) == 0)
    {
        if (mod < 0 || NengLogFilterGetModBit(filter, mod) != 1)
        {
            return 0;
        }
    }

    if (NengLogFilterTagBitIsEmpty(filter) == 0)
    {
        if (tag < 0 || NengLogFilterGetTagBit(filter, tag) != 1)
        {
            return 0;
        }
    }

    if (NengLogFilterLevelBitIsEmpty(filter) == 0)
    {
        if (level < 0 || NengLogFilterGetLevelBit(filter, level) != 1)
        {
            return 0;
        }
    }

    return 1;
}