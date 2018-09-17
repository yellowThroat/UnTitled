#include "000_Header.hlsl"

struct PixelInput
{
    float4 wPosition : POSITION0;
    float4 vPosition : POSITION1;
    float4 pPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 view : VIEW0;
    float4 viewPosition : LIGHTVIEW0;
};

PixelInput VS_Bone(VertexTextureNormalTangentBlend input)
{
    PixelInput output;
    
    output.wPosition = WORLD(input.position, Bones[BoneNumber]);
    output.vPosition = VIEW(output.wPosition);
    output.pPosition = PROJECTION(output.vPosition);

    output.viewPosition = ProjectionVP(input.position, Bones[BoneNumber]);

    output.normal = WorldNormal(input.normal, Bones[BoneNumber]);
    output.uv = input.uv;
    output.view = normalize(GetViewPosition() - output.wPosition.xyz);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 myColor = _diffuseMap.Sample(_diffuseSampler, input.uv);
    
    float4 color = myColor;
    Diffuse(color, input.normal);
    
    //Specular(color.rgb, input.normal, input.view);
    
    //int i = 0;
    //for (i = 0; i < _pointLightCount; i++)
    //    PointLighting(color.rgb, _pointLights[i], input.wPosition.xyz, input.normal);

    //for (i = 0; i < _spotLightCount; i++)
    //    SpotLighting(color.rgb, _spotLights[i], input.wPosition.xyz, input.normal);
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

    return color;
}