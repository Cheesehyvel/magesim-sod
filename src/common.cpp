#include "common.h"

#include <random>

bool isSchool(School var, School cmp)
{
    if ((cmp == SCHOOL_ARCANE || cmp == SCHOOL_FIRE) && var == SCHOOL_SPELLFIRE)
        return true;
    return var == cmp;
}

bool isSchool(School var, School cmp1, School cmp2)
{
    return isSchool(var, cmp1) || isSchool(var, cmp2);
}

thread_local std::mt19937 g_rng(std::random_device{}());

void setRNGSeed(int seed)
{
    g_rng.seed(seed);
}
