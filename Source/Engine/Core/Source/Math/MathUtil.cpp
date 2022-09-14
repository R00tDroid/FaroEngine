#include "MathUtil.hpp"

float pi = 0;

float Faro::Pi()
{
    if (pi == 0) SetPIPrecision(0);
    return pi;
}

void Faro::SetPIPrecision(int n)
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
