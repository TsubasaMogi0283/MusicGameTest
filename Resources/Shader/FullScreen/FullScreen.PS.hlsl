#include "FullScreen.hlsli"
#include "EffectType.hlsli"
#include "SmoothingIndex.hlsli"



Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

//hlsliでは絶対に日本語を使わないでね

struct Effect{
    int type;
};

struct EffectSelection{
	//白黒(グレースケール)
    int isMonochrome;
	//セピア
    int isSepia;
	//端が暗くなる
    int isVegnette;
		

	//Smoothing(平滑化)
	//輪郭などのくっきりしたところをぼかして滑らかな雰囲気を出すよ
    int isBoxFilter3x3;
    int isBoxFilter5x5;

	//GaussianFilter
    //セパレートが出来るとBoxFilterよりこっちの方良い感じらしい
    //計算する回数が断然違う！！
    //それに綺麗にぼかすことが出来る
    int isGaussianFilter3x3;
    int isGaussianFilter5x5;

};


struct Vignette{
	//倍
    float scale;
    //乗
    float pow;
	
};

struct GaussianFilter{
    //標準偏差
    float sigma;
};

//ConstantBuffer<構造体>変数名:register(b0);
ConstantBuffer<Effect> gEffect : register(b0);
ConstantBuffer<Vignette> gVignette : register(b1);
ConstantBuffer<GaussianFilter> gGaussianFilter : register(b2);


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

float Luminance(float3 v){
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));

}

//exp(exponent)は以下と同じ
//const float e=2.71828182f;
//pow(e exponent)

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSample, input.texcoord);
    float r = 0.2125f;
    float g = 0.7154f;
    float b = 0.0721f;
        
        
    float value = dot(output.color.rgb, float3(r, g, b));
    //モノクロ(グレースケール)
    if (gEffect.type == MONOCHROME){
        output.color.rgb = float3(value, value, value);
    }
    //セピア
    else if (gEffect.type == SEPIA){
        SepiaColor sepiaColor = { 1.0f, 74.0f / 107.0f, 43.0f / 107.0f };
        output.color.rgb = value * float3(sepiaColor.r, sepiaColor.g, sepiaColor.b);
    }
    //Vignette
    else if (gEffect.type == VIGNETTE){
        //周囲を0に、中心になるほど明るくなるように計算で調整
        float2 current = input.texcoord * (1.0f - input.texcoord.yx);
        //currentだけで計算すると中心の最大値が0.0625で暗すぎるのでScaleで調整。
        //この例では16倍にして1にしている
        float vignette = current.x * current.y * gVignette.scale;
        //とりあえず0.8乗でそれっぽくしてみた
        vignette = saturate(pow(vignette, gVignette.pow));
        //係数として乗算
        output.color.rgb *= vignette;

    }
    else if (gEffect.type == BOX_FILTER3x3){
        //uvStepSizeの算出
        uint width, height;
        gTexture.GetDimensions(width, height);
        //rcp...逆数にする。正確では無いけど処理が速いよ
        float2 uvStepSIze = float2(rcp(width), rcp(height));
        output.color.rgb = float3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;
        
        //畳み込みとkernelが重要
        for (int x = 0; x < 3; ++x){
            for (int y = 0; y < 3; ++y){
                float2 texcoord = input.texcoord + INDEX3x3[x][y] * uvStepSIze;
                float3 fetchColor = gTexture.Sample(gSample, texcoord).rgb;
                output.color.rgb += fetchColor * KERNEL3x3[x][y];

            }
        }
        

    }
    else if (gEffect.type == BOX_FILTER5x5)
    {
        //uvStepSizeの算出
        uint width, height;
        gTexture.GetDimensions(width, height);
        //rcp...逆数にする。正確では無いけど処理が速いよ
        float2 uvStepSIze = float2(rcp(width), rcp(height));
        output.color.rgb = float3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;
        
        for (int x = 0; x < 5; ++x)
        {
            for (int y = 0; y < 5; ++y)
            {
                float2 texcoord = input.texcoord + INDEX5x5[x][y] * uvStepSIze;
                float3 fetchColor = gTexture.Sample(gSample, texcoord).rgb;
                output.color.rgb += fetchColor * KERNEL5x5[x][y];

            }
        }
        

    }
    else if (gEffect.type == GaussianFilter3x3){
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
        float2 uvStepSIze = float2(rcp(width), rcp(height));
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
    else if (gEffect.type == GaussianFilter5x5)
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
        float2 uvStepSIze = float2(rcp(width), rcp(height));
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