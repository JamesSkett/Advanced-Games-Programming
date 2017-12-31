cbuffer CB0
{
	matrix WVPMatrix; //64 bytes
	matrix World; //64 bytes
} //128 bytes

struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(position, WVPMatrix);

	output.normal = mul(normal, World);

	output.texcoord = texcoord;

	return output;
}

float4 ModelPS(VOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse = texture0.Sample(sampler0, input.texcoord);

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}

float4 D2D_PS(VOut input) : SV_TARGET
{
	float4 diffuse = texture0.Sample(sampler0, input.texcoord);

	return diffuse;
}