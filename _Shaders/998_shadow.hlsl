#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 dPosition : POSITION0;
};

PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;
    output.position = float4(1, 1, 1, 1);
    if (isBone == 0)
    {
        output.position = WVP(input.position);

    }
    if (isBone == 1)
    {
        output.position = WVP(input.position, Bones[BoneNumber]);
    }
    if (isBone == 2)
    {
        matrix transform = 0;
        transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
        transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
        transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
        transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);

        output.position = WVP(input.position, transform);
    }

    
    output.dPosition = output.position;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return float4(0, 0, 0, 1);
}