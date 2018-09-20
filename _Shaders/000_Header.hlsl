cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
    matrix _viewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer VS_Bone : register(b2)
{
    matrix Bones[256];
}

cbuffer VS_Render : register(b3)
{
    int BoneNumber;
}

cbuffer VS_isBone : register(b4)
{
    uint isBone;
}

cbuffer VS_Time : register(b5)
{
    float _elapsedTime;
    float _runningTime;
}

cbuffer PS_Light : register(b0)
{
    float4 _sunColor;
    
    float3 _direction;
    float _overcast;
    
    float3 _dir;
    float _sunIntensity;
}

cbuffer PS_Material : register(b1)
{
    float4 _ambient;
    float4 _diffuse;
    float4 _specular;
    float4 _emissive;
	
    float _shininess;
    float _detailFactor;

    uint _haveDiffuse;

    float MaterialPadding;
}

Texture2D _diffuseMap : register(t0);
Texture2D _specularMap : register(t1);
Texture2D _emissiveMap : register(t2);
Texture2D _normalMap : register(t3);
Texture2D _detailMap : register(t4);
Texture2D _depthMap : register(t5);

SamplerState _diffuseSampler : register(s0);
SamplerState _specularSampler : register(s1);
SamplerState _emissiveSampler : register(s2);
SamplerState _normalSampler : register(s3);
SamplerState _detailSampler : register(s4);

///////////////////////////////////////////////////////////////////////////////////////////////////

struct Vertex
{
    float4 position : POSITION0;
};

struct VertexColor
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct VertexColorNormal
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
};

struct VertexTexture
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexColorTextureNormal
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexTextureNormalBlend
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
};

struct VertexTextureNormalTangentBlend
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
};

