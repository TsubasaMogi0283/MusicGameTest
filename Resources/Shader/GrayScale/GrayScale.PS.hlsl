#include "GrayScale.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

//hlsliでは絶対に日本語を使わないでね

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSample, input.texcoord);
    float r = 0.2125f;
    float g = 0.7154f;
    float b = 0.0721f;
        
        
    float value = dot(output.color.rgb, float3(r, g, b));
    //モノクロ(グレースケール)
    output.color.rgb = float3(value, value, value);
    
    return output;
}