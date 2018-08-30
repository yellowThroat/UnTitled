#include "000_Header.hlsl"

cbuffer VS_Ocean : register(b12)
{
    float WaveFrequency;
    float WaveAmplitude;

    float2 TextureScale;
    
    float2 BumpSpeed;
    float BumpHeight;
}

cbuffer PS_Ocean : register(b2)
{
    float4 ShallowColor; //얕은색
    float4 DeepColor; //깊은색

    float FresnelBias; //편향
    float FresnelPower; //강도
    float FresnelAmount; //보정값

    float SpecularAmount; //반사강도

    float2 OceanSize; //버텍스의 전체크기
    float HeightRatio; //높이 보정값
    float ShoreBlend; //알파값 변화정도
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 oPosition : POSITION0;
    float4 vPosition : POSITION1;
    float2 texScale : TEXSCALE0;
    float3 normal : NORMAL0;
    float3 view : VIEW0;

    float2 bump[3] : BUMP0;
    float3 tangent[3] : TANGENT0;
};

struct Wave
{
    float Frequency; //파동
    float Amplitude; //진폭
    float Phase; //위상
    float2 Direction; //방향
};

float EvaluateWave(Wave wave, float2 position, float time)
{
    float s = sin(dot(wave.Direction, position) * wave.Frequency + time + wave.Phase);
    return wave.Amplitude * s;
}

float EvaluateDifferent(Wave wave, float2 position, float time)
{
    float c = cos(dot(wave.Direction, position) * wave.Frequency + time + wave.Phase);
    return wave.Amplitude * wave.Frequency * c;
}

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    Wave wave[3] =
    {
        { 0.05f, 1.00f, 0.50f, float2(1.0f, 0.0f) },
        { 2.0f, 0.50f, 1.30f, float2(-0.7f, 0.7f) },
        { 0.50f, 0.50f, 0.250f, float2(0.2f, 0.1f) },
    };

    wave[0].Frequency = WaveFrequency;
    wave[0].Amplitude = WaveAmplitude;
    wave[1].Frequency = WaveFrequency * 2.0f;
    wave[1].Amplitude = WaveAmplitude * 0.5f;
    wave[2].Frequency = WaveFrequency * 3.0f;
    wave[2].Amplitude = WaveAmplitude * 1.0f;

    float ddx = 0, ddy = 0;
    for (int i = 0; i < 3; i++)
    {
        input.position.y += EvaluateWave(wave[i], input.position.xz, _runningTime);

        float diff = EvaluateDifferent(wave[i], input.position.xz, _runningTime);
        ddx += diff * wave[i].Direction.x;
        ddy += diff * wave[i].Direction.y;
    }

    float3 T = float3(0, ddx, 1);
    float3 B = float3(1, ddy, 0);
    float3 N = float3(ddx, 1, ddy);


    float3x3 matTangent = float3x3
    (
        normalize(T) * BumpHeight,
        normalize(B) * BumpHeight,
        normalize(N)
    );

    output.tangent[0] = mul(_world[0].xyz, matTangent);
    output.tangent[1] = mul(_world[1].xyz, matTangent);
    output.tangent[2] = mul(_world[2].xyz, matTangent);



    float4 position = WVP(input.position);
    output.position = position;
    output.oPosition = input.position;
    output.vPosition = WV(input.position);
    output.texScale = input.uv * TextureScale;

    float4 wPosition = WORLD(input.position);
    output.view = normalize(GetViewPosition() - wPosition.xyz);
    
    float tempTime = fmod(_runningTime, 100);
    output.bump[0] = output.texScale + tempTime * BumpSpeed;
    output.bump[1] = output.texScale * 2.0f + tempTime * BumpSpeed * 4.0f;
    output.bump[2] = output.texScale * 4.0f + tempTime * BumpSpeed * 8.0f;
    
    return output;
}


Texture2D _heightMap : register(t10);
SamplerState _heightSampler : register(s10);

bool WithInBound(float3 position)
{
    return (position.x > 0.0f && position.z > 0.0f && position.x < OceanSize.x && position.z < OceanSize.y);
}

float EvaluateShoreBlend(float3 position)
{
    float2 temp = float2(position.x / OceanSize.x, position.z / OceanSize.y);
    float color = _heightMap.Sample(_heightSampler, temp).r / HeightRatio;

    return 1.0f - color * ShoreBlend;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 t0 = _normalMap.Sample(_normalSampler, input.bump[0]) * 2.0f - 1.0f;
    float4 t1 = _normalMap.Sample(_normalSampler, input.bump[1]) * 2.0f - 1.0f;
    float4 t2 = _normalMap.Sample(_normalSampler, input.bump[2]) * 2.0f - 1.0f;

    float3 normal = t0.xyz + t1.xyz + t2.xyz;
    float3x3 matTangent;
    matTangent[0] = input.tangent[0];
    matTangent[1] = input.tangent[1];
    matTangent[2] = input.tangent[2];
    normal = mul(matTangent, normal);
    normal = normalize(normal);


    float facing = 1.0f - saturate(dot(input.view, normal));
    float fresnel = FresnelBias + (1.0f - FresnelBias) * pow(facing, FresnelPower);

    float4 color = lerp(DeepColor, ShallowColor, facing);
    if (ShoreBlend > 0 && WithInBound(input.oPosition.xyz))
    {
        color.a = EvaluateShoreBlend(input.oPosition.xyz);
        color.rgb = lerp(2, color.rgb, color.a);
    }

    color.rgb = color.rgb * _ambient.rgb * fresnel;
    
    float intensity = saturate(dot(normal, -_direction));

    color.rgb = color.rgb + _diffuse.rgb * intensity;

    Specular(color.rgb, normal, input.view);

    return color;
}