#include "BoxFilter.hlsli"
#include "SmoothingIndex.hlsli"

//hlsliでは絶対に日本語を使わないでね

struct BoxFilterSelection{

	//Smoothing(平滑化)
	//輪郭などのくっきりしたところをぼかして滑らかな雰囲気を出すよ
    int filterSize;
    
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

static const int BOX_FILTER3x3 = 0;
static const int BOX_FILTER5x5 = 1;

//ConstantBuffer<構造体>変数名:register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);
ConstantBuffer<BoxFilterSelection> gBoxFilterSelection : register(b0);



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSample, input.texcoord);
    if (gBoxFilterSelection.filterSize == BOX_FILTER3x3)
    {
        //uvStepSizeの算出
        uint width, height;
        gTexture.GetDimensions(width, height);
        //rcp...逆数にする。正確では無いけど処理が速いよ
        float2 uvStepSIze = float2(rcp(float(width)), rcp(float(height)));
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
    else if (gBoxFilterSelection.filterSize == BOX_FILTER5x5)
    {
        //uvStepSizeの算出
        uint width, height;
        gTexture.GetDimensions(width, height);
        //rcp...逆数にする。正確では無いけど処理が速いよ
        float2 uvStepSIze = float2(rcp(float(width)), rcp(float(height)));
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
    return output;
}