#include "DepthBasedOutline.hlsli"

//hlsliでは絶対に日本語を使わないでね


struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

struct Camera{
	//ビュー行列
    float4x4 viewMatrix_;
	//射影行列
    float4x4 projectionMatrix_;
	//正射影行列
    float4x4 orthographicMatrix_;
};

struct Material{
    float4x4 projectionInverse;
};

Texture2D<float4> gTexture : register(t0);
Texture2D<float> gDepthTexture : register(t1);

SamplerState gSample : register(s0);
SamplerState gSamplePoint : register(s1);


ConstantBuffer<Material> gMaterial : register(b0);

static const float PREWITT_HORIZONTAL_KERNEL[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0 / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0 / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0 / 6.0f },
};
static const float PREWITT_VERTICAL_KERNEL[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0 / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0 / 6.0f },
};

static const float2 INDEX3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};



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
            float ndcDepth = gDepthTexture.Sample(gSamplePoint, texCoord);
            //NDC->View。P^(-1)においてxとyはzwに影響を与えないので何でもよい。
            //なのでわざわざ行列を渡さなくてよい。
            //gMaterial.projectionInverseはCBufferを使って渡しておくこと
            float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
            //同次座標系からデカルト座標系へ変換
            float viewZ = viewSpace.z * rcp(viewSpace.w);
            difference.x += viewZ * PREWITT_HORIZONTAL_KERNEL[x][y];
            difference.y += viewZ * PREWITT_VERTICAL_KERNEL[x][y];
            
            
        }
    }
    
    
    
    //変化の長さをウェイトとして合成。
    //ウェイトの決定方法も色々と考えられる。
    //例えばdifference.xだけを使えば横方向のエッジが検出される
    float weight = length(difference);
    //差が小さいので大きくしている
    //後でCBufferで値を送るつもり
    weight = saturate(weight);
    
    //weightが大きい程暗く表示するようにしている
    //最もシンプルな合成
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSample,input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}