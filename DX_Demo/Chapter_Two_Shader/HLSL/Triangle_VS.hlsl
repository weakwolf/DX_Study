// ������ɫ��

#include "Triangle.hlsli"

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	// ����һ��ά����Ϊ�˹����������
	vOut.posH = float4(vIn.pos, 1.0f);
	vOut.color = vIn.color; //alphaͨ��Ĭ��Ϊ1.0

	return vOut;
}