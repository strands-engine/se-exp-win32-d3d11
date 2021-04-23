struct PS_INPUT
{
	float3 colour : COLOUR;
};

float4 main(PS_INPUT pin) : SV_TARGET
{
	return float4(pin.colour, 1.0f);
}