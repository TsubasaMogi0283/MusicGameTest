#include "Particle3d.hlsli"

//GPU用のデータ
struct ParticleForGPU{
    float4x4 World;
    float4 color;
};

//カメラ
struct Camera{
	//必要なのはこの3つ
	//ビュー行列
    float4x4 viewMatrix_;
	//射影行列
    float4x4 projectionMatrix_;
	//正射影行列
    float4x4 orthographicMatrix_;
};

//CBuffer
//StructuredBuffer...簡単に言えば配列みたいなやつ
StructuredBuffer<ParticleForGPU> gParticle : register(t0);
ConstantBuffer<Camera> gCamera : register(b0);

struct VertexShaderInput{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID){
    VertexShaderOutput output;
    
    //VP
    float4x4 viewProjection = mul(gCamera.viewMatrix_, gCamera.projectionMatrix_);
    //WVP
    float4x4 wvp = mul(gParticle[instanceId].World, viewProjection);
    
    output.position = mul(input.position, wvp);
    output.texcoord = input.texcoord;
	//法線の変換にはWorldMatrixの平衡移動は不要。拡縮回転情報が必要
	//左上3x3だけを取り出す
	//法線と言えば正規化をなのでそれを忘れないようにする
    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceId].World));
    output.color = gParticle[instanceId].color;
    
    return output;
}