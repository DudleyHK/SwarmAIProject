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

	output.position = mul(input.position, worldMat);
	output.position = mul(output.position, viewMat);
	output.position = mul(output.position, projMat);

	//output.colour = input.colour;
	return output;
}
