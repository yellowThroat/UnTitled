#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


PixelInput VS(VertexTexture input)
{
    PixelInput output;

    output.position = WVP(input.position);
    output.uv = input.uv;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    return color;
}