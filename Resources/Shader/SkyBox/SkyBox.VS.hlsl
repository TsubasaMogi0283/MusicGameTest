#include "SkyBox.hlsli"

//texcoordが3次元ベクトルだということに注意!

struct VertexShaderInput{
    float4 position : POSITION0;
    float3 texcoord : TEXCOORD0;
};

//座標返還を行うVS
struct TransformationMatrix{
	//32bitのfloatが4x4個
    float4x4 world;
};

struct Camera{
	//必要なのはこの3つ
	//ビュー行列
    float4x4 viewMatrix_;
	//射影行列
    float4x4 projectionMatrix_;
	//正射影行列
    float4x4 orthographicMatrix_;
};


ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

VertexShaderOutput main(VertexShaderInput input){
    VertexShaderOutput output;
    float4x4 viewProjecion = mul(gCamera.viewMatrix_, gCamera.projectionMatrix_);
    float4x4 wvp = mul(gTransformationMatrix.world, viewProjecion);
    
    output.position = mul(input.position, wvp).xyww;
    output.texcoord = input.position.xyz;
    return output;
}