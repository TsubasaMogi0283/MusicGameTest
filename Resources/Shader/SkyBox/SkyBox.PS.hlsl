#include "SkyBox.hlsli"


struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

struct Material{
    float4 color;
    float4x4 uvTransform;
};


TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float4(input.texcoord.xyz, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xyz);

    
    output.color = textureColor * gMaterial.color;
    return output;
}