#include "Line3D.hlsli"

struct LineTransformMatrix{
    float4x4 WVP;
    float4x4 World;
    
};

struct VertexShaderInput{
    float4 position : POSITION0;
};

//カメラ用
struct CamerTransformationMatrix{
    float4x4 View;
    float4x4 Projection;
    float4x4 Orthographic;
};

//CBuffer
ConstantBuffer<LineTransformMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<CamerTransformationMatrix> gCamerTransformationMatrix : register(b1);


VertexShaderOutput main(VertexShaderInput input){
    
    VertexShaderOutput output;
    
    //World
    float4x4 world = gTransformationMatrix.World;
    
    //WVP
    float4x4 viewProjection = mul(gCamerTransformationMatrix.View, gCamerTransformationMatrix.Projection);
    float4x4 wvp = mul(world, viewProjection);
    
    
	//mul...組み込み関数(乗算)
    output.position = mul(input.position, wvp);
    
    return output;
}