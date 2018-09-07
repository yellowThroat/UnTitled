#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 view : VIEW0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    float4 world = WORLD(input.position);
    output.position = VIEW(world);
    output.position = PROJECTION(output.position);

    output.uv = input.uv;
    output.normal = normalize(mul(input.normal, (float3x3) _world));
    output.view = normalize(GetViewPosition() - world.xyz);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    Diffuse(color, input.normal);
    Specular(color, input.normal, input.view);
    return color;
}