#include "Random.hpp"
#include <ctime>

Faro::RandomStream::RandomStream()
{
    Seed(RandomSeed());
}

void Faro::RandomStream::Seed(int seed)
{
    rng_.seed(seed);
}

int Faro::RandomStream::RandomSeed()
{
    return time(0);
}

Faro::RandomStream Faro::global_rng_stream_;