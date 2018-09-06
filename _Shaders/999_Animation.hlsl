#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 view : VIEW0;
    float3 tangent : TANGENT0;
};

PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);
    
    float4 world;
    world = mul(input.position, transform);
    output.position = mul(world , _view);
    output.position = mul(output.position, _projection);

    output.normal = normalize(mul(input.normal, (float3x3) transform));
    output.view = normalize(GetViewPosition() - world.xyz);
    output.uv = input.uv;
    output.tangent = normalize(mul(input.tangent, (float3x3) transform));
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 specular = _specularMap.Sample(_specularSampler, input.uv);
    if (length(specular) > 0)
        Specular(color.rgb, specular.rgb, input.normal, input.view);
    return color;
}