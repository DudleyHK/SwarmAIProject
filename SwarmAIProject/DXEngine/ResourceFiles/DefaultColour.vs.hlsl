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
	float3 instancePosition : TEXCOORD1;
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

	input.position.x += input.instancePosition.x;
	input.position.y += input.instancePosition.y;
	input.position.z += input.instancePosition.z;

	output.position = mul(input.position, worldMat);
	output.position = mul(output.position, viewMat);
	output.position = mul(output.position, projMat);

	//output.colour = input.colour;
	return output;
}