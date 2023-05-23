struct VSin
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct PSin
{
    float4 position : SV_POSITION;
    float3 color : COLOR0;
};

PSin VSMain(VSin input)
{
    PSin output;
    output.position = float4(input.position, 1);
    output.color = input.color;
    return output;
}

float4 PSMain(PSin input) : SV_TARGET
{
    return float4(input.color, 1);
}
