#include "SepiaScale.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

//hlsliでは絶対に日本語を使わないでね

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

struct SepiaColor
{
    float r;
    float g;
    float b;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSample, input.texcoord);
    float r = 0.2125f;
    float g = 0.7154f;
    float b = 0.0721f;
    float value = dot(output.color.rgb, float3(r, g, b));
    
    //セピアの色
    SepiaColor sepiaColor = { 1.0f, 74.0f / 107.0f, 43.0f / 107.0f };
    output.color.rgb = value * float3(sepiaColor.r, sepiaColor.g, sepiaColor.b);
    
    
    return output;
}