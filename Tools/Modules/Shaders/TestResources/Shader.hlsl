#include "Includes.hlsli"

PSin VSMain(VSin input)
{
    PSin output;
    output.position = float4(input.position, 1);
    output.color = input.color;
    return output;
}

float4 PSMain(PSin input) : SV_TARGET
{
    return input.color;
}
