#include "000_Header.hlsl"

cbuffer VS_Trail: register(b9)
{
    float4 TrailTop[100];
    float4 TrailBottom[100];
    int TrailStart;
    uint TrailSize;
    uint TrailLength;
    float TrailPadding;
}

struct VertexTrail
{
    float2 uv : TEXCOORD0;
    uint order : ORDER0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float alpha : ALPHA;
};

PixelInput VS(VertexTrail input)
{
    PixelInput output;
    int index0, index1, index2, index3;
    float t = (float) input.order / (float) (TrailSize - 1) * (float) (TrailLength - 1);

    index1 = (TrailStart + (uint) t) % TrailLength;
    index0 = (index1 - 1) % TrailLength;
    index2 = (index1 + 1) % TrailLength;
    index3 = (index1 + 2) % TrailLength;
    if (index0 < 0)
        index0 = TrailLength - 1;

    t = frac(t);

    float4 position = float4(0, 0, 0, 1);

    if(input.uv.y == 1.0f)
    {
        position = CatmullRom(TrailBottom[index0], TrailBottom[index1],
         TrailBottom[index2], TrailBottom[index3], t);
    }
    if(input.uv.y == 0.0f)
    {
        position = CatmullRom(TrailTop[index0], TrailTop[index1],
         TrailTop[index2], TrailTop[index3], t);
    }
    
    output.position = mul(position, _view);
    output.position = mul(output.position, _projection);
    
    output.uv = input.uv;
    output.alpha = 1.0f - (float) input.order / (float) (TrailSize - 1);
    return output;
}

Texture2D trailTexture : register(t9);
SamplerState trailSampler : register(s9);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = trailTexture.Sample(trailSampler, input.uv);

    if (color.a == 1.0f)
        color.a = input.alpha;
    return color;
}