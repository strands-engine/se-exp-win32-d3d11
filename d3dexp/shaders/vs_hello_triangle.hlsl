struct VSInput
{
	float3 pos    : POSITION;
	float3 colour : COLOUR;
};

struct VSOutput
{
	float4 colour : COLOUR;
	float4 pos    : SV_POSITION;
};

VSOutput main( VSInput vsi )
{
	VSOutput vso;

	vso.pos = float4(vsi.pos, 1.0f);
	vso.colour = float4(vsi.colour, 1.0);

	return vso;
}