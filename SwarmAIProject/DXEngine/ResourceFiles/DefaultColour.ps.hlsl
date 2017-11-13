/*
*/

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv       : TEXCOORD0;
	float4 normal   : NORMAL;
};


float4 ColourPixelShader(PixelInputType input) : SV_TARGET
{
	return float4(1.0, 1.0, 0.0, 1.0);
}