#include "GaussianFilter.hlsli"
#include "SmoothingIndex.hlsli"



Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

//hlsliでは絶対に日本語を使わないでね

struct EffectSelection{
	
	//GaussianFilter
    //セパレートが出来るとBoxFilterよりこっちの方良い感じらしい
    //計算する回数が断然違う！！
    //それに綺麗にぼかすことが出来る
    int isGaussianFilter3x3;
    int isGaussianFilter5x5;

};
static const int GAUSSIAN_FILTER3x3 = 0;
static const int GAUSSIAN_FILTER5x5 = 1;


struct GaussianFilter{
    //種類
    int type;
    
    //標準偏差
    float sigma;
};

//ConstantBuffer<構造体>変数名:register(b0);
ConstantBuffer<GaussianFilter> gGaussianFilter : register(b0);


struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

struct SepiaColor{
    float r;
    float g;
    float b;
};




//円周率
static const float PI = 3.1415926535f;

float gauss(float x, float y, float sigma){
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    //exp...ネイピア数eを求める関数
    //精度が高く高速なので使うべき
    //C++でもあるよ！
    return exp(exponent) * rcp(denominator);
    
}


//exp(exponent)は以下と同じ
//const float e=2.71828182f;
//pow(e exponent)

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSample, input.texcoord);
    if (gGaussianFilter.type == GAUSSIAN_FILTER3x3)
    {
        //kernelを求める。weightは後で使う
        float weight = 0.0f;
        float kernel3x3[3][3];
        for (int x = 0; x < 3; ++x){
            for (int y = 0; y < 3; ++y){
                //2.0fは標準偏差。
                kernel3x3[x][y] = gauss(INDEX3x3[x][y].x, INDEX3x3[x][y].y, gGaussianFilter.sigma);
                weight += kernel3x3[x][y];
            }
        }
        //求めたkernelを使い、BoxFilterと同じく畳み込みを行う。
        //KERNEL3x3と定数にしていたところがkernel3x3に変わるだけ
         //uvStepSizeの算出
        uint width, height;
        gTexture.GetDimensions(width, height);
        //rcp...逆数にする。正確では無いけど処理が速いよ
        float2 uvStepSIze = float2(rcp(float(width)), rcp(float(height)));
        output.color.rgb = float3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;
        
        //for文でも同じ変数を使わないようにしよう
        for (int rx = 0; rx < 3; ++rx){
            for (int ry = 0; ry < 3; ++ry){
                float2 texcoord = input.texcoord + INDEX3x3[rx][ry] * uvStepSIze;
                float3 fetchColor = gTexture.Sample(gSample, texcoord).rgb;
                output.color.rgb += fetchColor * kernel3x3[rx][ry];

            }
        }
        
        //畳み込み後の値を正規化する。本来gauss関数は全体を合計すると(積分)1になるように設計されている。
        //しかし、無限の範囲は足せないので、kernel値の合計であるweightは1に満たない。
        //なので、合計が1になるように逆数を掛けて全体を底上げして調整する
        output.color.rgb *= rcp(weight);

    }
    else if (gGaussianFilter.type == GAUSSIAN_FILTER5x5)
    {
        //kernelを求める。weightは後で使う
        float weight = 0.0f;
        float kernel5x5[5][5];
        for (int x = 0; x < 5; ++x)
        {
            for (int y = 0; y < 5; ++y)
            {
                //2.0fは標準偏差。
                kernel5x5[x][y] = gauss(INDEX5x5[x][y].x, INDEX5x5[x][y].y, gGaussianFilter.sigma);
                weight += kernel5x5[x][y];
            }
        }
        //求めたkernelを使い、BoxFilterと同じく畳み込みを行う。
        //KERNEL3x3と定数にしていたところがkernel3x3に変わるだけ
         //uvStepSizeの算出
        uint width, height;
        gTexture.GetDimensions(width, height);
        //rcp...逆数にする。正確では無いけど処理が速いよ
        float2 uvStepSIze = float2(rcp(float(width)), rcp(float(height)));
        output.color.rgb = float3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;
        
        
        for (int rx = 0; rx < 5; ++rx)
        {
            for (int ry = 0; ry < 5; ++ry)
            {
                float2 texcoord = input.texcoord + INDEX5x5[rx][ry] * uvStepSIze;
                float3 fetchColor = gTexture.Sample(gSample, texcoord).rgb;
                output.color.rgb += fetchColor * kernel5x5[rx][ry];

            }
        }
        
        //畳み込み後の値を正規化する。本来gauss関数は全体を合計すると(積分)1になるように設計されている。
        //しかし、無限の範囲は足せないので、kernel値の合計であるweightは1に満たない。
        //なので、合計が1になるように逆数を掛けて全体を底上げして調整する
        output.color.rgb *= rcp(weight);

    }
    
    return output;
}