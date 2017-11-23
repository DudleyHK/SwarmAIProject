/*
*/

cbuffer MatrixBuffer
{
	matrix worldMat;
	matrix viewMat;
	matrix projMat;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 uv       : TEXCOORD0;
	float4 normal   : NORMAL;
	matrix instanceWorldMat : INSTANCE;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv       : TEXCOORD0;
	float4 normal   : NORMAL;
};


PixelInputType ColourVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	matrix instanceWorldMat = mul(worldMat, input.instanceWorldMat);
	output.position = mul(input.position, instanceWorldMat);
	output.position = mul(output.position, viewMat);
	output.position = mul(output.position, projMat);

	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}