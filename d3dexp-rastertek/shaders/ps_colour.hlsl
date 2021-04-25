// input struct
struct ps_input
{
    float4 position : SV_POSITION;
    float4 colour   : COLOUR;
};


float4 main(ps_input pin) : SV_TARGET
{
	return pin.colour;
}