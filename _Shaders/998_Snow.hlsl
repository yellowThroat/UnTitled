#include "000_Header.hlsl"

struct VertexSnow
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float2 Random : RANDOM0;
    float2 Scale : SCALE0;
};

cbuffer VS_SnowBuffer : register(b2)
{
    float3 Velocity;    // 움직일 속도
    float DrawDistance; // 보여질 거리

    float4 Color;       // 컬러

    float3 Origin;      // 원점(min)
    float Padding0;
    float3 Size;
    float Turbulence;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD0;
    float2 Scale : SCALE0;
    float Alpha : ALPHA0;
};

PixelInput VS(VertexSnow input)
{
    PixelInput output;

    input.Position.x *= Size.x;
    input.Position.y *= Size.y;
    input.Position.z *= Size.z;

    float3 tempVelocity = Velocity;
    float3 displacement = (_runningTime + 1000.0f) * tempVelocity;

    input.Position.x = (input.Position.x + cos(_runningTime - input.Random.x) * Turbulence) % Size.x;
    input.Position.z = (input.Position.z + cos(_runningTime - input.Random.y) * Turbulence) % Size.z;

    input.Position.x = Origin.x + (input.Position.x + displacement.x) % Size.x;
    input.Position.y = Origin.y + Size.y + (input.Position.y + displacement.y) % Size.y;
    input.Position.z = Origin.z + (input.Position.z + displacement.z) % Size.z;
    
    if (Velocity.x < 0)
        input.Position.x += Size.x;
    if (Velocity.z < 0)
        input.Position.z += Size.z;


    float4 center = mul(input.Position, _world);
    float3 view = center.xyz - GetViewPosition();
    float3 rotAxis = normalize(-tempVelocity);
    float3 side = normalize(cross(rotAxis, view));

    float3 finalPos = center;
    finalPos += (input.Uv.x - 0.5f) * side * input.Scale.x;
    finalPos += (1.5f - input.Uv.y * 1.5f) * rotAxis * input.Scale.y;

    matrix VP = mul(_view, _projection);
    output.Position = mul(float4(finalPos, 1), VP);

    output.Alpha = lerp(0, 1, (Size.y - input.Position.y) / Size.y);
    output.Alpha *= saturate(1 - output.Position.z / DrawDistance);
    output.Uv = input.Uv;
    return output;
}

Texture2D       rainMap     : register(t5);
SamplerState    rainSampler : register(s5);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = rainMap.Sample(rainSampler, input.Uv);

    return color;
}