cbuffer CB0
{
	matrix WVPMatrix; //64 bytes
};

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut VShader(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1,1,1,1 };

	output.position = mul(WVPMatrix, position);
	// diffuse_amount = dot(directional_light_vector, normal);
	//diffuse_amount = saturate(diffuse_amount);
	//output.color = ambient_light_colour + (directional_light_colour * diffuse_amount);

	output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0, texcoord) * color;
}