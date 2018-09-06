#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
};

cbuffer PS_Color : register(b5)
{
    float4 Color;
}

PixelInput VS(Vertex input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return Color;
}