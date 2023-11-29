#include "common.h"

#include <random>

thread_local std::mt19937 g_rng(std::random_device{}());

void setRNGSeed(int seed)
{
    g_rng.seed(seed);
}
