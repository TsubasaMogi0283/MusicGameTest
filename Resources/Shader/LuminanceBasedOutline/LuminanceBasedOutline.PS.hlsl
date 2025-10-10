#include "LuminanceBasedOutline.hlsli"




Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

//hlsliでは絶対に日本語を使わないでね


struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};




static const float PREWITT_HORIZONTAL_KERNEL[3][3] = {
    { -1.0f / 6.0f, 0.0f, 1.0 / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0 / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0 / 6.0f },
};
static const float PREWITT_VERTICAL_KERNEL[3][3] ={
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0 / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0 / 6.0f },
};

static const float2 INDEX3x3[3][3] ={
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

float Luminance(float3 v){
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}




PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    
    //畳み込み
    float2 difference = float2(0.0f, 0.0f);
    
    //縦横それぞれの多々込みの結果を格納する
    //色を高度に変換して、畳込みを行っていく。
    //微分Filter用のkernelになっているので、やること自体は今までの畳み込みと同じ。
    //rcp...逆数にする。正確では無いけど処理が速いよ
    //uvStepSizeの算出
    uint width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSIze = float2(rcp(width), rcp(height));
    for (int x = 0; x < 3; ++x){
        for (int y = 0; y < 3; ++y){
            float2 texCoord = input.texcoord + INDEX3x3[x][y] * uvStepSIze;
            float3 fetchColor = gTexture.Sample(gSample, texCoord).rgb;
            float luminance = Luminance(fetchColor);
            difference.x += luminance * PREWITT_HORIZONTAL_KERNEL[x][y];
            difference.y += luminance * PREWITT_VERTICAL_KERNEL[x][y];
            
            
        }
    }
    
    
    
    //変化の長さをウェイトとして合成。
    //ウェイトの決定方法も色々と考えられる。
    //例えばdifference.xだけを使えば横方向のエッジが検出される
    float weight = length(difference);
    //差が小さいので大きくしている
    //後でCBufferで値を送るつもり
    weight = saturate(weight*6.0f);
    
    //weightが大きい程暗く表示するようにしている
    //最もシンプルな合成
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSample,input.texcoord).rgb;
    output.color.a = 1.0f;
    
    
    return output;
}