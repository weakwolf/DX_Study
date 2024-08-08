// 顶点着色器

#include "Triangle.hlsli"

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	// 增加一个维度是为了构建齐次坐标
	vOut.posH = mul(float4(vIn.pos, 1.0f), g_World);
	vOut.posH = mul(vOut.posH, g_View);
	vOut.posH = mul(vOut.posH, g_Proj);
	vOut.color = vIn.color; //alpha通道默认为1.0

	return vOut;
}