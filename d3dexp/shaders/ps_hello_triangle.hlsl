struct PSInput
{
	float4 colour : COLOUR;
};

float4 main(PSInput psi) : SV_TARGET
{
	return psi.colour;
}