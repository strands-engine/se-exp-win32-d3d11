struct VS_INPUT
{
	float3 position : POSITION;
	float3 colour   : COLOUR;
};

struct VS_OUTPUT
{
	float3 colour   : COLOUR;
	float4 position : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT vin)
{
	VS_OUTPUT vout;
	
	vout.position = float4(vin.position, 1.0f);
	vout.colour = vin.colour;

	return vout;
}