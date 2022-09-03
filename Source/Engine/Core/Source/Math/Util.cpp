#include "Util.h"
#include <math.h>
#include <ctime>

float pi = 0;

float Faro::Math::Pi()
{
    if (pi == 0) SetPIPrecision(0);
    return pi;
}



void Faro::Math::SetPIPrecision(int n)
{
    pi = 1;
    int sign = -1;

    for (int i = 1; i < 1000 * pow(10, n); i++)
    {
        pi += sign / (2.0 * i + 1.0);
        sign = -sign;
    }

    pi *= 4;
}

Faro::Math::RandomStream::RandomStream()
{
    Seed(RandomSeed());
}

void Faro::Math::RandomStream::Seed(int seed)
{
    rng_.seed(seed);
}

int Faro::Math::RandomStream::RandomSeed()
{
    return time(0);
}

Faro::Math::RandomStream Faro::Math::global_rng_stream_;