#include "RandomEffect.hlsli"


//三角形で覆われればOK
static const uint NUM_VERTEX = 3;
static const float4 POSITIONS[NUM_VERTEX] =
{
    //左上
    { -1.0f, 1.0f, 0.0f, 1.0f },
    //右上
    { 3.0f, 1.0f, 0.0f, 1.0f },
    //左下
    { -1.0f, -3.0f, 0.0f, 1.0f },
    
};

static const float2 TEXCOORDS[NUM_VERTEX] =
{
    //左上
    { 0.0f, 0.0f },
    //右上
    { 2.0f, 0.0f },
    //左下
    { 0.0f, 2.0f },
};


VertexShaderOutput main(uint vertexId : SV_VertexID)
{
    VertexShaderOutput output;
    
    //float32_t3 damyPosition = gPosition.position;
    output.position = POSITIONS[vertexId];
    output.texcoord = TEXCOORDS[vertexId];
    return output;
}