float3 GetViewPosition()
{
    return _viewInverse._41_42_43;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float4 WORLD(float4 position)
{
    position = mul(position, _world);

    return position;
}

float4 WORLD(float4 position, float4x4 world)
{
    position = mul(position, world);

    return position;
}


float4 VIEW(float4 wPosition)
{
    wPosition = mul(wPosition, _view);

    return wPosition;
}

float4 PROJECTION(float4 wPosition)
{
    wPosition = mul(wPosition, _projection);

    return wPosition;
}

float4 WV(float4 position)
{
    position = WORLD(position);
    position = VIEW(position);

    return position;
}

float4 WV(float4 position, float4x4 world)
{
    position = WORLD(position, world);
    position = VIEW(position);

    return position;
}

float4 WVP(float4 position)
{
    position = WORLD(position);
    position = VIEW(position);
    position = PROJECTION(position);

    return position;
}

float4 WVP(float4 position, float4x4 world)
{
    position = WORLD(position, world);
    position = VIEW(position);
    position = PROJECTION(position);

    return position;
}

float3 WorldNormal(float3 normal)
{
    normal = mul(normal, (float3x3) _world);

    return normal;
}

float3 WorldNormal(float3 normal, float4x4 world)
{
    normal = mul(normal, (float3x3) world);

    return normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PointLight
{
    float3 Position;
    float Intensity;
    float3 Color;
    float Range;
};

cbuffer PS_PointLights : register(b10)
{
    PointLight _pointLights[32];

    int _pointLightCount;
}

struct SpotLight
{
    float3 Position;
    float InnerAngle;
    float3 Color;
    float OuterAngle;
    float3 Direction;
    float Range;
};

cbuffer PS_SpotLight : register(b11)
{
    SpotLight _spotLights[32];
    
    int _spotLightCount;
}

void PointLighting(inout float3 color, PointLight light, float3 position, float3 normal)
{
    float dist = distance(light.Position, position);
    float intensity = saturate((light.Range - dist) / light.Range);

    intensity = pow(intensity, (1 / light.Intensity));

    color = saturate(color + light.Color * intensity);
}

void SpotLighting(inout float3 color, SpotLight light, float3 position, float3 normal)
{
    float3 lightDir = normalize(position - light.Position);
    float dist = distance(light.Position, position);
    float distRatio = dist / light.Range;

    float3 intensity = 0;
    float lightAngle = dot(light.Direction, lightDir);
    lightAngle = acos(lightAngle);

    float outer = radians(light.OuterAngle) / 2.0f;
    float inner = radians(light.InnerAngle) / 2.0f;
    [branch]
    if (lightAngle >= 0.0f && lightAngle <= outer && dist <= light.Range)
    {
        intensity = smoothstep(outer, inner, lightAngle);
    }
    intensity = intensity * smoothstep(1.0f, 0.0f, distRatio);

    color = saturate(color + light.Color * intensity);
}

Texture2D _projectionMap : register(t6);
SamplerState _projectionSampler : register(s6);

cbuffer VS_VPTexture : register(b10)
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
}

float4 ProjectionVP(float4 position, matrix world)
{
    position = WORLD(position, world);
    position = mul(position, View);
    position = mul(position, Projection);

    return position;
}

void ProjectionTexture(inout float4 color, float4 proj, bool isShadow)
{
    float4 tex = float4(1, 1, 1, 1);
    float2 uv;
    uv.x = proj.x / proj.w / 2.0f + 0.5f;
    uv.y = -proj.y / proj.w / 2.0f + 0.5f;
    
    if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
    {
        if (!isShadow)
        {
            color = _projectionMap.Sample(_projectionSampler, uv);
        }
        if (isShadow)
        {
            tex = _projectionMap.Sample(_projectionSampler, uv);
            color *= tex;
            clamp(color, _ambient, 1);
        }
    }
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////

cbuffer PS_Fog : register(b12)
{
    float4  _fogColor;
    float _fogDensity;
    float3 Padding;
}

void LinearFog(inout float3 color, float3 vPosition)
{
    float factor = pow(1.0f / 2.71828, (vPosition.z * _fogDensity) * vPosition.z * _fogDensity);

    color = lerp(_fogColor.rgb, color, factor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Diffuse(inout float4 color, float3 normal)
{
    float intensity = dot(normal, -_direction);
    float4 diffuse = _ambient;

    if (intensity > 0.0f)
    {
        diffuse = _ambient + _diffuse * intensity;
        diffuse = saturate(diffuse);
    }

    color *= diffuse;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Specular(inout float4 color, float3 normal, float3 eye)
{
    float3 reflection = reflect(_direction, normal);
    float intensity = saturate(dot(reflection, eye));
    float specular = pow(intensity, _shininess);

    if(specular > 0.0f)
        color.rgb = saturate(color.rgb + _specular.rgb * specular);
}

void Specular(inout float4 color, float4 specularMap, float3 normal, float3 eye)
{
    float3 reflection = reflect(_direction, normal);
    float intensity = saturate(dot(reflection, eye));
    float specular = pow(intensity, _shininess);

    if (specular > 0.0f)
        color.rgb = saturate(color.rgb + specularMap.rgb * specular);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Bump(inout float4 tColor, float4 bumpMap, float3 normal, float3 tangent)
{
    float3 biNormal = cross(normal, tangent);
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    float3 bumpNormal = (bumpMap.x * tangent) + (bumpMap.y * biNormal) + (bumpMap.z * normal);

    bumpNormal = normalize(bumpNormal);

    Diffuse(tColor, bumpNormal);
}

void Bump(inout float4 tColor, float4 bumpMap, float3 normal, float3 tangent, float3 eye)
{
    float3 biNormal = cross(normal, tangent);
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    float3 bumpNormal = (bumpMap.x * tangent) + (bumpMap.y * biNormal) + (bumpMap.z * normal);

    bumpNormal = normalize(bumpNormal);

    Diffuse(tColor, bumpNormal);
    Specular(tColor, bumpNormal, eye);
}

void Bump(inout float4 tColor, float4 bumpMap, float4 specularIntensity, float3 normal, float3 tangent, float3 eye)
{
    float3 biNormal = cross(normal, tangent);
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    float3 bumpNormal = (bumpMap.x * tangent) + (bumpMap.y * biNormal) + (bumpMap.z * normal);

    bumpNormal = normalize(bumpNormal);

    Diffuse(tColor, bumpNormal);
    Specular(tColor, specularIntensity, bumpNormal, eye);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

float4 CatmullRom(float4 p0, float4 p1, float4 p2, float4 p3, float t)
{
    float4 r;

    r = 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0 * p2 - p3) *
    pow(t, 2.0f) + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * pow(t, 3.0f));

    return r;
}