// ������ɫ��

#include "Triangle.hlsli"

VertextOut VS(VertexIn vIn)
{
	VertextOut vOut;
	// ����һ��ά����Ϊ�˹����������
	vOut.posH = float4(vIn.pos, 1.0f);
	vOut.color = vIn.color; //alphaͨ��Ĭ��Ϊ1.0

	return vOut;
}