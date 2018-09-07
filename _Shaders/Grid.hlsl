#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 oPosition : POSITION0;
    float4 depth : DEPTH0;
    float4 color : COLOR0;
    float4 view : VIEW0;
};


PixelInput VS(VertexColor input)
{
    PixelInput output;

    output.oPosition = input.position;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    output.depth = output.position;

    output.color = input.color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float2 grid = input.oPosition.xz;

    float2 range = abs(frac(grid - 0.5f) - 0.5f);
    float2 speed = fwidth(grid);

    float2 pixelRange = range / speed;
    float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - 0.1f);

    float depth = input.depth.z / input.depth.w;

    float4 color = lerp(0, input.color, lineWeight);

    float alpha = 1.0f - depth;
    alpha = saturate(alpha * 200.0f);
    color.a = alpha;
    return color;

    //return input.color;
}