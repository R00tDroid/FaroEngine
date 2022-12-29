struct VSin
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSin
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};
