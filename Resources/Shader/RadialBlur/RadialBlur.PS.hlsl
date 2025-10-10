#include "RadialBlur.hlsli"




Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

//hlsliでは絶対に日本語を使わないでね


struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //中心点。ここを基準に放射状にブラーがかかる
    //移動できるようにしたら面白そう
    const float2 CENTER = float2(0.5f, 0.5f);
    
    //サンプリング数。多いほど滑らかだけど重くなる。
    const int NUM_SAMPLES = 10;
    
    //ぼかしの幅
    const float BLUR_WIDTH = 0.01f;
    
    //中心から現在のuvに対しての方向を計算。
    //方向と言ったら単位ベクトルだけど、ここでは正規化しないで遠いほどより遠くをサンプリングする
    float2 direction = input.texcoord - CENTER;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int sampleIndex = 0; sampleIndex < NUM_SAMPLES; ++sampleIndex){
        //現在のuvから先ほど計算した方向にサンプリング点を進めながらサンプリングしていく
        float2 texcoord = input.texcoord + direction * BLUR_WIDTH * float(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSample, texcoord).rgb;
    }
    
    //平均化する
    outputColor.rgb *= rcp(NUM_SAMPLES);
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    
    return output;
}