#include "Line3D.hlsli"


//Material...色など三角形の表面の材質をけっていするもの
struct LineMaterial{
    
    float4 color;
};

////ConstantBuffer<構造体>変数名:register(b0);

ConstantBuffer<LineMaterial> gLineMaterial : register(b0);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};



 
PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
	
    
    output.color = gLineMaterial.color;
    

	
	
    return output;
}