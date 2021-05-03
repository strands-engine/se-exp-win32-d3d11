struct VSInput
{
    float3 position : POSITION;
    float3 colour : COLOUR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 colour : COLOUR;
};


VSOutput main( VSInput vin )
{
    VSOutput vout;
    
    vout.position = float4(vin.position, 1.0f);
    vout.colour = float4(vin.colour, 1.0f);
    
	return vout;
}