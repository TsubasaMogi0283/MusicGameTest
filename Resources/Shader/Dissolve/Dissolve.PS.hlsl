#include "Dissolve.hlsli"


struct Dissolve{
    //Edgeを使うかどうか
    bool isUseEdge;
	//Edgeの色
    float3 edgeColor;
    //Edgeの厚さ
    float edgeThinkness;
    //閾値
    float therashold;
    
};

Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture : register(t1);
SamplerState gSample : register(s0);
ConstantBuffer<Dissolve> gDissolve : register(b0);

//hlsliでは絶対に日本語を使わないでね


struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    
    float mask = gMaskTexture.Sample(gSample, input.texcoord);
    //maskの値が設定したtherashold以下の場合はdiscard
    if (mask <= gDissolve.therashold){
        discard;
    }
    
   
    //色の計算
    output.color = gTexture.Sample(gSample, input.texcoord);
    
    
    //Edgeを使う場合
    if (gDissolve.isUseEdge == true){
        //Edgeの厚さ
        float thinkness = gDissolve.edgeThinkness;
        float edge = 1.0f - smoothstep(gDissolve.therashold, gDissolve.therashold + thinkness, mask);
        float3 edgeColor = gDissolve.edgeColor;
        output.color.rgb += edge * edgeColor;
    }
    
    
    return output;
}