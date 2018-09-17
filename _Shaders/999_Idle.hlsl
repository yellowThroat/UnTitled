#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 view : VIEW0;
    float3 tangent : TANGENT0;
    float4 viewPosition : LIGHTVIEW0;
};

PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;
    
    float4 world = WORLD(input.position, Bones[BoneNumber]);
    output.position = VIEW(world);
    output.position = PROJECTION(output.position);
    output.viewPosition = ProjectionVP(input.position, Bones[BoneNumber]);


    output.normal = normalize(mul(input.normal, (float3x3) Bones[BoneNumber]));
    output.view = normalize(GetViewPosition() - world.xyz);
    output.tangent = normalize(mul(input.tangent, (float3x3) Bones[BoneNumber]));
    output.uv = input.uv;


    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 myColor = (1, 1, 1, 1);
    float4 color = myColor;
    Diffuse(color, input.normal);
    Specular(color, input.normal, input.view);

    float2 projUv;
    projUv.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
    projUv.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

    if (saturate(projUv.x) == projUv.x && saturate(projUv.y) == projUv.y)
    {
        float depthMap = _depthMap.Sample(_diffuseSampler, projUv).r;

        float lightDepth = input.viewPosition.z / input.viewPosition.w - 0.0001f;

        if (lightDepth > depthMap)
            color = myColor * _ambient;
    }

    //float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    //float4 bump = _normalMap.Sample(_normalSampler, input.uv);
    //float4 specular = _specularMap.Sample(_specularSampler, input.uv);
    //Bump(color, bump, input.normal, input.tangent, input.view);

        return color;
}