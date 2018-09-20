#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct VertexGrass
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 instance : TEXCOORD1;
};

PixelInput VS(VertexGrass input)
{
    PixelInput output;
    
    matrix billBoard = _viewInverse;

    float distinction = 0.0f;
    distinction += input.instance.x + input.instance.y + input.instance.z;
    distinction += length(input.instance);

    if (input.uv.y == 0)
    {
        float wind = sin(_runningTime + distinction) * 0.1f;
        wind = clamp(wind, -0.5f, 1.0f);
        input.position.x += wind;
    }

    billBoard._41_42_43 = float3(0, 0, 0);
    billBoard._12_21_23_42 = float4(0, 0, 0, 0);
    billBoard._22 = 1.0f;

    output.position = mul(input.position, billBoard);
    output.position = mul(output.position + float4(input.instance, 0), _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

Texture2D grassTexture : register(t10);
SamplerState grassSampler : register(s10);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = grassTexture.Sample(grassSampler, input.uv);

    clip(color.a - 0.5f);

    return float4(0.0f, 0.0f, 0.0f, 1);
}