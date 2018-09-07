#include "000_Header.hlsl"

cbuffer PS_Grid : register(b2)
{
    float4 GridColor;
    uint GridSpacing;
    float GridThickness;
    bool GridView;

    float GridPadding;
}

cbuffer PS_Brush : register(b3)
{
    float4 BrushColor;
    float3 BrushPosition;
    uint BrushType;
    float BrushRange;
    float3 BrushBefore;
    bool BrushSelect;
    float BrushSpeed;
    float2 BrushPadding;

}

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float4 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 oPosition : POSITION0;
    float3 wPosition : POSITION1;
    float4 dPosition : POSITION2;
    float4 vPosition : LIGHTVIEW0;
};


PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.oPosition    = input.position;
    output.wPosition    = WORLD(input.position);
    output.position     = VIEW(float4(output.wPosition, 1));
    output.position     = PROJECTION(output.position);
    output.vPosition    = ProjectionVP(input.position, _world);
    output.normal       = WorldNormal(input.normal);    
    
    output.uv           = input.uv;
    output.color        = input.color;
    output.dPosition    = output.position;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 myColor;

    if (_haveDiffuse)
    {
        myColor = _diffuseMap.Sample(_diffuseSampler, input.uv.xy);
    }
    else
        myColor = input.color;

    float4 color;
    float depthValue;
    float detailBrightness;
    float4 detailColor;
    
    depthValue = input.dPosition.z / input.dPosition.w;

    if (depthValue < 0.95f)
    {
        detailColor = _detailMap.Sample(_detailSampler, input.uv.zw);

        detailBrightness = 1.8f;

        myColor = myColor * detailColor * detailBrightness;
    }
    color = myColor;
    Diffuse(color, input.normal);

    float4 brush = float4(0, 0, 0, 1);
    bool inBrush = false;
    if(BrushType == 1 && BrushSelect)
    {
        uint BeforeX = BrushBefore.x / (float) GridSpacing;
        uint BeforeY = BrushBefore.z / (float) GridSpacing;
        uint CurrentX = BrushPosition.x / (float) GridSpacing;
        uint CurrentY = BrushPosition.z / (float) GridSpacing;

        uint2 grid = uint2(0, 0);
        grid.x = input.oPosition.x / (float) GridSpacing;
        grid.y = input.oPosition.z / (float) GridSpacing;

        if(CurrentX >= BeforeX && CurrentY >= BeforeY)
        {
            if (grid.x >= BeforeX && grid.x <= CurrentX &&
               grid.y >= BeforeY && grid.y <= CurrentY)
            {
                inBrush = true;
                brush = BrushColor;
            }
        }
        if (CurrentX >= BeforeX && CurrentY < BeforeY)
        {
            if (grid.x >= BeforeX && grid.x <= CurrentX &&
               grid.y >= CurrentY && grid.y <= BeforeY)
            {
                inBrush = true;
                brush = BrushColor;
            }

        }
        if(CurrentX < BeforeX && CurrentY >= BeforeY)
        {
            if (grid.x >= CurrentX && grid.x <= BeforeX &&
               grid.y >= BeforeY && grid.y <= CurrentY)
            {
                inBrush = true;
                brush = BrushColor;
            }

        }
        if(CurrentX < BeforeX && CurrentY < BeforeY)
        {
            if (grid.x >= CurrentX && grid.x <= BeforeX &&
               grid.y >= CurrentY && grid.y <= BeforeY)
            {
                inBrush = true;
                brush = BrushColor;
            }

        }
        
    }

    if(BrushType == 2)
    {
        float x = input.oPosition.x - BrushPosition.x;
        float y = input.oPosition.z - BrushPosition.z;

        float dist = sqrt(x * x + y * y);

        if (dist <= BrushRange)
        {
            brush = BrushColor;
            inBrush = true;
        }
    }

    if (BrushType == 3)
    {
        if (input.oPosition.x >= BrushPosition.x - BrushRange &&
           input.oPosition.x < BrushPosition.x + BrushRange &&
           input.oPosition.z >= BrushPosition.z - BrushRange &&
           input.oPosition.z < BrushPosition.z + BrushRange)
        {
            brush = BrushColor;
            inBrush = true;
        }
    }


    if(GridView)
    {
        float2 grid = input.oPosition.xz / GridSpacing;

        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        float2 speed = fwidth(grid);

        float2 pixelRange = range / speed;
        float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - GridThickness);
        if (inBrush)
        {
            return lerp(GridColor, brush, lineWeight);

        }

        Diffuse(color, input.normal);

        int i = 0;
        for (i = 0; i < _pointLightCount; i++)
            PointLighting(color.rgb, _pointLights[i], input.oPosition, input.normal);

        for (i = 0; i < _spotLightCount; i++)
            SpotLighting(color.rgb, _spotLights[i], input.oPosition, input.normal);


        return lerp(GridColor, color, lineWeight);
    }

    if (inBrush)
        return saturate(color * brush);

    float2 projUv;
    projUv.x = input.vPosition.x / input.vPosition.w / 2.0f + 0.5f;
    projUv.y = -input.vPosition.y / input.vPosition.w / 2.0f + 0.5f;

    if (saturate(projUv.x) == projUv.x && saturate(projUv.y) == projUv.y)
    {
        float depthMap = _depthMap.Sample(_specularSampler, projUv).r;

        float lightDepth = input.vPosition.z / input.vPosition.w - 0.0001f;

        if (lightDepth > depthMap)
            color = myColor * _ambient;
    }


    int i = 0;
    for (i = 0; i < _pointLightCount; i++)
        PointLighting(color.rgb, _pointLights[i], input.wPosition, input.normal);

    for (i = 0; i < _spotLightCount; i++)
        SpotLighting(color.rgb, _spotLights[i], input.wPosition, input.normal);

    return color;
}