#include "000_Header.hlsl"

cbuffer PS_Sky : register(b2)
{
    float4 Center;
    float4 Apex;
    float Height;
    float3 SkyPadding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 wPosition : OUTPUT0;

};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.wPosition = input.position;
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);



    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float height = saturate(input.wPosition.y * 2);
    return lerp(Center, Apex, height * Height);
}