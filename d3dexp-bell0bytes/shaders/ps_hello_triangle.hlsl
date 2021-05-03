struct PSInput
{
    float4 position : SV_POSITION;
    float4 colour : COLOUR;
};

float4 main(PSInput pin) : SV_TARGET
{
	return pin.colour;
}