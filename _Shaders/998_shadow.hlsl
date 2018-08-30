#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 dPosition : POSITION0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;
    output.position = float4(1, 1, 1, 1);
    if (isBone)
        output.position = WVP(input.position, Bones[BoneNumber]);
    if (!isBone)
        output.position = WVP(input.position);
    
    output.dPosition = output.position;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float depth = input.dPosition.z / input.dPosition.w;
    return float4(depth, 0, 0, 1);
}