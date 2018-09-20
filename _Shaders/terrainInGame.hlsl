#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 oPosition : POSITION0;
    float3 wPosition : POSITION1;
    float4 dPosition : POSITION2;
    float4 vPosition : LIGHTVIEW0;
};


PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.oPosition    = input.position.xyz;
    output.wPosition    = WORLD(input.position).xyz;
    output.position     = VIEW(float4(output.wPosition, 1));
    output.position     = PROJECTION(output.position);
    output.vPosition    = ProjectionVP(input.position, _world);
    output.normal       = WorldNormal(input.normal);    
    
    output.uv           = input.uv;
    output.color        = input.color;
    output.dPosition    = output.position;
    return output;
}

static const float weights[9] =
{
    0.022f, 0.04f, 0.123f, 0.201f,
    0.224f, 0.201f, 0.123f, 0.04f, 0.022f
};

Texture2D _landTexture0 : register(t10);
Texture2D _landTexture1 : register(t11);
Texture2D _landTexture2 : register(t12);
Texture2D _landTexture3 : register(t13);


float4 PS(PixelInput input) : SV_TARGET
{
    float4 textureBlend = input.color;
    float4 myColor = float4(1, 1, 1, 1);

    if (length(_diffuseMap.Sample(_diffuseSampler, input.uv)) > 0.0f)
    {
        myColor = _diffuseMap.Sample(_diffuseSampler, input.uv) * (1.0f - textureBlend.x - textureBlend.y - textureBlend.z - textureBlend.w);
    }
    if (length(_landTexture0.Sample(_diffuseSampler, input.uv)) > 0.0f)
    {
        myColor += _landTexture0.Sample(_diffuseSampler, input.uv) * textureBlend.x;
    }
    if (length(_landTexture1.Sample(_diffuseSampler, input.uv)) > 0.0f)
    {
        myColor += _landTexture1.Sample(_diffuseSampler, input.uv) * textureBlend.y;
    }
    if (length(_landTexture2.Sample(_diffuseSampler, input.uv)) > 0.0f)
    {
        myColor += _landTexture2.Sample(_diffuseSampler, input.uv) * textureBlend.z;
    }
    if (length(_landTexture3.Sample(_diffuseSampler, input.uv)) > 0.0f)
    {
        myColor += _landTexture3.Sample(_diffuseSampler, input.uv) * textureBlend.w;
    }

    float4 color;
    float depthValue;
    float detailBrightness;
    float4 detailColor;
    
    depthValue = input.dPosition.z / input.dPosition.w;

    if (depthValue < 0.95f)
    {
        detailColor = _detailMap.Sample(_detailSampler, input.oPosition.xz);

        detailBrightness = 1.8f;

        myColor = myColor * detailColor * detailBrightness;
    }
    color = myColor;
    Diffuse(color, input.normal);

    float2 projUv;
    projUv.x = input.vPosition.x / input.vPosition.w / 2.0f + 0.5f;
    projUv.y = -input.vPosition.y / input.vPosition.w / 2.0f + 0.5f;
        
    float4 depth = float4(0, 0, 0, 1);
    float width = 1.0f / 1280.0f;
    float height = 1.0f / 720.0f;
    int i = 0;

    for (i = -4; i <= 4; i++)
    {
        depth += weights[i + 4] * _depthMap.SampleLevel(_specularSampler, projUv + float2(width * (float) i,0),0);
        depth += weights[i + 4] * _depthMap.SampleLevel(_specularSampler, projUv + float2(0, height * (float) i), 0);
    }

    if (saturate(projUv.x) == projUv.x && saturate(projUv.y) == projUv.y)
    {
        depth = saturate(depth);
        depth = max(depth, _ambient);
        color *= depth;
    }


    for (i = 0; i < _pointLightCount; i++)
        PointLighting(color.rgb, _pointLights[i], input.wPosition, input.normal);

    for (i = 0; i < _spotLightCount; i++)
        SpotLighting(color.rgb, _spotLights[i], input.wPosition, input.normal);

    return color;
}