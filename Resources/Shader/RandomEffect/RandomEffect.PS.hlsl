#include "RandomEffect.hlsli"

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};


//https://www.ronja-tutorials.com/post/024-white-noise/
float rand2dTo1d(float2 value){
    float2 dotDir = float2(12.9898f, 78.233f);
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453f);
    return random;
}

struct RandomEngine{
    //値
    float value;
    //ノイズの強さ
    float strength;
	//テクスチャと乗算させるか
    bool isUseTexture;
};

ConstantBuffer<RandomEngine> gRandomEngine : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);


PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //ランダムの値
    float random = rand2dTo1d(input.texcoord * gRandomEngine.value);
   
    //テクスチャを使って乗算させるかどうか
    if (gRandomEngine.isUseTexture == true){
        //テクスチャの色を取得
        float4 textureColor = gTexture.Sample(gSample, input.texcoord);
        //色にする
        output.color = float4(random, random, random, 1.0f) * textureColor;
    }
    //しない場合
    else{
        output.color = float4(random, random, random, 1.0f);
    }
    
    return output;
}