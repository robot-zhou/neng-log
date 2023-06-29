#include <neng/log/log.h>

#include "log_misc.h"

////////////////////////////////////////////////////////////////////
// NengLogMatch Bit Function
// Mod Bits
int NengLogMatchSetModBit(NengLogMatch *match, int mod)
{
    return __neng_log_bits_set(match->mod_bits, COUNT_OF(match->mod_bits), mod);
}

int NengLogMatchGetModBit(NengLogMatch *match, int mod)
{
    return __neng_log_bits_get(match->mod_bits, COUNT_OF(match->mod_bits), mod);
}

int NengLogMatchModBitIsEmpty(NengLogMatch *match)
{
    return __neng_log_bits_empty(match->mod_bits, COUNT_OF(match->mod_bits));
}

void NengLogMatchClearModBit(NengLogMatch *match)
{
    __neng_log_bits_clear(match->mod_bits, COUNT_OF(match->mod_bits));
}

// Tag Bits
int NengLogMatchSetTagBit(NengLogMatch *match, int mod)
{
    return __neng_log_bits_set(match->tag_bits, COUNT_OF(match->tag_bits), mod);
}

int NengLogMatchGetTagBit(NengLogMatch *match, int mod)
{
    return __neng_log_bits_get(match->tag_bits, COUNT_OF(match->tag_bits), mod);
}

int NengLogMatchTagBitIsEmpty(NengLogMatch *match)
{
    return __neng_log_bits_empty(match->tag_bits, COUNT_OF(match->tag_bits));
}

void NengLogMatchClearTagBit(NengLogMatch *match)
{
    __neng_log_bits_clear(match->tag_bits, COUNT_OF(match->tag_bits));
}

// Level Bits
int NengLogMatchSetLevelBit(NengLogMatch *match, int mod)
{
    return __neng_log_bits_set(match->level_bits, COUNT_OF(match->level_bits), mod);
}

int NengLogMatchGetLevelBit(NengLogMatch *match, int mod)
{
    return __neng_log_bits_get(match->level_bits, COUNT_OF(match->level_bits), mod);
}

int NengLogMatchLevelBitIsEmpty(NengLogMatch *match)
{
    return __neng_log_bits_empty(match->level_bits, COUNT_OF(match->level_bits));
}

void NengLogMatchClearLevelBit(NengLogMatch *match)
{
    __neng_log_bits_clear(match->level_bits, COUNT_OF(match->level_bits));
}