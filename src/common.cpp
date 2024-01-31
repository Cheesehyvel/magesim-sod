#include "common.h"

#include <random>

bool isSchool(School var, School cmp)
{
    if ((cmp == SCHOOL_ARCANE || cmp == SCHOOL_FIRE) && var == SCHOOL_SPELLFIRE)
        return true;
    if ((cmp == SCHOOL_FROST || cmp == SCHOOL_FIRE) && var == SCHOOL_FROSTFIRE)
        return true;
    if ((cmp == SCHOOL_FROST || cmp == SCHOOL_ARCANE) && var == SCHOOL_SPELLFROST)
        return true;
    return var == cmp;
}

bool isSchool(School var, School cmp1, School cmp2)
{
    return isSchool(var, cmp1) || isSchool(var, cmp2);
}

double runeScale(int level)
{
    return ((double) level) / 60.0;
}

thread_local std::mt19937 g_rng(std::random_device{}());

void setRNGSeed(int seed)
{
    g_rng.seed(seed);
}
