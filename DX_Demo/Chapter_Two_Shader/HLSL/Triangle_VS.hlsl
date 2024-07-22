// 顶点着色器

#include "Triangle.hlsli"

VertextOut VS(VertexIn vIn)
{
	VertextOut vOut;
	// 增加一个维度是为了构建齐次坐标
	vOut.posH = float4(vIn.pos, 1.0f);
	vOut.color = vIn.color; //alpha通道默认为1.0

	return vOut;
}