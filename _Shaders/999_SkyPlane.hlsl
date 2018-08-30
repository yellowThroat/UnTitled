#include "000_Header.hlsl"

Texture2D cloud1 : register(t5);
Texture2D cloud2 : register(t6);
Texture2D cloud3 : register(t7);

SamplerState cloudSampler1 : register(s5);
SamplerState cloudSampler2 : register(s6);
SamplerState cloudSampler3 : register(s7);

cbuffer PS_SkyBuffer : register(b3)
{
    float translation;
    float scale;
    float brightness;
    float padding;

};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXTURE0;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;

    //output.position = mul(input.position, Bones[BoneNumber]);
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 perturbValue;
    float4 cloudColor;
    float4 cloudColor2;
    float4 color; 

    input.uv.x = input.uv.x + translation;

    perturbValue = cloud2.Sample(cloudSampler2, input.uv);

    perturbValue = perturbValue * scale;

    perturbValue.xy = perturbValue.xy + input.uv.xy + translation;

    cloudColor = cloud1.Sample(cloudSampler1, perturbValue.xy);
    cloudColor2 = cloud3.Sample(cloudSampler3, perturbValue.xy);

    //cloudColor = cloudColor * brightness;
    color = lerp(cloudColor, cloudColor2, 0.5f);
    color *= brightness;

    return color;
}