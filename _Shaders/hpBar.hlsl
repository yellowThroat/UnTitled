#include "000_Header.hlsl"

cbuffer VS_Bar : register(b9)
{
    float3 Bar_position;
    float Bar_Padding;
    float Bar_height;
    float Bar_width;
    float Bar_length;
    float Bar_ratio;
}

struct VertexBar
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    uint isBottom : BOTTOM0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint isBottom : BOTTOM0;
};

PixelInput VS(VertexBar input)
{
    PixelInput output;

    float4 center;
    center = WORLD(input.position + float4(Bar_position, 1));

    output.uv = input.uv;
    output.isBottom = input.isBottom;

    float3 finalPos = center;
    float3 view = normalize(_viewInverse._31_32_33);
    float3 rotAxis = -normalize(_viewInverse._21_22_23);
    float3 side = normalize(_viewInverse._11_12_13);

    finalPos.y += Bar_height;
    finalPos -= side * Bar_length / 2.0f;
    finalPos -= rotAxis * Bar_width / 2.0f;

    if (input.isBottom)
    {
        finalPos += input.uv.x * Bar_length * side;
        finalPos += input.uv.y * Bar_width * rotAxis;
        finalPos += view * 0.01f;
    }
    else
    {
        finalPos += input.uv.x * Bar_length * Bar_ratio * side;
        finalPos += input.uv.y * Bar_width * rotAxis;
    }

    
    matrix VP = mul(_view, _projection);
    output.position = mul(float4(finalPos, 1), VP);

    return output;
}

Texture2D bottomTexture : register(t9);
SamplerState bottomSampler : register(s9);

Texture2D topTexture : register(t10);
SamplerState topSampler : register(s10);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 bottom = bottomTexture.Sample(bottomSampler, input.uv);
    float4 top = topTexture.Sample(topSampler, input.uv);
    float4 color = float4(0, 0, 0, 1);

    if(input.isBottom)
        color = bottom;
    if(!input.isBottom)
        color = top;

    
    return color;
}